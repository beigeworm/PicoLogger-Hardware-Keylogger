<h2 align="center"> PicoLogger Hardware USB Keylogger and Bad-USB</h2>

<h3> Change Log </h3>

**Version 1.0** - Proof of Concept
- Added logging special characters
- Added Multi keypress handling for key combinations
- Added serial commands for Read, Clear logs and Format FS

**Version 1.1** - WiFi Update
- Created Webserver and AP for PicoW 
- New webpage : Keylogger - For viewing and clearing logs
- Added 'Clear Logs' handler
- Added serial commands for WiFi ON/OFF
- Added serial commands for changing WiFi password and SSID 

**Version 1.2** - Bad-USB Update
- Added Duckyscript handling and interpreter
- Added Payload-on-boot functionality
- New webpage : Payload Manager - Create, run, edit and delete Bad-USB payloads
- New webpage : Settings - Edit SSID, Wifi password, Format FS

**Version 1.3** - Virtual Keyboard
- New webpage : Virtual Keyboard - 'in browser' keyboard
- Toggle function keys for multi keypresses
- Revised code to enable start without host serial connection
- Added capital letter and special character support

**Version 1.4** - Remote Shell update
- Added remote shell powershell script 
- New webpage : Remote Shell - Connect to a Windows host and send commands over serial
- Added handlers for executing commands and receiving output over serial
- Added buffer system to keylogger to handle large amount of keystrokes continuously

**Version 1.5** - Windows Screenshots
- Added Windows screenshots over serial powershell script 
- Added base64 handler to decode screenshots from powershell
- New webpage : Screenshots - Connect to a Windows host and send screenshots over serial

**Version 1.6** - Optimisation
- Huge code revision - split 1700 lines of code into separate files for better future proofing further updates!
- Added USB Mouse support for using Keyboard/Mouse combo keyboards
- Bug Fix : fixed issue where pico 2W's would crash using wireless 2.4g keyboards
- Added restart button to settings menu

**Version 1.7** - OLED Screen Update
- Added SD1306 128x32 screen support
- Added Live debug screen output for collected keystrokes and Web UI activity
- Added Linux remote shell agent Bash script
- Added Linux remote shell 'deploy agent' handlder
- Added sudo password sniffer

**Version 1.8** - PicoLogger UI Update
- Added 5-way navigation switch support
- Picologger UI : User interface with menu for full control on-device!
- Added read and clear keylogger logs menu options
- Added menu options for Bad-USB, Remote Shell, Remote screenshots and system settings.

**Version 1.8** - File Browser + Web UI update
- Updated web UI Page selection
- New webpage : File Explorer - Download, edit, and delete all files on the file system.
- Added Screen rotation option in system menu for left handed use
