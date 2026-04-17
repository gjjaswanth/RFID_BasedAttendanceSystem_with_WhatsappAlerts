# 🚀 RFID-Based Smart Attendance System with Real-Time WhatsApp Alerts

An IoT-based smart attendance system that automates classroom attendance using RFID technology and provides real-time WhatsApp alerts for late arrivals, absentees, and proxy detection.

---

## 📌 Project Overview

This project replaces traditional attendance systems with a contactless, automated, and intelligent solution.

* Each student is assigned a unique RFID card
* Attendance is recorded using NodeMCU (ESP8266) + RC522
* Data is stored in Google Sheets (Cloud)
* WhatsApp alerts are sent only for important events (Late, Absent, Proxy)

---

## ⚡ Features

* Contactless RFID-based attendance
* Session-based attendance control (Teacher controlled)
* Late detection using time thresholds
* Dual-round verification for proxy detection
* Real-time cloud logging (Google Sheets)
* Selective WhatsApp alerts (No notification spam)
* Secure HTTPS communication

---

## 🏗️ System Architecture

The system is divided into four layers:

1. **Perception Layer**

   * RFID Tags + RC522 Reader

2. **Processing Layer**

   * NodeMCU (ESP8266) handles logic

3. **Network Layer**

   * WiFi communication (HTTP/HTTPS)

4. **Application Layer**

   * Google Sheets (Database)
   * Twilio API (WhatsApp Alerts)

---

## 🔌 Hardware Components

* NodeMCU (ESP8266)
* RC522 RFID Module
* RFID Cards/Tags
* Buzzer
* Breadboard
* Jumper Wires

---

## 🧠 Software & Technologies

* Embedded C/C++ (Arduino IDE)
* Google Apps Script
* Google Sheets API
* Twilio API (WhatsApp)
* SPI Protocol

---

## 🔗 Circuit Connections

| Component      | Pin      | ESP8266 Pin |
| -------------- | -------- | ----------- |
| RC522 SDA (SS) | SS       | D2          |
| RC522 SCK      | Clock    | D5          |
| RC522 MOSI     | Data Out | D7          |
| RC522 MISO     | Data In  | D6          |
| RC522 RST      | Reset    | D1          |
| RC522 VCC      | Power    | 3.3V        |
| RC522 GND      | Ground   | GND         |
| Buzzer (+)     | Signal   | D0          |
| Buzzer (-)     | Ground   | GND         |

---

## ⚙️ Working Principle

### 1. Session Start

Teacher scans RFID card to start attendance.

### 2. Round 1 (Attendance)

Students scan cards.
System records time and marks:

* On-time
* Late

### 3. Round 2 (Verification)

Students scan again.
Used to detect proxy attendance.

### 4. Final Evaluation

* On-Time → No alert
* Late → WhatsApp alert
* Absent → WhatsApp alert
* Proxy → WhatsApp alert

---

## 🧮 Decision Logic

* On-Time: t ≤ Tlimit
* Late: t > Tlimit
* Proxy: Present in Round 1 but absent in Round 2
* Absent: Not present in both rounds

---

## 📡 Data Flow

RFID Card → RC522 → NodeMCU → WiFi → Google Apps Script → Google Sheets
↓
Twilio API
↓
WhatsApp Alert

---

## 📊 Performance

* Cloud sync latency: ~1.85 seconds
* WhatsApp alert delay: 2.4 – 4.1 seconds
* Proxy detection accuracy: 100%

---

## 🧪 Test Cases

| Scenario | Output            |
| -------- | ----------------- |
| On-Time  | Logged as Present |
| Late     | WhatsApp Alert    |
| Proxy    | WhatsApp Alert    |
| Absent   | WhatsApp Alert    |

---

## 🔐 Security Features

* HTTPS communication (SSL/TLS)
* Twilio authentication (Account SID & Token)
* Duplicate entry prevention using Apps Script

---

## 🚀 Setup Instructions

1. Connect hardware as per circuit diagram
2. Upload code to NodeMCU using Arduino IDE
3. Configure:

   * WiFi credentials
   * Google Apps Script URL
   * Twilio API credentials
4. Scan teacher card to start session
5. Scan student cards

---

## 📈 Advantages

* Eliminates manual attendance errors
* Prevents proxy attendance
* Real-time parent notifications
* Reduces unnecessary alerts
* Scalable and low-cost

---

## 🔮 Future Improvements

* Mobile app integration
* Face recognition system
* Firebase database integration
* Analytics dashboard

---

## 👨‍💻 Team

* Gunja Jaswanth
* Ravuri Leela Venkata Sai Krishna
* Chandhanika Kokkula
* Khushboo Mehta

---

## 🙏 Acknowledgment

We thank our faculty for their guidance and support throughout this project.

---

## 📜 License

This project is for academic and educational purposes.
