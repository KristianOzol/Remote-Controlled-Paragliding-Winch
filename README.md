# Remote-Controlled-Paragliding-Winch

Conceptually the project consists of two entities both controlled by Arduino boards. One is the "Handle", which is an Arduino Pro equipped with a radio frequency transceiver, a barometer, an LCD display, four push button switches, and a throttle based on optical switches. This handle transmits information regarding the state of these components to the "Controller", which is an Arduino Mega controlling an electric winch through four relay switches according to the data it receives through its transceiver.


