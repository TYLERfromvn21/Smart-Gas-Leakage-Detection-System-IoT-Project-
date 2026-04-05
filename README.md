# 🔥 Smart Gas Leakage Detection System (Industrial IoT)

![Status](https://img.shields.io/badge/Status-Completed-28A745?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0.0-blue?style=for-the-badge)

A robust, real-time safety monitoring solution using **NodeMCU-32S (ESP32)** and **FreeRTOS**. This system bridges physical environment sensing with instant cloud-based Telegram alerts and bidirectional remote control.

---

## 📋 Table of Contents
- [About the Project](#-about-the-project)
- [Key Features](#-key-features)
- [Tech Stack](#-tech-stack)
- [System Architecture](#-system-architecture)
- [Local Development Setup](#-local-development-setup)
- [Telegram Bot Commands](#-telegram-bot-commands)

---

## 🚀 About the Project

This project addresses the critical need for real-time hazardous gas detection (LPG, smoke, carbon monoxide). Moving beyond simple standalone Arduino sketches, this firmware utilizes a **FreeRTOS multi-threading architecture** to ensure the ESP32 can seamlessly monitor sensors, trigger local hardware alarms, and communicate over secure HTTPS networks simultaneously without blocking.

> **✅ Current Status:** The project is fully completed. The firmware has been stress-tested for memory leaks (Stack Overflow) and race conditions in a multi-threaded environment.

## ✨ Key Features

* **⚙️ Multi-threading Architecture:** Leverages dual-core ESP32 with FreeRTOS. Tasks are pinned to specific cores to isolate network processing from time-critical sensor polling.
* **🧠 Baseline Subtraction Algorithm:** A self-calibrating logic that dynamically calculates the "Clean Air" baseline upon startup, eliminating false positives caused by environmental noise.
* **🔒 Secure Bidirectional Communication:** Integrates Telegram Bot API via HTTPS. Users can remotely monitor status and configure system thresholds securely.
* **🛡️ Resource Protection:** Implements **Mutex** (Semaphore) to protect the Telegram API resource, preventing system crashes when multiple tasks attempt to access the network module.
* **🚨 Redundant Alerting Logic:** - *Tier 1 (Local):* Zero-latency GPIO triggers for Buzzer and LED.
  - *Tier 2 (Remote):* Telegram push notifications with an anti-spam timer mechanism.

---

## 🛠️ Tech Stack

### Firmware & Languages
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Embedded C](https://img.shields.io/badge/Embedded_C-A8B9CC?style=for-the-badge&logo=c&logoColor=black)

### Hardware & OS
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-221E68?style=for-the-badge&logo=freertos&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-F6822B?style=for-the-badge&logo=PlatformIO&logoColor=white)

### Communication & Cloud
![Telegram API](https://img.shields.io/badge/Telegram_API-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)
![HTTPS](https://img.shields.io/badge/HTTPS-000000?style=for-the-badge&logo=https&logoColor=white)

---

## 🏗️ System Architecture

1. **Sensing Layer (Core 1):** TaskReadSensor continuously polls the MQ2 Analog pin every 100ms, calculates the differential deviation from the baseline, and maps it to a percentage.
2. **Notification Layer (Core 1):** TaskAlertManager evaluates the gas percentage against the configured threshold. It manages local GPIO states (Buzzer/LED) and triggers Cloud alerts via Mutex.
3. **Cloud & Control Layer (Core 1):** TaskTelegram polls the Telegram server every 1000ms for incoming commands (e.g., /set, /calibrate) to dynamically adjust system parameters.
4. **Network Driver (Core 0):** Dedicated exclusively to maintaining the Wi-Fi stack and HTTPS encryption to prevent watchdog timeouts.

---

## 💻 Local Development Setup

To deploy this firmware to your own ESP32 board, follow these steps using Visual Studio Code and PlatformIO.

### Prerequisites
* Visual Studio Code with the **PlatformIO IDE** extension installed.
* An ESP32 development board (NodeMCU-32S recommended).
* A Telegram Bot Token (obtained from BotFather) and your Chat ID.

### 1. Clone the repository
```bash
git clone https://github.com/TYLERfromvn21/Smart-Gas-Leakage-Detection-System-IoT-Project-.git
cd Smart-Gas-Leakage-Detection-System-IoT-Project-
```
### 2. Configure Credentials
Navigate to include/config.h and update the environment definitions with your actual credentials:

// --- WI-FI CONFIGURATION ---
```bash
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
```
// --- TELEGRAM CONFIGURATION ---
```bash
#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_PERSONAL_CHAT_ID"
```
### 3. Build and Upload
1. Connect the ESP32 board via a USB data cable.
2. Click the Build (✓) icon in the PlatformIO toolbar to compile the C++ source code.
3. Click the Upload (→) icon to flash the firmware onto the MCU. (Hold the BOOT button on the ESP32 if it gets stuck at "Connecting...")
4. Open the Serial Monitor to verify the Wi-Fi connection and the initial calibration process.

---

## 🤖 Telegram Bot Commands

Once the system is online, interact with your Bot using the following commands:
* /status - Check current gas level, safety threshold, and baseline value.
* /calibrate - Trigger manual sensor recalibration (takes ~10 seconds).
* /set [value] - Set a custom safety threshold from 1 to 100 (e.g., /set 80).
* /up - Increase threshold by 5%.
* /down - Decrease threshold by 5%.
* /help - Show the interactive command menu.
