
#include <Arduino.h>
#include <math.h>

#include "AudioTools.h"
#include "AudioTools/AudioLibs/MaximilianDSP.h"

// ==========================================
// AUDIO OUTPUT
// ==========================================

I2SStream out;
Maximilian maximilian(out);

// PCM5102A pins
const int BCK_PIN = 26;
const int LCK_PIN = 25;
const int DIN_PIN = 27;

const int SAMPLE_RATE = 32000;

// ==========================================
// BUTTON MATRIX
// ==========================================

// Matrix rows
const int rowPins[4] = {33, 14, 22, 13};

// Matrix columns
const int colPins[4] = {21, 19, 18, 5};

// Currently selected chord
// -1 = no button pressed
volatile int activeChord = -1;

// ==========================================
// ROTARY ENCODER
// ==========================================

const int ENCODER_CLK = 23;
const int ENCODER_DT  = 17;
const int ENCODER_SW  = 16;

// Encoder mode
volatile bool arpeggiatorEnabled = false;

// Arpeggiator speed
volatile int arpSpeed = 6;

const int MIN_ARP_SPEED = 1;
const int MAX_ARP_SPEED = 20;

// Encoder rotation tracking
int encoderLastState = 0;
int encoderPosition = 0;

// Encoder push-button debounce
int lastButtonReading = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;

const unsigned long ENCODER_DEBOUNCE_MS = 30;

// ==========================================
// SYNTHESIZER
// ==========================================

maxiOsc oscillators[4];
maxiFilter filter;

// MIDI note numbers for each chord

const int chordNotes[16][4] = {

  // PROGRESSION 1

  {51, 55, 58, 63}, // 1: Eb
  {55, 58, 62, 67}, // 2: Gm
  {56, 60, 63, 67}, // 3: Abmaj7
  {56, 59, 63, 65}, // 4: Abm6

  // PROGRESSION 2

  {48, 51, 55, 60}, // 5: Cm
  {51, 55, 59, 63}, // 6: Ebaug
  {48, 51, 55, 60}, // 7: Cm
  {51, 55, 58, 61}, // 8: Eb7

  // PROGRESSION 3

  {56, 60, 63, 68}, // 9: Ab
  {56, 59, 63, 68}, // 10: Abm
  {51, 55, 58, 63}, // 11: Eb
  {48, 52, 55, 60}, // 12: C

  // PROGRESSION 4

  {53, 56, 60, 65}, // 13: Fm
  {56, 59, 63, 65}, // 14: Abm6
  {51, 55, 58, 63}, // 15: Eb
  {60, 64, 67, 70}  // 16: C7
};

// Chord names
const char* chordNames[16] = {
  "Eb",
  "Gm",
  "Abmaj7",
  "Abm6",
  "Cm",
  "Ebaug",
  "Cm",
  "Eb7",
  "Ab",
  "Abm",
  "Eb",
  "C",
  "Fm",
  "Abm6",
  "Eb",
  "C7"
};

// Precalculated chord frequencies
double chordFrequencies[16][4];

// ==========================================
// MIDI TO FREQUENCY
// ==========================================

double midiToFrequency(int midiNote) {
  return 440.0 * pow(2.0, (midiNote - 69) / 12.0);
}

// ==========================================
// ROTARY ENCODER SCANNER
// ==========================================

void scanEncoder() {

  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);

  int currentState = (clkState << 1) | dtState;

  static const int8_t encoderTable[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  int index = (encoderLastState << 2) | currentState;

  encoderPosition += encoderTable[index];

  encoderLastState = currentState;

  // Four transitions per detent
  if (encoderPosition >= 4) {

    if (arpSpeed < MAX_ARP_SPEED) {
      arpSpeed++;
    }

    encoderPosition = 0;

    Serial.print("Arpeggiator speed: ");
    Serial.print(arpSpeed);
    Serial.println(" notes/sec");
  }

  else if (encoderPosition <= -4) {

    if (arpSpeed > MIN_ARP_SPEED) {
      arpSpeed--;
    }

    encoderPosition = 0;

    Serial.print("Arpeggiator speed: ");
    Serial.print(arpSpeed);
    Serial.println(" notes/sec");
  }
}

// ==========================================
// ENCODER PUSH BUTTON
// ==========================================

void scanEncoderButton() {

  int reading = digitalRead(ENCODER_SW);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > ENCODER_DEBOUNCE_MS) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == LOW) {

        arpeggiatorEnabled = !arpeggiatorEnabled;

        if (arpeggiatorEnabled) {
          Serial.println("Mode: ARPEGGIATOR");
        }

        else {
          Serial.println("Mode: STEADY CHORD");
        }
      }
    }
  }

  lastButtonReading = reading;
}

// ==========================================
// MATRIX SCANNER
// ==========================================

void scanMatrix() {

  int detectedChord = -1;

  for (int r = 0; r < 4; r++) {

    digitalWrite(rowPins[r], LOW);

    delayMicroseconds(10);

    for (int c = 0; c < 4; c++) {

      if (digitalRead(colPins[c]) == LOW) {

        // Reverse columns to match physical numbering
        int buttonNumber = (r * 4) + (3 - c) + 1;

        if (buttonNumber <= 16) {
          detectedChord = buttonNumber - 1;
        }
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }

  activeChord = detectedChord;
}

// ==========================================
// AUDIO CALLBACK
// ==========================================

void play(float *output) {

  static int arpNoteIndex = 0;
  static int previousChord = -1;
  static bool previousMode = false;

  static uint32_t sampleCounter = 0;
  static uint32_t samplesPerNote = SAMPLE_RATE / 6;

  static int previousSpeed = 6;

  int chord = activeChord;
  bool arpMode = arpeggiatorEnabled;
  int speed = arpSpeed;

  // No button pressed
  if (chord < 0) {

    output[0] = 0.0;
    output[1] = 0.0;

    previousChord = -1;
    sampleCounter = 0;
    arpNoteIndex = 0;

    return;
  }

  // Reset when chord or mode changes
  if (chord != previousChord || arpMode != previousMode) {

    arpNoteIndex = 0;
    sampleCounter = 0;

    previousChord = chord;
    previousMode = arpMode;
  }

  // Update arpeggiator speed
  if (speed != previousSpeed) {

    samplesPerNote = SAMPLE_RATE / speed;
    previousSpeed = speed;
  }

  double sound = 0.0;

  // ========================================
  // STEADY CHORD MODE
  // ========================================

  if (!arpMode) {

    for (int i = 0; i < 4; i++) {

      sound += oscillators[i].sawn(
        chordFrequencies[chord][i]
      );
    }

    sound /= 4.0;
  }

  // ========================================
  // ARPEGGIATOR MODE
  // ========================================

  else {

    if (sampleCounter >= samplesPerNote) {

      sampleCounter = 0;

      arpNoteIndex++;

      if (arpNoteIndex >= 4) {
        arpNoteIndex = 0;
      }
    }

    // Same sawtooth oscillator function
    sound = oscillators[0].sawn(
      chordFrequencies[chord][arpNoteIndex]
    );

    // Match average chord output level
    sound *= 0.5;

    sampleCounter++;
  }

  // ========================================
  // FILTER
  // ========================================

  double filtered = filter.lores(sound, 1000, 0.8);

  // Stereo output
  output[0] = filtered * 0.5;
  output[1] = filtered * 0.5;
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 Synth Initialized!");

  // Configure matrix rows
  for (int r = 0; r < 4; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  // Configure matrix columns
  for (int c = 0; c < 4; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  // Configure rotary encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  // Initialize encoder state
  encoderLastState =
    (digitalRead(ENCODER_CLK) << 1) |
    digitalRead(ENCODER_DT);

  // Calculate all 16 chord frequencies
  for (int chord = 0; chord < 16; chord++) {

    for (int note = 0; note < 4; note++) {

      chordFrequencies[chord][note] =
        midiToFrequency(chordNotes[chord][note]);
    }
  }

  // Configure I2S audio
  auto cfg = out.defaultConfig(TX_MODE);

  cfg.is_master = true;
  cfg.pin_bck = BCK_PIN;
  cfg.pin_ws = LCK_PIN;
  cfg.pin_data = DIN_PIN;

  cfg.sample_rate = SAMPLE_RATE;
  cfg.channels = 2;
  cfg.bits_per_sample = 16;
  cfg.buffer_size = 256;

  out.begin(cfg);
  maximilian.begin(cfg);

  Serial.println("16 chords loaded!");
  Serial.println("Default mode: STEADY CHORD");
  Serial.println("Arpeggiator speed: 6 notes/sec");
  Serial.println("Press encoder to change playback mode.");
}

// ==========================================
// MAIN LOOP
// ==========================================

void loop() {

  // Scan chord matrix
  scanMatrix();

  // Scan rotary encoder
  scanEncoder();

  // Scan encoder push switch
  scanEncoderButton();

  // Print chord only when selection changes
  static int previousChord = -2;

  if (activeChord != previousChord) {

    if (activeChord == -1) {

      Serial.println("Chord released");
    }

    else {

      Serial.print("Button ");
      Serial.print(activeChord + 1);
      Serial.print(": ");

      Serial.println(chordNames[activeChord]);
    }

    previousChord = activeChord;
  }

  // Process audio
  maximilian.copy();
}
