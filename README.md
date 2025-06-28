# 🔥 ESP32 Wi-Fi MITM Captive Portal

An original Wi-Fi **Man-in-the-Middle (MITM)** tool built with an **ESP32**. This project creates a **fake access point** that mimics public Wi-Fi, tricks users into entering their real Wi-Fi password, and logs captured credentials — all without a laptop or external software.

> ⚠️ **For educational and authorized testing purposes only. Do not use on networks you do not own or have explicit permission to test.**

---

## 🚀 Features

- ✅ Creates a realistic fake Wi-Fi network
- ✅ Custom captive portal asking for Wi-Fi password
- ✅ Captures and logs passwords to ESP32 SPIFFS
- ✅ View logs from any browser (no serial monitor needed)
- ✅ Secret admin page: `http://192.168.4.1/admin12345`
- ✅ Auto-refreshing logs page with “Clear Logs” button
- ✅ Fully standalone — no PC required after flashing

---

## 📸 Screenshots

**Login Page:**

![Login Page](https://github.com/MuhamadJalalDev/MIM-attack-ESP32/blob/main/img/newlogin.png)

**Captured Logs Admin Page:**

![Admin Page](https://github.com/MuhamadJalalDev/MIM-attack-ESP32/blob/main/img/captured.png)

---

## 🛠️ Hardware Requirements

- [x] ESP32 Development Board
      ![image alt](https://github.com/MuhamadJalalDev/MIM-attack-ESP32/blob/main/img/esp.jpg)

- [x] Micro-USB cable


- [x] Arduino IDE or PlatformIO
- (Optional) Breadboard or enclosure for deployment & antena for longer range

---

## 📦 Installation

1. **Flash the code:**
   - Open the `.ino` or `.cpp` file in Arduino IDE
   - Install required libraries: `WiFi.h`, `WebServer.h`, `DNSServer.h`, `SPIFFS.h`
   - Choose `ESP32 Dev Module` and correct COM port
   - Click **Upload**

2. **Connect to the ESP32 Wi-Fi:**
   - Network name: `WiFi name`

3. **Visit the fake portal:**
   - Open a browser and go to any site (or `192.168.4.1`)
   - Enter a test password in the fake login form

4. **View captured credentials:**
   - Navigate to `http://192.168.4.1/admin12345`
   - Logs auto-refresh every 10 seconds

5. **Clear captured logs:**
   - Click **“Clear Logs 🗑️”** on the admin page

---

## 🧠 How It Works

- Creates a SoftAP (Access Point)
- DNS spoofing redirects all domains to captive portal
- Victim submits password via fake form
- ESP32 stores credentials in `creds.txt` on SPIFFS
- Admin panel allows live viewing and clearing of logs

---

## 👨‍💻 Author

**Muhamad Jalal**  
Security Researcher | 3rd Year IT Student  

---

## 📜 Disclaimer

This tool is intended **only for educational purposes** or authorized security assessments. The misuse of this tool could result in criminal charges. **You are responsible** for your actions.

---

## ⭐️ Support or Feature Requests?

Feel free to open an issue or contact me directly if you want:

muhamadjalal1300@gmail.com


