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
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
#define OLED_WIDTH  128
#define OLED_HEIGHT 32
Adafruit_USBD_MSC usb_msc;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, RST_PIN);
WebServer server(80);
extern const uint8_t _asciimap[] PROGMEM;
Adafruit_USBH_Host USBHost;
tusb_desc_device_t desc_device;

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
