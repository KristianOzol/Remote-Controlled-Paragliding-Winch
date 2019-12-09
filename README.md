# Remote-Controlled-Paragliding-Winch

--Introduction--

Conceptually the project consists of two entities both controlled by Arduino boards. One is the "Handle", which is an Arduino Pro wired with a radio frequency transceiver, a barometer, an LCD display, four push button switches, and a throttle based on optical switches. This handle transmits information regarding the state of these components to the "Controller", which is an Arduino Mega controlling an electric winch through four relay switches according to the data it receives through its transceiver.

I was responsible for developing the software for the two microcontrollers, written in C/C++ using the Arduino IDE, while the hardware and electrical engineering aspects were primarily handled by the project manager.

--Hardware Components--

The Handle:
Arduino Pro Mini board
Adafruit SSD1306 128X32 OLED LCD display
Adafruit MPL3115A2 I2C Barometric Pressure/Altitude/Temperature Sensor
C50XB TTL Wireless Radio Frequency Transceiver
