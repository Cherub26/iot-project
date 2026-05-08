# **IoT Reflex-Speed Training & Performance Analytics System** 

> **"The Battle Box"** — A professional-grade, millisecond-accurate diagnostic tool for athletes and physical therapy use-cases. 

<img width="2016" height="1134" alt="image" src="https://github.com/user-attachments/assets/b34fe514-e616-4334-bd87-d7e32060c266" />


## **1. Project Overview**
Athletic performance and neurological health often rely on objective reaction-speed metrics, yet professional timing equipment is frequently inaccessible to the average user. This project targets sports-tech and physical therapy by creating a smart "Battle Box" training pad that provides millisecond-accurate reflex data. 

Locally, the athlete receives haptic "Go!" signals and immediate feedback on an OLED screen. Simultaneously, performance data is synchronized to the **Arduino Cloud**, allowing trainers to monitor real-time reaction latency, historical "Best Times," and average performance trends over multiple sessions. This transforms a simple interaction into a professional-grade diagnostic tool.

---

## **2. Key Features**
* **High-Precision Timing:** Utilizes hardware-level timing to measure response latency with sub-millisecond precision.
* **Dual-Sensor Coordination:** An IR sensor verifies the user's starting position, while a vibration sensor detects the exact moment of physical impact.
* **Intelligent Logic States:** Features a three-stage haptic/visual start sequence: **Waiting**, **Ready** (Blue), **Get Set** (Yellow/Countdown), and **Go!** (Green/Vibration).
* **False Start Detection:** The system flags "False Starts" when impact occurs before the signal is delivered.
* **Cloud Analytics:** The dashboard calculates "Average Reaction Time" across sessions and displays performance trends.
* **Remote Trainer Control:** A trainer can remotely trigger new training rounds or reset session data from the dashboard.

---

## **3. Hardware Specifications**
| Component | Model | Task |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 | Central coordination and Cloud Wi-Fi gateway. |
| **Sensor 1** | SW-420 Vibration Sensor | Detects physical impact to mark the end of the reflex timing cycle. |
| **Sensor 2** | TCRT5000 IR Reflective | Ensures hand presence/ready position before a round begins. |
| **Actuator** | 5V Vibration Motor | Provides a haptic "Go!" signal and "False Start" alerts. |
| **Display** | 0.96 inch OLED | Renders real-time status and local performance analytics. |
| **Feedback** | Common Cathode RGB LED | Visual signaling for distinct game states (Ready, Set, Go). |

<img width="2016" height="1134" alt="image" src="https://github.com/user-attachments/assets/72a4d2e1-a498-4174-a521-17b640cf1d2f" />


---

## **4. Wiring Specifications**
To replicate the "Battle Box," connect the components to the ESP32 as follows:

| Component | Component Pin | ESP32 Pin | Logic Type |
| :--- | :--- | :--- | :--- |
| **SW-420 Sensor** | Digital Out | **GPIO 12** | Hardware Interrupt |
| **TCRT5000 IR** | Digital Out | **GPIO 34** | Digital Input |
| **Vibration Motor** | Positive (+) | **GPIO 18** | Digital Output |
| **RGB LED** | Red Anode | **GPIO 2** | PWM / Analog |
| **RGB LED** | Green Anode | **GPIO 4** | PWM / Analog |
| **RGB LED** | Blue Anode | **GPIO 5** | PWM / Analog |
| **OLED Display** | SDA | **GPIO 21** | I2C Data |
| **OLED Display** | SCL | **GPIO 22** | I2C Clock |

<img width="1739" height="1424" alt="image" src="https://github.com/user-attachments/assets/bdd6b433-8440-4944-b9a0-aea4662428d6" />

---

## **5. System Architecture** 
The system architecture follows a real-time event-driven model. The ESP32 acts as the central processing unit, continuously polling the TCRT5000 IR sensor to verify the user is in the "Ready" position. Once verified, the controller triggers a random delay before activating the vibration motor as a haptic "Go!" signal.

The system utilizes a high-priority hardware interrupt to monitor the SW-420 vibration sensor, capturing the exact microsecond of physical impact to calculate reaction latency. This performance data is rendered locally on the OLED display via I2C and synchronized via Wi-Fi to the Arduino IoT Cloud. The cloud dashboard enables remote monitoring and session management.

---

## **6. Dashboard & Analytics**
The trainer’s web dashboard provides a comprehensive suite of monitoring and control widgets:

* **Reflex Time Trend:** A line chart showing performance trends over multiple sessions.
* **Session Metrics:** Value widgets for **Best Time**, **Average Time**, and **Round Count**.
* **Live Status Monitoring:** Real-time feedback of the athlete's current state (e.g., WAITING, [READY], HIT!).
* **Trainer Controls:** Manual "Start Round" button and a "Reset Session" button to clear current session data.

<img width="1920" height="953" alt="Screenshot_20260508_153721" src="https://github.com/user-attachments/assets/9c83a49d-7194-4630-8b28-0d6c6cf379c1" />


---

## **7. How to Use**
1.  **Power On:** Connect the ESP32 to a power source. The OLED will display WiFi and Cloud connection status.
2.  **Ready Position:** The athlete places their hand near the IR sensor. The LED turns **Blue**, and status shows `[READY]`.
3.  **The Start:** After a brief hold, the LED turns **Yellow** (`GET SET...`) to signify the countdown. 
4.  **The Hit:** When the motor pulses and the LED turns **Green**, hit the box as fast as possible!.
5.  **Analytics:** View reaction time in milliseconds on the OLED and watch your "Best Time" and "Average" update on the Cloud Dashboard.

---
