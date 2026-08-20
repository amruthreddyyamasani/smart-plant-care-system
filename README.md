# Smart Plant Care Alert System (ESP32)

**Wokwi Simulation Link:** [Insert Your Shared Wokwi URL Here]

---

## Project Overview
An automated ESP32-based IoT monitoring system simulated in Wokwi. It reads environmental metrics—temperature, humidity, light intensity, and soil moisture—evaluates overall plant health through a custom decision engine, and streams real-time JSON status payloads to an MQTT broker every 5 seconds.

---

## Pin Connections & Hardware Layout

| Component | ESP32 Pin | Function | Input Type |
| :--- | :--- | :--- | :--- |
| **DHT22** | GPIO 15 | Ambient Temperature (°C) & Humidity (%) | Digital |
| **LDR (Photoresistor)** | GPIO 34 | Light Intensity ($0–100\%$) | Analog ADC ($0–4095$) |
| **Potentiometer** | GPIO 35 | Soil Moisture Level ($0–100\%$) | Analog ADC ($0–4095$) |
| **Power & Ground** | 3V3 / GND | Common Power Rail | Power Supply |

---

## Threshold Logic & Risk Conditions

The system continuously processes single-sensor baselines alongside prioritized multi-sensor risk conditions:

* **Base Checks:**
  * **Needs Water:** Soil Moisture $<30\%$
  * **Needs Light:** Light Intensity $<25\%$
  * **Too Hot:** Temperature $>35^\circ\text{C}$
* **Custom Multi-Sensor Conditions (Priority):**
  1. **Fungal & Rot Risk:** Soil Moisture $>80\%$ AND Humidity $>85\%$
  2. **Frost & Freeze Risk:** Temperature $<10^\circ\text{C}$ AND Humidity $>80\%$
  3. **Sunburn & Scorch Danger:** Light Intensity $>80\%$ AND Temperature $>35^\circ\text{C}$

---

## MQTT Payload Example

Messages are published to `plantCare/status/data` every 5 seconds:

```json
{
  "temp": 24.5,
  "humidity": 88.0,
  "light": 55,
  "soil": 82,
  "status": "Fungal & Rot Risk"
}
