extern const uint8_t us_layout[];
extern const uint8_t es_layout[];
extern const uint8_t uk_layout[];
extern const uint8_t fr_layout[];
extern const uint8_t dk_layout[];
extern const uint8_t de_layout[];
extern const uint8_t br_layout[];
extern const uint8_t be_layout[];
extern const uint8_t fi_layout[];
extern const uint8_t it_layout[];
extern const uint8_t pt_layout[];
extern const uint8_t tr_layout[];

void setLayout(const uint8_t* layout);

 #ifndef KEYBOARD_h
 #define KEYBOARD_H
 
#ifndef USE_TINYUSB
  //if not using TinyUSB then default to the standard Arduino routines.
  #include <HID.h>
  #include <Keyboard.h>
  #include <Mouse.h>
#else

  #include <Arduino.h>
  
  #define MOUSE_LEFT 1
  #define MOUSE_RIGHT 2
  #define MOUSE_MIDDLE 4
  #define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)
  
  class TinyMouse_ {
    private:
      uint8_t _buttons;
      void buttons(uint8_t b);
    public:
      TinyMouse_(void);
      void begin(void);
      void end(void);
      void click(uint8_t b = MOUSE_LEFT);
      void move(signed char x, signed char y, signed char wheel = 0); 
      void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
      void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
      bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
  };
  
  extern TinyMouse_ Mouse;
  
  #define KEY_LEFT_CTRL   0x80
  #define KEY_LEFT_SHIFT    0x81
  #define KEY_LEFT_ALT    0x82
  #define KEY_LEFT_GUI    0x83
  #define KEY_RIGHT_CTRL    0x84
  #define KEY_RIGHT_SHIFT   0x85
  #define KEY_RIGHT_ALT   0x86
  #define KEY_RIGHT_GUI   0x87
  
  #define KEY_UP_ARROW    0xDA 
  #define KEY_DOWN_ARROW    0xD9 
  #define KEY_LEFT_ARROW    0xD8 
  #define KEY_RIGHT_ARROW   0xD7 
  #define KEY_BACKSPACE   0xB2 
  #define KEY_TAB       0xB3 
  #define KEY_RETURN      0xB0 
  #define KEY_ESC       0xB1 
  #define KEY_INSERT      0xD1 
  #define KEY_DELETE      0xD4 
  #define KEY_SPACE 	0x2c 
  #define KEY_MINUS 	0x2d 
  #define KEY_PAGE_UP     0xD3 
  #define KEY_PAGE_DOWN   0xD6 
  #define KEY_HOME      0xD2 
  #define KEY_END       0xD5 
  #define KEY_CAPS_LOCK   0xC1 
  #define KEY_PRINT_SCREEN 0x46 
  #define KEY_F1        0xC2
  #define KEY_F2        0xC3
  #define KEY_F3        0xC4
  #define KEY_F4        0xC5
  #define KEY_F5        0xC6
  #define KEY_F6        0xC7
  #define KEY_F7        0xC8
  #define KEY_F8        0xC9
  #define KEY_F9        0xCA
  #define KEY_F10       0xCB
  #define KEY_F11       0xCC
  #define KEY_F12       0xCD
  #define KEY_F13       0xF0
  #define KEY_F14       0xF1
  #define KEY_F15       0xF2
  #define KEY_F16       0xF3
  #define KEY_F17       0xF4
  #define KEY_F18       0xF5
  #define KEY_F19       0xF6
  #define KEY_F20       0xF7
  #define KEY_F21       0xF8
  #define KEY_F22       0xF9
  #define KEY_F23       0xFA
  #define KEY_F24       0xFB
  
  //  Low level key report: up to 6 keys and shift, ctrl etc at once
  typedef struct
  {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
  } KeyReport;
  
  /*
   * This class contains the exact same methods as the Arduino Keyboard.h class.
   */
  
  class TinyKeyboard_ : public Print
  {
  private:
    KeyReport _keyReport;
    void sendReport(KeyReport* keys);
  public:
    void setLayout(const uint8_t* layout);
    TinyKeyboard_(void);
    void begin(void);
    void end(void);
    size_t write(uint8_t k);
    size_t write(const uint8_t *buffer, size_t size);
    size_t press(uint8_t k);
    size_t release(uint8_t k);
    size_t rawpress(uint8_t k, uint8_t rawmodifiers);
    size_t rawrelease(uint8_t k, uint8_t rawmodifiers);
    void releaseAll(void);
  };
  
  extern TinyKeyboard_ Keyboard;

#endif
#endif

#ifndef USE_TINYUSB
  //if not using TinyUSB then default to the standard Arduino routines.
  #include <HID.h>
  #include <Keyboard.h>
  #include <Mouse.h>
#else

  #include <Adafruit_TinyUSB.h>

  #include "layouts/US/us.h"
  #include "layouts/ES/es.h"
  #include "layouts/UK/uk.h"
  #include "layouts/FR/fr.h"
  #include "layouts/DK/dk.h"
  #include "layouts/DE/de.h"
  #include "layouts/IT/it.h"
  #include "layouts/PT/pt.h"
  #include "layouts/FI/fi.h"
  #include "layouts/BE/be.h"
  #include "layouts/BR/br.h"
  #include "layouts/TR/tr.h"

 
const uint8_t* _layoutMap = us_layout;
  
  #define RID_KEYBOARD 1
  #define RID_MOUSE 2
  
  // GENERIC CONF
  uint8_t const desc_hid_report[] =
  {
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) ) // ONLY FOR USBHOST MOUSE ATTACK
  };
  
  // ONLY FOR BYPASS INTERFACE WHITELIST: https://www.youtube.com/watch?v=sc4aZd3XP70
  /*uint8_t const desc_system_report[] =
  {
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL( HID_REPORT_ID(2) ),
    TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(1) ),
  };*/

  // GENERIC CONF
  Adafruit_USBD_HID usb_hid;
  
  // ONLY FOR BYPASS INTERFACE WHITELIST: https://www.youtube.com/watch?v=sc4aZd3XP70
  //Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);
  //Adafruit_USBD_HID usb_keyboard(desc_system_report, sizeof(desc_system_report), HID_ITF_PROTOCOL_NONE, 2, false);
  
  TinyKeyboard_::TinyKeyboard_(void) {
  }

  TinyMouse_::TinyMouse_(void) {
  }
  
   void TinyKeyboard_::begin(void)
  {
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    //usb_keyboard.begin(); // ONLY FOR BYPASS INTERFACE WHITELIST: https://www.youtube.com/watch?v=sc4aZd3XP70
    while( !TinyUSBDevice.mounted() ) delay(1);
  }

  void TinyKeyboard_::setLayout(const uint8_t* layout) {
    _layoutMap = layout;
  }
  
  void TinyMouse_::begin(void) {
    _buttons = 0;
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }

  void TinyMouse_::move (int8_t x, int8_t y, int8_t wheel) {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.mouseReport(RID_MOUSE,_buttons,x,y,wheel,0);
  }
  
  void TinyMouse_::end(void) 
  {
  }
  
  void TinyMouse_::click(uint8_t b)
  {
    _buttons = b;
    move(0,0,0);
    _buttons = 0;
    move(0,0,0);
  }
  
  void TinyMouse_::buttons(uint8_t b)
  {
    if (b != _buttons)
    {
      _buttons = b;
      move(0,0,0);
    }
  }
  
  void TinyMouse_::press(uint8_t b) 
  {
    buttons(_buttons | b);
  }
  
  void TinyMouse_::release(uint8_t b)
  {
    buttons(_buttons & ~b);
  }
  
  bool TinyMouse_::isPressed(uint8_t b)
  {
    if ((b & _buttons) > 0) 
      return true;
    return false;
  }
  
  TinyMouse_ Mouse;//create an instance of the Mouse object
  
  void TinyKeyboard_::end(void)
  {
  }
  
  void TinyKeyboard_::sendReport(KeyReport* keys)
  {
    if ( TinyUSBDevice.suspended() )  {
      TinyUSBDevice.remoteWakeup();
    }
    while(!usb_hid.ready()) delay(1);
    usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
    delay(2);
  }
  
  // press() adds the specified key (printing, non-printing, or modifier)
  // to the persistent key report and sends the report.  Because of the way 
  // USB HID works, the host acts like the key remains pressed until we 
  // call release(), releaseAll(), or otherwise clear the report and resend.
  size_t TinyKeyboard_::press(uint8_t k)
  {
    uint8_t i;

	if(k>=0xB0 && k<=0xDA){			//it's a non-printing key
		if(k>=0xB5 && k<=0xBE){		//0xB5-0xBE reserved for special non printing keys asigned manually
			if(k==0xB5) k=0x65;	//0xB5 ==> 0x76 (MENU key)
			if(k==0xB6) k=0x46;	//0xB6 ==> 0x46 (PRINT Screen key)
		}
		else{
			k = k - 136;
		}
	}
	else {
	if(k>=0x80 && k<=0x87){			//it's a modifier
		_keyReport.modifiers |= (1<<(k-128));
		k = 0;
	}
	else{					//it's a printable key

	k = pgm_read_byte(_layoutMap + k);

	if (k & 0x80) {				// it's a capital letter or other character reached with shift
		_keyReport.modifiers |= 0x02;	// the left shift modifier
		k &= 0x7F;
	}
	if (k & 0x40) {				// altgr modifier (RIGHT_ALT)
		_keyReport.modifiers |= 0x40;	// the left shift modifier
		k &= 0x3F;
	}
	if (k == 0x03) { // special case 0x64
		k = 0x64;
	}
	}
	}

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {
		
		for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			setWriteError();
			return 0;
		}	
	}
	sendReport(&_keyReport);
	return 1;
  }
  
  size_t TinyKeyboard_::rawpress(uint8_t k, uint8_t rawmodifiers)
  {
    uint8_t i;

    _keyReport.modifiers = rawmodifiers;	

    if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
      _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
      _keyReport.keys[4] != k && _keyReport.keys[5] != k) {
		
      for (i=0; i<6; i++) {
        if (_keyReport.keys[i] == 0x00) {
	  _keyReport.keys[i] = k;
	  break;
	}
      }
		
      if (i == 6) {
        setWriteError();
	return 0;
      }	
    }
	
    sendReport(&_keyReport);
    return 1;
}
  
  // release() takes the specified key out of the persistent key report and
  // sends the report.  This tells the OS the key is no longer pressed and that
  // it shouldn't be repeated any more.
  size_t TinyKeyboard_::release(uint8_t k)
  {
    uint8_t i;

	if(k>=0xB0 && k<=0xDA){			//it's a non-printing key
		if(k>=0xB5 && k<=0xBE){		//0xB5-0xBE reserved for special non printing keys asigned manually
			if(k==0xB5) k=0x65;	//0xB5 ==> 0x76 (MENU key)
			if(k==0xB6) k=0x46;	//0xB6 ==> 0x46 (PRINT Scr key)
		}
		else{
			k = k - 136;
		}
	}
	else {
	if(k>=0x80 && k<=0x87){			//it's a modifier
		_keyReport.modifiers &= ~(1<<(k-128));
		k = 0;
	}
	else{					//it's a printable key

	k = pgm_read_byte(_layoutMap + k);
  
	if (k & 0x80) {					// it's a capital letter or other character reached with shift
		_keyReport.modifiers &= ~(0x02);	// the left shift modifier
		k &= 0x7F;
	}
	if (k & 0x40) {
		_keyReport.modifiers &= ~(0x40);       // the altgr shift modifier
		k &= 0x3F;
	}
	if (k == 0x03) { // special case 0x64
		k = 0x64;
	}

	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	}
	}
	}

	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i=0; i<6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
  }
  
  size_t TinyKeyboard_::rawrelease(uint8_t k, uint8_t rawmodifiers)
  {
    uint8_t i;

    _keyReport.modifiers = rawmodifiers;

    for (i=0; i<6; i++) {
      if (0 != k && _keyReport.keys[i] == k) {
        _keyReport.keys[i] = 0x00;
      }
    }

    sendReport(&_keyReport);
    return 1;
}

  
  void TinyKeyboard_::releaseAll(void)
  {
    _keyReport.keys[0] = 0;
    _keyReport.keys[1] = 0; 
    _keyReport.keys[2] = 0;
    _keyReport.keys[3] = 0; 
    _keyReport.keys[4] = 0;
    _keyReport.keys[5] = 0; 
    _keyReport.modifiers = 0;
    sendReport(&_keyReport);
  }
  
  size_t TinyKeyboard_::write(uint8_t c)
  {
    uint8_t p = press(c);  // Keydown
    release(c);            // Keyup
    return p;              // just return the result of press() since release() almost always returns 1
  }
  
  size_t TinyKeyboard_::write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (*buffer != '\r') {
        if (write(*buffer)) {
          n++;
        } else {
          break;
        }
      }
      buffer++;
    }
    return n;
  }
  TinyKeyboard_ Keyboard;//create an instance of the Keyboard object
#endif
