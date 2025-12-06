# Automatic Dice Shaker & Color-Detecting Dice Reader

This project is an Arduino-based automatic dice randomizer and optical dice reader. It shakes a set of dice using a vibration bowl, releases a single die using a servo-controlled gate, detects the die color with a TCS3200 sensor, and displays the result on an LCD.

The system was built for the CU Boulder ECEN/GEEN 1400 Freshman Projects course.

---

## ⚡ Features
- **Automated Dice Randomization**
  - 0.75-second vibration motor randomized dice reliably.
- **Servo-Controlled Release Gate**
  - Guarantees exactly one die passes through at a time.
- **Optical Color Detection**
  - TCS3200 sensor detects RGB values and classifies die color.
- **LCD Display Output**
  - Output shows “Detected Color: ____”.
- **Fail-Safe Mechanical Design**
  - Chute angle tested for optimal sliding  
  - Gate geometry prevents multiple dice entering the read zone

---

## 🛠 Hardware Used
- Arduino Uno (Rev 3)
- TCS3200 Color Sensor
- SG90 Micro Servo
- 16x2 LCD Display + I2C Backpack
- Vibration Motor / Shaker Bowl (custom-built)
- Custom acrylic or 3D-printed chute + enclosure

Full wiring diagrams are located in **/hardware**.

---

## 📦 Repository Structure
