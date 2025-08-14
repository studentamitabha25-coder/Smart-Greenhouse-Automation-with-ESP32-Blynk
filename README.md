# Smart-Greenhouse-Automation-with-ESP32-Blynk
This project is an IoT-based Smart Greenhouse control system powered by an ESP32 microcontroller. It automatically monitors and manages environmental conditions such as temperature, humidity, and soil moisture, while allowing manual control through the Blynk mobile app.

It’s designed for home gardeners, researchers, and small-scale farmers who want an affordable, reliable, and remotely controllable plant-growing setup.

# Features

Real-time monitoring of temperature, humidity, and soil moisture.

Automatic irrigation when soil moisture drops below a set threshold.

Humidifier control based on adjustable humidity levels.

Customizable pump delay and duration via the Blynk app.

Manual pump activation from your smartphone.

Pump usage tracking (total run-time and number of activations).

Data persistence in EEPROM — pump stats survive power loss.

Blynk dashboard for remote control and live data visualization.

Non-blocking code for smooth, responsive operation.

Humidity scaling correction for inaccurate sensors.

# Hardware Required

1)  ESP32 development board

2) DHT11 / DHT22 temperature & humidity sensor (DHT22 recommended for accuracy)

3) Soil moisture sensor

4) Water pump + relay/MOSFET

5) Humidifier (small ultrasonic)
   1. 1 kiloohm resistor
   2. Diode
   3. TIP122

7) Power supply
   1. Solar panel 25W
   2. Solar charge controller
   3. Battery
   4. L7805 voltage regulator
   5. Capacitor(Electrolytic and ceramic)


# Blynk Virtual Pin Mapping
| Widget Name        | Pin | Type          | Range / Notes    |
| ------------------ | --- | ------------- | ---------------- |
| Temperature        | V0  | Gauge/Chart   | Double           |
| Humidity           | V1  | Gauge/Chart   | Double           |
| Soil Moisture      | V2  | Gauge/Chart   | 0–100            |
| Pump Status        | V3  | LED           | 0 = OFF, 1 = ON  |
| Humidifier Status  | V4  | LED           | 0 = OFF, 1 = ON  |
| Humidity Threshold | V5  | Slider        | 60–95            |
| Manual Pump        | V6  | Button (Push) | Sends 1 on press |
| Pump Duration      | V8  | Number Input  | 1–30 seconds     |
| Pump Delay Hours   | V10 | Number Input  | 0–23             |
| Pump Delay Minutes | V11 | Number Input  | 0–59             |
| Pump Delay Seconds | V12 | Number Input  | 0–59             |
| Pump Start Count   | V13 | Value Display | Integer          |
| Total ON Time      | V14 | Value Display | In seconds       |

# How It Works

1. ESP32 reads sensors every 2 seconds.

2. Soil moisture is mapped to 0–100% and compared to the threshold.

3. If the moisture is too low and the pump delay time has passed, the pump turns on for the set duration.

4. Humidity is compared to the set threshold, and the humidifier switches on/off automatically.

5. All values and controls are synced with the Blynk app.

6. Pump usage stats are saved in EEPROM so they survive power loss.


## 🔌 Circuit Design

You can view the Circuit design here:
[💡Circuit.png](Circuit.png)
(Sorry for not giving SVG. It was making an issue)

Follow this link to talk intractably about the circuit:

https://github.com/studentamitabha25-coder/Smart-Greenhouse-Automation-with-ESP32-Blynk/blob/0caf6de005c19b01a0e139be8a9cbe800c4cb6fb/Circuit.png

## 💻 Code
You can view the full source code here:  
[📜 SmartGreenhouse.ino](Smart_Greenhouse.ino)


# Example Blynk Dashboard
![Image Alt](https://github.com/studentamitabha25-coder/Smart-Greenhouse-Automation-with-ESP32-Blynk/blob/bd5129f273b2d1e766a67c116b4c0977a55bba4d/another%20screenchot.png)
![Image Alt](https://github.com/studentamitabha25-coder/Smart-Greenhouse-Automation-with-ESP32-Blynk/blob/bd5129f273b2d1e766a67c116b4c0977a55bba4d/Screenshot%20(117).png)

