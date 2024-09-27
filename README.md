# IoT-Garden-Monitoring-System

## Overview
This project implements an IoT-based garden monitoring system using ESP8266/ESP32 microcontrollers. The system collects data from various sensors to monitor environmental conditions crucial for plant growth, publishes this data to an MQTT broker, processes it through Node-RED, stores it in InfluxDB, and visualizes it using both Node-RED dashboards and a comprehensive Grafana dashboard.

## Features
- Temperature and humidity monitoring (DHT22 sensor)
- Soil moisture sensing
- Light intensity measurement (LDR sensor)
- pH level sensing
- NPK (Nitrogen, Phosphorus, Potassium) soil nutrient monitoring
- Real-time data processing with Node-RED
- Data storage in InfluxDB
- Data visualization with Node-RED dashboard and Grafana

## Hardware Requirements
- ESP8266 or ESP32 board
- DHT22 temperature and humidity sensor
- Soil moisture sensor
- LDR (Light Dependent Resistor)
- pH sensor
- NPK sensor with RS485 interface
- Jumper wires
- Breadboard (optional, for prototyping)

## Software Requirements
- Arduino IDE
- MQTT Broker (e.g., Mosquitto)
- Node-RED
- InfluxDB
- Grafana

### Required libraries:
- ESP8266WiFi (for ESP8266) or WiFi (for ESP32)
- SoftwareSerial
- Wire
- PubSubClient
- DHT sensor library



## Setup Instructions

### ESP8266/ESP32 Setup
1. Install the Arduino IDE and required libraries.
2. Open the provided code in the Arduino IDE.
3. Set your Wi-Fi credentials:
```
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
```
4. Set your MQTT broker IP address:
```
#define MQTT_BROKER_IP "192.168.100.44"
```
5. Upload the code to your ESP8266/ESP32 board.

### MQTT Broker Setup
1. Install and configure your MQTT broker (e.g., Mosquitto) on the IP address specified in the ESP8266/ESP32 code.

### Node-RED Setup
1. Install Node-RED on your server or local machine.
2. Import the provided Node-RED flow:
- In Node-RED, go to the menu and select "Import" > "Clipboard"
- Paste the contents of the provided JSON file
- Click "Import" to create the flow
3. The imported flow includes:
- A UI chart node for Temperature visualization
- A UI group named "temp"
- A UI tab named "test1"
4. Configure MQTT input nodes to subscribe to the topics published by your ESP8266/ESP32.
5. Set up InfluxDB output nodes to write data to your InfluxDB database.
6. Deploy your flow in Node-RED.
7. Access the Node-RED dashboard at http://your-node-red-server:1880/ui to view the Temperature chart.

### InfluxDB Setup
1. Install InfluxDB on your server or local machine.
2. Create a new database for your garden monitoring data.

### Grafana Setup
1. Install Grafana on your server or local machine.
2. Add InfluxDB as a data source in Grafana.
3. Import the provided Grafana dashboard JSON file:
- In Grafana, go to "Create" > "Import".
- Paste the contents of the ```Grafana.json``` file.
- Select the appropriate InfluxDB data source.
- Click "Import" to create the dashboard.

## MQTT Topics
The system publishes sensor data to the following MQTT topics:
- Temperature (Celsius): sensor/DHT22/temperature_celsius
- Temperature (Fahrenheit): sensor/DHT22/temperature_fahrenheit
- Humidity: sensor/DHT22/humidity
- Soil Moisture: sensor/soil_moisture
- Light Intensity: sensor/ldr
- pH Level: sensor/ph
- Nitrogen: sensor/NPK/nitrogen
- Phosphorous: sensor/NPK/phosphorous
- Potassium: sensor/NPK/potassium

## Node-RED Dashboard
The Node-RED dashboard includes:
- A line chart showing real-time temperature data
- Data is displayed for the last hour
- The chart updates in real-time as new data is received

## Grafana Dashboard
The Grafana dashboard, named "Griselda_grafana", includes the following panels:
1. Potassium Level (Bar Chart)
2. Phosphorous Level (Time Series)
3. Nitrogen Level (Time Series)
4. Humidity (Time Series)
5. Temperature (Time Series, in Fahrenheit)
6. Soil Moisture (Gauge)
7. pH Sensor (Time Series)
8. Light Intensity (Time Series)

The dashboard is set to display data from the last 6 hours by default.

## Data Flow
1. Sensors collect environmental data
2. ESP8266/ESP32 reads sensor data and publishes to MQTT topics
3. Node-RED subscribes to MQTT topics and processes the data
4. Node-RED sends data to InfluxDB for storage
5. Node-RED updates its own UI with real-time temperature data
6. Grafana queries data from InfluxDB and updates its comprehensive dashboard

## Usage
After setting up all components:
1. The ESP8266/ESP32 will automatically connect to the specified Wi-Fi network and MQTT broker.
2. Sensor readings will be taken and published to the respective MQTT topics.
3. Node-RED will process the data, store it in InfluxDB, and update its temperature chart.
4. View real-time temperature data in the Node-RED UI at ```http://your-node-red-server:1880/ui```.
5. Access the Grafana dashboard for a comprehensive view of all sensor data over time.

## Troubleshooting

- Check the serial monitor in Arduino IDE for ESP8266/ESP32 debugging information.
- Verify MQTT broker is running and accessible from the ESP8266/ESP32.
- In Node-RED, use debug nodes to ensure data is flowing correctly.
- In Grafana, use the query inspector to verify data retrieval from InfluxDB.

## Contributing
Contributions to improve the project are welcome. Please feel free to submit pull requests or open issues to discuss proposed changes or report bugs.

## Acknowledgments
- List any libraries, resources, or individuals you'd like to credit.
