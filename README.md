# esp32synth
The ESP32 Synthesizer is a custom-built electronic musical instrument designed around the ESP32 microcontroller. The project combines embedded systems programming, digital audio synthesis, and physical hardware interfacing to create an interactive instrument capable of generating musical chords through a 4×4 button matrix.
# Custom Hardware Synth Project
<img width="2160" height="2880" alt="IMG_7821" src="https://github.com/user-attachments/assets/d1ee5a67-b05e-452f-a6ae-5979fc0f64b1" />


---

## Hardware Used

### Interface I2S PCM5102A DAC Decoder GY-PCM5102

<p align="center">
  <img width="600" alt="Main Board" src="https://github.com/user-attachments/assets/46574a20-2fd0-4157-84c9-6804b67c569a" />
</p>

- [Buy on AliExpress](https://www.aliexpress.us/item/3256805917724211.html?spm=a2g0o.order_list.order_list_main.70.5da71802OAppXl&gatewayAdapt=glo2usa4itemAdapt)

---

### Basic Starter Kit for ESP32 ESP-32 Devkit v1 

<p align="center">
  <img width="700" alt="Additional Accessory" src="https://github.com/user-attachments/assets/5f6c12e1-a11c-4320-ad2e-45d3f28f87c3" />
</p>

- [Buy on AliExpress](https://www.aliexpress.us/item/3256805879357212.html?spm=a2g0o.order_list.order_list_main.145.5da71802OAppXl&gatewayAdapt=glo2usa4itemAdapt)

---

### 4x4 Matrix 16 Keypad Keyboard Module

<p align="center">
  <img width="1285" height="490" alt="image" src="https://github.com/user-attachments/assets/18486356-0b09-4261-bd20-170af4dbcc23" />

</p>

- [Buy on AliExpress]([https://www.aliexpress.us/item/3256805917724211.html?spm=a2g0o.order_list.order_list_main.70.5da71802OAppXl&gatewayAdapt=glo2usa4itemAdapt](https://www.aliexpress.us/item/3256805928656170.html?spm=a2g0o.productlist.main.9.6f4944d4l0cyrk&algo_pvid=222ac145-0fa6-4566-91cb-e8b96d9e31f0&algo_exp_id=222ac145-0fa6-4566-91cb-e8b96d9e31f0-8&pdp_ext_f=%7B%22order%22%3A%22276%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%211.30%211.09%21%21%211.30%211.09%21%40210328df17950363207437214e0faf%2112000059199096549%21sea%21US%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A859f8922%3Bm03_new_user%3A-29895%3BpisId%3A5000000218165384&curPageLogUid=AO9OePooC5F4&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006114970922%7C_p_origin_prod%3A))

---

### KY-040 360 Degrees EC11 Rotary Encoder Module

<p align="center">
  <img width="1262" height="439" alt="image" src="https://github.com/user-attachments/assets/8e0f7ac7-284b-459a-a15a-cd3a1412b117" />

</p>

- [Buy on AliExpress]([https://www.aliexpress.us/item/3256805879357212.html?spm=a2g0o.order_list.order_list_main.145.5da71802OAppXl&gatewayAdapt=glo2usa4itemAdapt](https://www.aliexpress.us/item/3256812187820082.html?spm=a2g0o.productlist.main.7.3ca52283rLRqoJ&algo_pvid=d0da04e5-e50d-4b7f-93e2-b226135afa07&algo_exp_id=d0da04e5-e50d-4b7f-93e2-b226135afa07-6&pdp_ext_f=%7B%22order%22%3A%22246%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%215.41%211.09%21%21%2135.93%217.24%21%402103274a17950364040235918e0baa%2112000058197161516%21sea%21US%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A859f8922%3Bm03_new_user%3A-29895%3BpisId%3A5000000218165391&curPageLogUid=nwPlwAqb6MsM&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005012374134834%7C_p_origin_prod%3A))

---

## Libraries Used

- [arduino-audio-tools](https://github.com/pschatzmann/arduino-audio-tools)  
- [Maximilian](https://github.com/pschatzmann/Maximilian)  

---

## Source Code

- [synthengine.cpp]([https://github.com/wprudencio/esp32-synth/blob/main/synth-base.cpp](https://github.com/jeshuawilliams50-lab/esp32synth/blob/main/synthengine.cpp))

---

## Troubleshooting

If you're using the I2S DAC interface linked above, **you may need to solder the bridges** shown below:

<p align="center">
  <img width="1360" alt="DAC Soldering Instructions" src="https://github.com/user-attachments/assets/a8513952-70d2-4831-b567-fbacfc36fee3" />
</p>

- Official guide: [Audio Tools Wiki – External DAC](https://github.com/pschatzmann/arduino-audio-tools/wiki/External-DAC#hiletgo-pcm5102-i2s-iis-lossless-digital-audio-dac-decoder)  
- It may work without soldering, but I soldered it to ensure proper functionality.
