/*
  ESP32 Servo Web Control (Custom UI)
  ------------------------------------
  - ESP32 works as a WiFi Access Point (AP)
  - Serves the custom "Device Control" web page
  - Open  -> servo moves to open position, blue LED on, clear LED off
  - Close -> servo moves to closed position, clear LED on, blue LED off
  Required library: ESP32Servo (install via Library Manager)
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ---- Access Point settings ----
const char* ssid     = "Badr_Servo";
const char* password = "badr12345";   // at least 8 characters

// ---- Pin assignments ----
const int servoPin  = 18;
const int clearLED  = 2;   // lights up on CLOSE
const int blueLED   = 4;   // lights up on OPEN

// ---- Servo angles ----
const int OPEN_ANGLE  = 90;
const int CLOSE_ANGLE = 0;

WebServer server(80);
Servo myServo;

// current state: "open" or "close"
String currentStatus = "close";

// ---- HTML page (matches the custom design) ----
const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Device Control</title>
<style>
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
    background: #f2f0f8;
  }
  .topbar {
    display: flex;
    align-items: center;
    justify-content: space-between;
    background: #3d2a86;
    color: #fff;
    padding: 18px 24px;
  }
  .topbar h1 { font-size: 26px; font-weight: 700; margin: 0; }
  .content {
    max-width: 960px;
    margin: 0 auto;
    padding: 40px 20px;
    text-align: center;
  }
  .content h2 { font-size: 30px; font-weight: 700; color: #1f1147; margin: 0 0 8px; }
  .content p.subtitle { color: #6b6480; font-size: 16px; margin: 0 0 16px; }
  .current-status {
    display: inline-block;
    margin-bottom: 24px;
    padding: 6px 16px;
    border-radius: 999px;
    font-size: 14px;
    font-weight: 600;
    background: #ece9f7;
    color: #3d2a86;
  }
  .cards { display: flex; gap: 24px; justify-content: center; flex-wrap: wrap; }
  .card { flex: 1 1 320px; max-width: 380px; border-radius: 16px; padding: 32px 24px; border: 1px solid; }
  .card.open { background: #eef7ee; border-color: #cfe8cf; }
  .card.close { background: #fbedec; border-color: #f3d3d0; }
  .icon-circle {
    width: 96px; height: 96px; margin: 0 auto 20px; border-radius: 50%;
    display: flex; align-items: center; justify-content: center;
    background: #fff; border: 2px solid;
  }
  .card.open .icon-circle { border-color: #4c9a4c; }
  .card.close .icon-circle { border-color: #c94b3f; }
  .icon-circle svg { width: 40px; height: 40px; }
  .card h3 { font-size: 26px; font-weight: 700; margin: 0 0 6px; }
  .card.open h3 { color: #3f8f3f; }
  .card.close h3 { color: #c0392b; }
  .card p { color: #55506a; font-size: 15px; margin: 0 0 24px; }
  .btn {
    display: block; width: 100%; border: none; border-radius: 8px;
    padding: 16px; font-size: 17px; font-weight: 600; color: #fff;
    cursor: pointer; transition: filter 0.15s ease;
  }
  .btn:hover { filter: brightness(0.92); }
  .btn:disabled { opacity: 0.6; cursor: not-allowed; }
  .btn.open-btn { background: #4c9a4c; }
  .btn.close-btn { background: #c0392b; }
  @media (max-width: 480px) {
    .topbar h1 { font-size: 20px; }
    .content h2 { font-size: 24px; }
  }
</style>
</head>
<body>

  <div class="topbar">
    <div></div>
    <h1>Device Control</h1>
    <div></div>
  </div>

  <div class="content">
    <h2>Control Panel</h2>
    <p class="subtitle">Click a button to control the device</p>
    <div class="current-status" id="statusBadge">جارِ التحميل...</div>

    <div class="cards">
      <div class="card open">
        <div class="icon-circle">
          <svg viewBox="0 0 24 24" fill="none" stroke="#4c9a4c" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <rect x="4" y="11" width="12" height="9" rx="2" fill="#4c9a4c" stroke="none"/>
            <path d="M8 11V7a4 4 0 0 1 7.5-2" />
          </svg>
        </div>
        <h3>OPEN</h3>
        <p>Open the device</p>
        <button class="btn open-btn" id="openBtn" onclick="controlDevice('open')">Open</button>
      </div>

      <div class="card close">
        <div class="icon-circle">
          <svg viewBox="0 0 24 24" fill="none" stroke="#c94b3f" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <rect x="4" y="11" width="16" height="9" rx="2" fill="#c94b3f" stroke="none"/>
            <path d="M7 11V7a5 5 0 0 1 10 0v4" />
          </svg>
        </div>
        <h3>CLOSE</h3>
        <p>Close the device</p>
        <button class="btn close-btn" id="closeBtn" onclick="controlDevice('close')">Close</button>
      </div>
    </div>
  </div>

<script>
  const statusBadge = document.getElementById('statusBadge');
  const openBtn = document.getElementById('openBtn');
  const closeBtn = document.getElementById('closeBtn');

  function renderStatus(status) {
    statusBadge.textContent = status === 'open' ? 'الحالة الحالية: مفتوح' : 'الحالة الحالية: مغلق';
  }

  function fetchStatus() {
    fetch('/status')
      .then(res => res.json())
      .then(data => { if (data && data.status) renderStatus(data.status); })
      .catch(() => { statusBadge.textContent = 'تعذر الاتصال بالخادم'; });
  }

  function controlDevice(action) {
    openBtn.disabled = true;
    closeBtn.disabled = true;

    fetch('/' + action)
      .then(res => res.json())
      .then(data => {
        if (data.success) { renderStatus(data.status); }
        else { alert('حدث خطأ: ' + data.error); }
      })
      .catch(() => alert('تعذر الاتصال بالخادم'))
      .finally(() => {
        openBtn.disabled = false;
        closeBtn.disabled = false;
      });
  }

  fetchStatus();
</script>

</body>
</html>
)rawliteral";

// ---- Handlers ----
void handleRoot() {
  server.send_P(200, "text/html", PAGE_HTML);
}

void handleStatus() {
  String json = "{\"status\":\"" + currentStatus + "\"}";
  server.send(200, "application/json", json);
}

void handleOpen() {
  myServo.write(OPEN_ANGLE);
  digitalWrite(blueLED, HIGH);
  digitalWrite(clearLED, LOW);
  currentStatus = "open";
  server.send(200, "application/json", "{\"success\":true,\"status\":\"open\"}");
}

void handleClose() {
  myServo.write(CLOSE_ANGLE);
  digitalWrite(clearLED, HIGH);
  digitalWrite(blueLED, LOW);
  currentStatus = "close";
  server.send(200, "application/json", "{\"success\":true,\"status\":\"close\"}");
}

void setup() {
  Serial.begin(115200);

  pinMode(clearLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  digitalWrite(clearLED, HIGH);   // start in "closed" state
  digitalWrite(blueLED, LOW);

  myServo.attach(servoPin);
  myServo.write(CLOSE_ANGLE);

  WiFi.softAP(ssid, password);
  Serial.print("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP());   // usually 192.168.4.1

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/open", handleOpen);
  server.on("/close", handleClose);

  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}