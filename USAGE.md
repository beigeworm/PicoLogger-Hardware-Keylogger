<h1 align="center"> PicoLogger Web UI Usage Guide</h1>

**Connecting to the Web Interface**
- Connect to the WiFi network - (Default SSID > `PicoLogger` PASSWORD > `12345678`)
- Goto `http://192.168.42.1` to login. (Default USERNAME > `admin` PASSWORD > `password`)
- Use menu icon (top left) view logs, manage payloads and change settings etc.

<br></br>

**USB Keylogger**
<img width="1598" height="893" alt="logs" src="https://github.com/user-attachments/assets/11501cfd-5fc7-4f13-9726-61608f7b43e5" />

Place in between a keyboard and host system (PicoLogger is powered by the host).

All keystrokes from that keyboard will be shown in the output window.
- The shift key is recorded for capitals and special characters
- Function keys such as Escape are shown as \[ESC\]
- All keypresses are recorded on a single line, new lines are created when ENTER is pressed
- 'Clear Logs' button will delete all current logs

<br></br>

**Bad-USB Payload Manager**

<img width="1598" height="898" alt="payloads" src="https://github.com/user-attachments/assets/143949f7-f44a-4540-933a-e36c7091a8d5" />

You can use the 'Payload Manager' webpage to create, run, edit and delete payloads as well as enable any payload on boot.
Picologger has it's own handler so you can use basic Duckyscript commands to create Bad-USB scripts in the editor.

<br></br>

**Virtual Keyboard**

<img width="1596" height="898" alt="kb" src="https://github.com/user-attachments/assets/36f2affa-6bce-4b9f-9789-fdc146cbc19b" />

Virtual Keyboard sends keystrokes to the host! 
CTRL, ALT, GUI and SHIFT can be toggled for key combinations.

<br></br>


**Remote Shell**

<img width="1597" height="893" alt="shell" src="https://github.com/user-attachments/assets/d4be50a8-2da8-4119-9f4e-e3d8acdc7255" />

Navigate to the `Remote Shell` page. (you can use the hidden switch to hide the console once running)
- Click `Deploy Windows Agent` while connected to a Windows host.
- Wait 10 - 20 seconds for the agent to start on the host (the Powershell script for this can be found in `Scripts` folder. [Remote-Shell-Agent.ps1](Scripts/Remote-Shell-Agent.ps1)
- Use the command input to send Powershell commands to the host and receive output.


The Linux agent requires a sudo password - if it has not been automatically sniffed (see sudo pass sniffing below), it can be specified in the password box.
- Click `Deploy Linux Agent` while connected to a Linux host.
- Wait 10 - 20 seconds for the agent to start on the host (the Bash script for this can be found in `Scripts` folder. [Remote-Shell-Agent.sh](Scripts/Remote-Shell-Agent.sh)
- Use the command input to send Bash commands to the host and receive output.
- PicoLogger has an automatic password sniffer - it works by listening for any sudo command, and assumes the next line will be the password in between enter keypresses.
if the password has already been found the sniffer will be deactivated until Picologger has been powered off or restarted.

<br></br>

**Remote Screenshots**

<img width="1592" height="896" alt="Screenshots" src="https://github.com/user-attachments/assets/63609969-4b9a-4b71-80f7-c88713855d88" />

For Windows systems you can use the `Screenshots` page. 
- Click `Deploy Agent` while connected to a Windows host. (you can use the hidden switch to hide the console once running)
- Wait 10 - 20 seconds for the agent to start on the host (the Powershell script for this can be found in `Scripts` folder. [Remote-Screenshot-Agent.ps1](Scripts/Remote-Screenshot-Agent.ps1)
- Use the `Take Screenshot` button to receive screenshots of the host display.
*This function can be tempremental.. it may take 2-3 `Take Screenshot` button presses*

<br></br>


**Windows File Exfiltration**

<img width="1594" height="888" alt="exfil" src="https://github.com/user-attachments/assets/02f16fe7-73c2-498a-8241-f8233d5920f1" />

Navigate to the `Exfiltration` page. 
Here you can deploy a file exfiltration agent to run on windows. this allows you to navigate all drives on the host and save them to Picologger (3MB max for pico 2w) over serial 
- Click `Deploy Agent` while connected to a Windows host. (you can use the hidden switch to hide the console once running)
- Wait 10 - 20 seconds for the agent to start on the host (the Powershell script for this can be found in `Scripts` folder. [Remote-Exfiltration-Agent.ps1](Scripts/Remote-Exfiltration-Agent.ps1)
- You should now be able to navigate any specified drive (C:\ by default)

<br></br>

**File Explorer** (Pico W &amp; Pico 2 W only)

<img width="1599" height="892" alt="littlefs" src="https://github.com/user-attachments/assets/0b3f6646-3017-4e21-9ed5-83a85b649a0b" />

Navigate to the `File Explorer` page. 
This allows you to download, edit, and delete all files on the Picologger file system.

<br></br>

**Settings Page**

<img width="1594" height="897" alt="settings" src="https://github.com/user-attachments/assets/555d78ac-3c6b-4798-8a9c-2239d8a326b0" />

- Change WiFi SSID and Password
- Change Picologger login credentials
- Toggle WiFi (Connect to serial and send `wifion` command to turn back on)
- Format (Format entire picologger filesystem **You will loose all payloads, logs and settings!**)
- Keyboard Layout Selection
You can select the keyboard layout for using Bad-USB, Remote Shell, and Remote Screenshot functions. 
This changes the selected layout without needing to reboot PicoLogger!

