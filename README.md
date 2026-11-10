# ⚙️ AC Motor Condition Monitoring (IoT)

> Collects current, temperature, vibration and noise from an AC motor and sends them to the cloud, as the basis for predictive maintenance.

![Arduino](https://img.shields.io/badge/Arduino-00878F?logo=arduino&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/Raspberry%20Pi-A22846?logo=raspberrypi&logoColor=white)
![Node-RED](https://img.shields.io/badge/Node--RED-8F0000?logo=nodered&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-660066?logo=mqtt&logoColor=white)
![IBM Cloud](https://img.shields.io/badge/IBM%20Cloud-1261FE?logo=ibmcloud&logoColor=white)

## Why

Most motor failures show early signs: higher current, overheating, more vibration or noise. Logging these variables continuously makes it possible to detect abnormal behaviour and plan maintenance **before** the motor fails.

## Measured variables

| Variable | Sensor / method | Unit |
|---|---|---|
| Current | Current transformer (30 A / 1 V) on an ADS1115 differential input, RMS over 1 s | A |
| Temperature | MLX90614 infrared sensor | °C |
| Vibration | Vibration sensor on the ADS1115, pulses counted over 1 s | Hz |
| Noise | Microphone on the ADS1115, peak-to-peak converted to dB | dB |

## Architecture

```
Sensors ─► ADS1115 / MLX90614 ─► Arduino ──serial──► Raspberry Pi
                                                        │
                                       MQTT (shiftr.io) ▼
                                 Node-RED ──► IBM Cloud (storage)
```

Two acquisition options are included:

- **`sensores/sensores.ino`** – Arduino reads the sensors and, with a **Timer1 interrupt every 1 s**, sends `current - temperature - vibration - noise` to the Raspberry Pi over serial.
- **`main.py`** – the Raspberry Pi reads the ADS1115 and MLX90614 directly over I²C and publishes each variable to its own MQTT topic (`Corriente`, `Temperatura`, `Vibracion`, `Ruido`) with a timestamp.

## Dataset

`Data_project.csv` contains **~600 samples** recorded from the motor:

```
Corriente (A);Ruido (db);Temperatura (C°);Vibracion (Hz)
4.92;66.66;21.39;52
```

## Run

```bash
pip install paho-mqtt smbus2 PyMLX90614 Adafruit_ADS1x15
python main.py
```

Set your own MQTT broker and credentials in `main.py` before running.

## Next steps toward predictive maintenance

- [ ] Label normal vs. faulty operation (overload, imbalance, bearing wear).
- [ ] Use vibration spectrum (FFT) instead of pulse counting.
- [ ] Train an anomaly-detection model on the dataset and raise alerts in Node-RED.
- [ ] Port the acquisition to an ESP32 for a single Wi-Fi device.
