<h2 align="center"> PicoLogger Hardware USB Keylogger and Bad-USB</h2>

![Prototype](https://github.com/user-attachments/assets/f50db2b6-a33b-47c6-a8b3-c47fbf5de7b6)

**SYNOPSIS**

A hardware USB keylogger and Bad-USB device for $8 using the Pi Pico &amp; Pi Pico-W!

**FEATURES**
- Flash, plug and play
- Bad-USB Functionality using Duckyscript
- Serial control
- Webserver to display results, payload management and settings management.
- Multi keypress handling for key combinations
- WiFi ON/OFF serial control (Pico-W only)
- Run Bad-USB payloads on boot
- Create, run, edit and delete Bad-USB payloads using the webserver (Pico-W only)
- Change wifi settings and others

**COMPONENT LIST**
1. A Raspberry Pi Pico OR Pico-W (RP2040 chip)
2. USB-A female port
3. Thin gauge wire

**SETUP** (easy method)
1. Connect your Pico or Pico-W to your computer via USB
2. A new drive `RPI-UF2` should appear
3. Copy the .uf2 file from the `UF2` folder for your specific Pico to the `RPI-UF2` drive
4. The Pico should now restart as a picologger!

**SETUP** (from source)
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

![baord-options](https://github.com/user-attachments/assets/cddcc7e6-7675-4a6b-911e-3c4eba17c1c1)

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
   Choose your COM port for your Pico and use 115200 baudrate
   - `read`                   : Output logged keys to serial
   - `clear`                  : Delete all logs
   - `format`                 : Format file system (LittleFS)
   - `wifion`                 : Enable WiFi AP (wifi version only)
   - `wifioff`                : Disable WiFi AP (wifi version only)
   - `ssid <yourssid>`        : Change the SSID (requires restart)
   - `password <newpassword>` : Change the password (requires restart)
   - `pobenabled`             : Enable payload on boot (non-wifi version only)
   - `pobdisabled`            : Disable payload on boot (non-wifi version only)
     
     *(All logs, WiFi state & settings will be saved to survive restarts and reflashing - use `format` command to reset defaults & remove ALL files)*
2. Web Interface (Pico-W only)
   - Connect to the WiFi network - (Default SSID > `PicoLogger` PASSWORD > `12345678`)
   - Goto `http://192.168.42.1` to view logs, manage payloads and change settings.

**Bad-USB Functionality**

The Pi Pico (non-W) can be setup to run a pre-coded payload on boot using the command `pobenabled`. 
You will need to flash the pico after editing the `payload()` function in the PicoLogger.ino file.
An example payload is provided to show some basic functionality using this (non-W) version

```
void payload() {
  // Example Payload - modify this as needed.
  delay(2000);
  // key press
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  // keys release (for multi keypresses)
  Keyboard.releaseAll();
  delay(1500);
  // print string
  Keyboard.print("notepad");
  delay(1000);
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
  delay(3000);
  // print string + return
  Keyboard.println("Hello World!");
}
```

With the Pico-W you can use the 'Payload Manager' webpage to create, run, edit and delete payloads as well as enable any payload on boot.
The Pico-W version has it's own handler so you can use basic Duckyscript commands to create Bad-USB scripts in the editor.

*example duckyscript*
```
REM Example Script
DELAY 1000
REM press Windows key + R key
GUI r
DELAY 500
REM Type a string of text
STRING notepad
REM press enter
ENTER
DELAY 3000
STRING hello world!
```

![picologger](https://github.com/user-attachments/assets/8ce1399d-f8d6-4f8e-95eb-0d32f3dc9d77)

**TO-DO**
1. USB Mouse support
2. Change Hardware ID etc (settings)
3. Add Virtual Keyboard (in browser)
