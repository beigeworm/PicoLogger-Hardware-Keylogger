<h2 align="center"> PicoLogger-Hardware-Keylogger </h2>

**SYNOPSIS**

A hardware USB keylogger using the Pi Pico &amp; Pi Pico-W.

**FEATURES**
- Flash, plug and play
- Serial control
- Webserver to display results (Pico-W only)
- Multi keypress handling for key combinations

**COMPONENT LIST**
1. A Raspberry Pi Pico OR Pico-W (RP2040 chip)
2. USB-A female port
3. Thin gauge wire
4. Soldering kit

**SETUP**
1. Download Arduino 1.8.x (Legacy Version)
2. Goto File > Preferences > then add this line to Additional Boards Manager URLs : `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`
3. Goto Tools > Board > Boards Manager > then search and install Raspberry Pi Pico/RP2040/RP2350
4. Goto Sketch > Include Library > Add ZIP Library > then add the files from `librares` folder
5. Goto Tools > then change all options below

*Options are the same for both Pico and Pico-W*
![Board Options](https://github.com/user-attachments/assets/338ac8da-0fd3-4127-92bc-af3d02d599d4)


**Wiring Diagram**

GND     =>  GND (PIN 1)

GPIO 19 =>  D+ (PIN 2)

GPIO 20 =>  D- (PIN 3)

VCC     =>  5v (PIN 4)

![Wiring](https://github.com/user-attachments/assets/9933fdf4-d53e-4493-8b7b-34116efe050c)
