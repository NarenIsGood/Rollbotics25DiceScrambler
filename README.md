Automatic Dice Shaker and Color Detecting Dice Reader

This project is an Arduino based system that shakes dice using a vibration bowl, releases one die using a servo gate, reads its color with a TCS3200 sensor, and displays the result on an LCD. The system was built for the CU Boulder Freshman Projects course.

Features:

Automated dice shaking

Servo controlled single die release

TCS3200 color detection

LCD output

Basic mechanical and electrical integration

Hardware:
Arduino Uno, TCS3200 sensor, SG90 servo, 16x2 LCD with I2C, vibration motor bowl, custom chute and gate assembly. Wiring diagrams are located in the hardware folder.

Firmware:
The firmware shakes the dice for a set duration, opens the gate, reads RGB values from the sensor, classifies the color, and prints the result on the LCD. The code is in the firmware folder.

Repository Structure:
firmware – Arduino code
hardware – wiring diagrams and schematics
mechanical – CAD files and mechanical components
testing – data and graphs
report – final PDF and LaTeX
demo – videos and photos

Testing:
Mechanical and electrical testing showed reliable dice shaking, single die release, and stable sensor readings. Color detection accuracy was approximately 90 percent.

Team:
Naren Tarapareddy
Micheal Vitas
Pol Jimenez
Nathan Vannatter
