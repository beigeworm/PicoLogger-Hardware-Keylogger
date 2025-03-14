<h2 align="center"> PicoLogger-Hardware-Keylogger </h2>

![picow](https://github.com/user-attachments/assets/bea89e7e-63c2-46bb-8663-b86e8e91c17d)

**SYNOPSIS**

A hardware USB keylogger using the Pi Pico &amp; Pi Pico-W.

**FEATURES**
- Flash, plug and play
- Serial control
- Webserver to display results (Pico-W only)
- Multi keypress handling for key combinations
- WiFi ON/OFF serial control

**COMPONENT LIST**
1. A Raspberry Pi Pico OR Pico-W (RP2040 chip)
2. USB-A female port
3. Thin gauge wire

**SETUP**
1. Download Arduino 1.8.x (Legacy Version)
2. Goto File > Preferences > then add this line to Additional Boards Manager URLs :
```
 https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```
3. Goto Tools > Board > Boards Manager > then search and install Raspberry Pi Pico/RP2040/RP2350
4. Goto Sketch > Include Library > Add ZIP Library > then add the files from `librares` folder
   - `Pico-PIO-USB-0.6.1.zip` (*Used for USB host input*)
   - `USBCrowKeyboard.zip` (*Used for HID passthrough to the host*)
5. Goto Tools > then change all options below

*Options are the same for both Pico and Pico-W*

![Board Options](https://github.com/user-attachments/assets/338ac8da-0fd3-4127-92bc-af3d02d599d4)


**Wiring Diagram**

```
Pico/Pico-W  ||    USB-A Port

GND          =>    GND (PIN 1)

GPIO 19      =>    D+ (PIN 2)

GPIO 20      =>    D- (PIN 3)

VCC          =>    5v (PIN 4)
```

![Wiring](https://github.com/user-attachments/assets/9933fdf4-d53e-4493-8b7b-34116efe050c)

**USAGE**

Place in between a keyboard and host system (PicoLogger is powered by the host)

1. Serial Control
   - Choose your COM port and 115200 baudrate
   - `read`    : Output logged keys to serial
   - `clear`   : Delete all logs
   - `format`  : Format file system (LittleFS)
   - `wifion`  : Enable WiFi AP
   - `wifioff` : Disable WiFi AP
     *(WiFi state will be saved to survive restarts)*
2. Web Interface (Pico-W only)
   - Connect to the WiFi network : SSID > `PicoLogger` PASSWORD > `12345678`
   - Goto `http://192.168.42.1` to view and clear logs

