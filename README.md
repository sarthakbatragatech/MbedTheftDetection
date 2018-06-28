# Mbed Theft Detection
Simple anti-theft system implementation on Arm Mbed OS using microcontrollers designed to benefit both beginners and more experienced programmers that have an interest in the basics of home security. 

# Design and Work
This design uses an accelerometer to measure displacement for protection against forced entry. A sensor to measure fluctuations in temperature in order to simulate a fire alarm and an ultrasonic sensor to monitor movement. If any of these sensors are triggered an alarm is activated and a camera turns on and takes pictures of the intruder. Bluetooth technology allows your phone to activate/deactivate the system.

# Parts used
Mbed x2
MMA8452Q Accelerometer
TMP36 Temperature Sensor
HC-SR04 Ultrasonic Sensor
Adafruit Bluefruit LE UART Friend Bluetooth
LS-Y201 Camera
4DGL ULCD
Speaker
2N3904 Transistor
220-1k Ohm Resistor
Push button x2
