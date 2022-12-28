# IoT-maintenance-motor

## Description
This project takes the data of the sensors installed on an electrical motor measuring
current (A), Noise (db), temperature (C°) and vibration (hz).
All this data is recollected by the Arduino with the script `sensores.ino` and this 
send all the string data to the Raspberry Pi by serial communication so this one can 
send the data to the IBM Cloud storaging it there using Node-RED by the MQTT broker called 
shiftr.io.
