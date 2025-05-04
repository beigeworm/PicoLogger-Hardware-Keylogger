
void restartPico() {
  showWaitScreen("Restarting Pico...");
  watchdog_enable(1, 1);
  while (true) {}
}

// ============================================================ Buffer Functions =================================================================

void flushBuffer() {
  File f = LittleFS.open("/keys.txt", "a");
    if (bufferIndex > 0 && f) {
        f.write((const uint8_t*)keyBuffer, bufferIndex);
        f.flush();
        bufferIndex = 0;
    }
}

void writeBufferedKey(char key) {
  if (liveCaptureActive){
    if (lastPrintWasRoute) {
        printout("\n");
        lastPrintWasRoute = false;
        printout("[LOG] ");
    }
    if (liveCaptureStart) {
        liveCaptureStart = false;
        printout("[LOG] ");
    }     
    char str[2] = { key, '\0' };
    printout(str);
  }
    
    if (bufferIndex < BUFFER_SIZE - 1) {
        keyBuffer[bufferIndex++] = key;
        lastKeyTime = millis(); 
    } else {
        flushBuffer();
    }
}

void openLogFile() {
    File f = LittleFS.open("/keys.txt", "a");
    if (!f) {
        Serial.println("Error opening log file");
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
