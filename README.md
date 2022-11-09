# Paper-WiFi

Wifi-enabled e-paper display for I2C sensors. Arduino compatible software on Platformio. 3D printable case.

## Specifications

 * MCU: ESP32C3
 * Power supply: 2xAA, 3V
 * Screen size: 2.13"
 * Display resolution: 212(H) × 104(V) pixels
 * Connectivity: I2C, UART over USB, UART 
 * Sleep current: [TODO]
 
## Programming instructions

Firmware build and upload process is managed by [Platformio](https://platformio.org). Most probably you will encounter the board in deep sleep in which it is not visible via USB. Jump GPIO9 to GND and reset the board to put it into bootloader mode. Run `pio run -t upload` to compile and upload firmware. Remove jumper and reset the board to run. If deep sleep is not used, board can be programmed without GPIO9 jumper.

