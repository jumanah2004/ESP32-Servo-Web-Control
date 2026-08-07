# 🤖 ESP32 Servo Web Control

A simple ESP32-based project that lets you control a servo motor (open/close) from any phone or browser through a custom web interface — no app or internet connection required.

## 📸 Screenshots & Demo

**🖥️ Web Control Panel**

![Device Control UI](docs/website-screenshot.png)

**🔩 Circuit / Wiring Diagram**

![Circuit Diagram](https://github.com/jumanah2004/ESP32-Servo-Web-Control/blob/d757e142f59b399813f192fdf9202cca88808a0e/RUN_G.mp4)
![Circuit PHOTO ](https://github.com/jumanah2004/ESP32-Servo-Web-Control/blob/d757e142f59b399813f192fdf9202cca88808a0e/RUN_G.mp4)

**🎬 Demo Video**

[![Demo Video](docs/video-thumbnail.png)](docs/demo-video.mp4)

*(On GitHub, upload the video file directly into an Issue or the repo — GitHub will generate a playable link you can paste above. Local `.mp4` files don't play inline in a README.)*

## 📝 Overview

The ESP32 creates its own WiFi network (Access Point mode) and serves a styled control panel. From the panel you can:

- 🔓 **Open** the device → servo moves to the open position, blue LED turns on, clear/red LED turns off
- 🔒 **Close** the device → servo moves to the closed position, clear/red LED turns on, blue LED turns off
- 📶 See the **current status** (open/closed) fetched live from the ESP32

## 🔧 Hardware

| Component        | Notes                        |
|-------------------|-------------------------------|
| ESP32 dev board    | Any standard ESP32            |
| Servo motor        | Connected to GPIO 18          |
| LED (blue)          | Connected to GPIO 4 — lights on OPEN |
| LED (clear/red)     | Connected to GPIO 2 — lights on CLOSE |
| Resistors (2x 1kΩ)  | One in series with each LED   |
| Breadboard + jumper wires | For prototyping        |

## 🔌 Wiring

- Servo signal → **GPIO 18**
- Blue LED → **GPIO 4** (in series with a **1kΩ resistor**)
- Clear/Red LED → **GPIO 2** (in series with a **1kΩ resistor**)
- Servo and LED grounds → ESP32 **GND**
- Servo power → **5V** (external power recommended for larger servos)

## 📦 Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) with ESP32 board support installed
- **ESP32Servo** library (install via Library Manager: *Sketch → Include Library → Manage Libraries → search "ESP32Servo"*)

## ⚙️ Setup

1. Open the `.ino` sketch in the Arduino IDE.
2. Select your ESP32 board and correct COM port.
3. (Optional) Change the WiFi credentials in the sketch:
   ```cpp
   const char* ssid     = "Badr_Servo";
   const char* password = "badr12345";
   ```
4. Upload the sketch to the ESP32.
5. Open the Serial Monitor at `115200` baud — after boot you'll see the Access Point IP address (usually `192.168.4.1`).

## 🚀 Usage

1. On your phone or laptop, connect to the WiFi network **Badr_Servo** (password: `badr12345`).
2. Open a browser and go to:
   ```
   http://192.168.4.1
   ```
3. Use the **Open** and **Close** buttons on the control panel to move the servo and toggle the LEDs.
4. The status badge at the top updates automatically to reflect the device's current state.

## 🧠 How It Works

- The ESP32 runs a lightweight `WebServer` on port 80.
- The main page (`/`) serves an embedded HTML/CSS/JS control panel (stored in flash via `PROGMEM`).
- `/open` and `/close` endpoints move the servo, toggle the LEDs, and return a JSON response.
- `/status` returns the current state as JSON, which the page polls on load to sync the status badge.

## 🌐 Endpoints

| Route     | Method | Description                          |
|-----------|--------|---------------------------------------|
| `/`        | GET    | Serves the control panel web page     |
| `/status`  | GET    | Returns `{ "status": "open" \| "close" }` |
| `/open`    | GET    | Moves servo to open position           |
| `/close`   | GET    | Moves servo to close position          |

## 📄 License

Feel free to use and modify this project for personal or educational purposes.
