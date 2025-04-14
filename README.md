# Cold Chain Supply Management

A real-time IoT-enabled cold chain monitoring and security system designed for organ and critical medical asset transportation. This project uses **ESP32**, **Blynk Cloud**, and multiple sensors to monitor GPS location, environmental conditions, RFID access, motion/shock events, and send live updates via dashboard and SMS.

> *Real-time Blynk dashboard showing GPS, temperature, humidity, box status, RFID access, and buzzer state*

---

## Project Summary

This system aims to improve reliability and security in cold chain logistics for organ transport and sensitive medical materials. Using a combination of sensors and wireless communication modules, it ensures end-to-end visibility, traceability, and access control during transit.

---

## Features

- ✅ **GPS Location Tracking** with live coordinates and IST timestamp
- 🌡️ **Temperature & Humidity Monitoring** inside the transport box (DHT11)
- 🔓 **RFID Authentication** for secured box access (MFRC522)
- 📦 **Box Open/Close Detection** using LDR sensor
- 🔊 **Buzzer Alerts** for threshold breach or unauthorized access
- 🛰️ **GSM Module** for SMS alerts in low-connectivity zones
- ⚠️ **Motion/Impact Detection** using MPU6050 (shock/tip alert)
- 📊 **Blynk Dashboard Integration** for cloud-based real-time monitoring

---

## 🖥Dashboard 

The Blynk dashboard provides the following widgets:
- GPS Details: Latitude, Longitude, IST Time
- Temperature Gauge (°C)
- Humidity Gauge (%)
- GPS Speed
- Box Status (Open/Closed)
- Buzzer ON/OFF Status
- RFID Authentication Status
- Object Presence Confirmation

### 📸 **Dashboard View**  

![image](https://github.com/user-attachments/assets/cc9c8e1e-879e-4e9c-b2b8-5afb435cf0a9)
---

## Hardware Used

| Component           | Description                                  |
|---------------------|----------------------------------------------|
| ESP32 Dev Board     | Main microcontroller (Wi-Fi + Serial)        |
| DHT11 Sensor        | Temperature and Humidity sensing             |
| MFRC522 RFID Module | RFID reader for access control               |
| GPS Module (Neo-6M) | Location tracking                            |
| GSM Module (SIM800L)| SMS backup communication                     |
| MPU6050 Sensor      | Accelerometer + Gyroscope for impact sensing |
| LDR Sensor          | Detects box open/close based on light        |
| Ultrasonic Sensor   | Object presence confirmation inside box      |
| Buzzer              | Driver-side alert for emergencies            |

---

## 💡 How It Works

1. **Startup:** ESP32 initializes all connected modules and sensors.
2. **GPS Tracking:** GPS coordinates are continuously pushed to Blynk.
3. **Sensor Monitoring:** DHT11 tracks environment; MPU6050 detects impact.
4. **Access Control:** RFID scan validates identity. Unauthorized access triggers buzzer + alert.
5. **Cloud Updates:** All parameters are updated to Blynk virtual pins (V0 to V8).
6. **SMS Alerts:** GSM module sends fallback SMS in absence of Wi-Fi.

---

## Getting Started

### 1. Clone This Repository
```bash
git clone https://github.com/your-username/cold-chain-supply-management.git
```

### 2. Install Arduino Libraries

- `BlynkSimpleEsp32.h`
- `DHT.h`
- `TinyGPS++.h`
- `MFRC522.h`
- `SoftwareSerial.h`

> Use the Arduino Library Manager or install from `.zip` file if needed.

---

### 3. Flash ESP32

- Open the `.ino` file in **Arduino IDE**
- Enter your **Wi-Fi SSID**, **Password**, and **Blynk Auth Token**
- Select **ESP32 Dev Module** as the board
- Upload the code and monitor via **Serial Monitor** (baud rate: `115200`)

---

## Sample Results

| Parameter           | Observed Value       | Threshold/Expected | Alert Triggered |
|---------------------|----------------------|---------------------|-----------------|
| Temperature         | 30.1°C               | < 35°C              | No              |
| Humidity            | 84%                  | < 90%               | No              |
| GPS Speed           | 0 km/h               | -                   | No              |
| RFID Authentication | Authorized (D72C0CCA)| Valid UID           | No              |
| Box Status          | Open                 | -                   | No              |

---

## 📄 License

This project is licensed under the **MIT License** – see the [LICENSE](LICENSE) file for details.

---

## Contact

For collaboration, deployment, or academic inquiries, reach out via GitHub Issues or open a discussion on this repository.



