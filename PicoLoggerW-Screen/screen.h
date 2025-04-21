// ==================================================== Screen Functions ====================================================

void showStartupScreen(String ssid, String password) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("+++++++++++++++++++++\n+    PicoLogger     +\n+      v-1.5.5      +\n+++++++++++++++++++++");
    display.display();
    delay(4000);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.display();
    display.print("++++ PicoLogger +++++\n");
    display.print("SSID: ");
    display.print(String(ssid));
    display.print("\n");
    display.print("PASS: ");
    display.print(String(password));
    display.print("\n");
    display.print("http://");
    display.print(WiFi.softAPIP()); 
    display.display();
}

void scrollUp(uint8_t pixels) {
  display.startscrollright(0x00, 0x07);
  display.stopscroll();
  for (int i = 0; i < display.height() - pixels; i++) {
    for (int j = 0; j < display.width(); j++) {
      uint8_t color = display.getPixel(j, i + pixels);
      display.drawPixel(j, i, color);
    }
  }
  display.fillRect(0, display.height() - pixels, display.width(), pixels, SSD1306_BLACK);
  display.display();
}

void checkAndScroll() {
  if ((display.getCursorY() + 8) > display.height()) {
    scrollUp(8);
    display.setCursor(0, display.getCursorY() - 8);
  }
}

void printout(const char *str){
  checkAndScroll();
  display.print(str);
  display.display();
}
