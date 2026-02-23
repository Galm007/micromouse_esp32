# micromouse_esp32
Code for the microprocessor inside the actual MicroMouse.

How to run:
1) Download platformIO here: https://platformio.org/install
2) Connect ESP32 to your computer via USB.

```
# clone the project
git clone https://github.com/Galm007/micromouse_esp32 && cd micromouse_esp32

# Build, upload to ESP32, then monitor outputs
pio run -t upload && pio device monitor
```
