# Environmental Monitoring Device
This project is a device that measures CO2 concentration, temperature and relative humididty using Sensirion's SCD4x sensor and a Raspberry Pi Zero W. The data is then stored internally and, optionally, transmitted via MQTT.

## Hardware Requirements
- Raspberry Pi Zero W
- Sensirion SCD41 or SCD40 sensor

## Wiring
| SCD4x Pin | Raspberry Pi Pin |
|-----------|------------------|
| VCC       | 3.3V             |
| GND       | GND              |
| SDA       | GPIO2 (Pin 3)    |
| SCL       | GPIO3 (Pin 5)    |

## Installation

### 1. Clone the repository

```bash
git clone https://github.com/msbavaresco@gmail.com/Environmental-Monitoring-Device.git
cd Environmental-Monitoring-Device
```

### 2. Install required packages

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    gcc \
    g++ \
    make \
    libpaho-mqttpp3-dev \
    nlohmann-json3-dev
```

### 3. Enable I2C on the Raspberry Pi

Run:

```bash
sudo raspi-config
```

Then go to:

```text
Interface Options -> I2C -> Enable
```

Reboot:

```bash
sudo reboot
```

After reboot, return to the project folder:

```bash
cd Environmental-Monitoring-Device
```

### 4. Create the configuration directory

```bash
sudo mkdir -p /var/lib/ThisDevice
```

### 5. Copy the example configuration file

```bash
sudo cp config/config.example.json /var/lib/ThisDevice/config.json
```

### 6. Edit the configuration file

```bash
sudo nano /var/lib/ThisDevice/config.json
```

Example:

```json
{
  "wifi": {
    "configured": true,
    "ssid": "MyWifi"
  },
  "mqtt": {
    "host": "192.168.1.50",
    "port": 1883,
    "username": "",
    "password": "",
    "base_topic": "env"
  },
  "device": {
    "location": "office"
  }
}
```

### 7. Build the project

```bash
make
```

### 8. Run the program

```bash
./bin/project
```

## Notes

- The configuration file must exist at `/var/lib/ThisDevice/config.json`
- I²C must be enabled or the sensor will not work
- Update the MQTT settings and device location before running


