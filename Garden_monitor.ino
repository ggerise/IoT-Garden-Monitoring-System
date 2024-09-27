#ifdef ESP8266
  #include <ESP8266WiFi.h>     // WiFi library for ESP8266 boards
#else
  #include <WiFi.h>            // WiFi library for ESP32 boards
#endif
#include <SoftwareSerial.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "DHT.h"             // DHT sensor library

#define LDR_SENSOR_PIN 15
#define SOIL_MOISTURE_SENSOR_PIN 2 
#define DHT_SENSOR_PIN 4
#define DHT_SENSOR_TYPE DHT22

#define WIFI_SSID "enter your ssid"
#define WIFI_PASSWORD "enter the password"
#define MQTT_BROKER_IP "   "

#define HUMIDITY_TOPIC "sensor/DHT22/humidity"
#define TEMP_C_TOPIC "sensor/DHT22/temperature_celsius"
#define TEMP_F_TOPIC "sensor/DHT22/temperature_fahrenheit"
#define SOIL_MOISTURE_TOPIC "sensor/soil_moisture"
#define LDR_TOPIC "sensor/ldr"
#define PH_TOPIC "sensor/ph"
#define NPK_N_TOPIC "sensor/NPK/nitrogen"
#define NPK_P_TOPIC "sensor/NPK/phosphorous"
#define NPK_K_TOPIC "sensor/NPK/potassium"

// pH sensor variables
const int PH_PIN = A0;
float phValue;
float phVoltage = 0;

// NPK sensor and RS485 pins
SoftwareSerial npkSensorSerial(27, 26);  // RX, TX
int RS485_DE_PIN = 32;
int RS485_RE_PIN = 33;

// DHT sensor instance
DHT dhtSensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting garden monitoring system...");

  initializeSensors();
  connectToWiFi();
  mqttClient.setServer(MQTT_BROKER_IP, 1883);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectToMQTTBroker();
  }
  mqttClient.loop();

  delay(5000);

  readSensorsAndPublishData();
}

void initializeSensors() {
  dhtSensor.begin();
  pinMode(PH_PIN, INPUT);
  delay(1000);

  // NPK sensor setup
  npkSensorSerial.begin(4800);
  pinMode(RS485_DE_PIN, OUTPUT);
  pinMode(RS485_RE_PIN, OUTPUT);
  digitalWrite(RS485_DE_PIN, LOW);
  digitalWrite(RS485_RE_PIN, LOW);
}

void connectToWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectToMQTTBroker() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");
    
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

void readSensorsAndPublishData() {
  // DHT sensor readings
  float humidity = dhtSensor.readHumidity();
  float temperatureC = dhtSensor.readTemperature();
  float temperatureF = dhtSensor.readTemperature(true);

  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float heatIndexC = dhtSensor.computeHeatIndex(temperatureC, humidity, false);

  Serial.print("\nHumidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperatureC);
  Serial.print("°C  Heat Index: ");
  Serial.print(heatIndexC);
  Serial.println("°C");

  mqttClient.publish(TEMP_C_TOPIC, String(temperatureC).c_str(), true);
  mqttClient.publish(TEMP_F_TOPIC, String(temperatureF).c_str(), true);
  mqttClient.publish(HUMIDITY_TOPIC, String(humidity).c_str(), true);

  // LDR sensor reading
  int ldrValue = analogRead(LDR_SENSOR_PIN);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);  
  mqttClient.publish(LDR_TOPIC, String(ldrValue).c_str(), true);

  // Soil moisture sensor reading
  int soilMoistureValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);
  mqttClient.publish(SOIL_MOISTURE_TOPIC, String(soilMoistureValue).c_str(), true);

  // pH sensor reading
  phValue = analogRead(PH_PIN);
  Serial.print("pH Value: ");
  phVoltage = phValue * (3.3 / 4095.0);
  float pH = 3.3 * phVoltage;
  Serial.println(pH);
  mqttClient.publish(PH_TOPIC, String(pH).c_str(), true);

  // NPK sensor readings
  byte npkQuery[]{ 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
  byte npkResponseData[19];
  digitalWrite(RS485_DE_PIN, HIGH);
  digitalWrite(RS485_RE_PIN, HIGH);

  npkSensorSerial.write(npkQuery, sizeof(npkQuery));

  digitalWrite(RS485_DE_PIN, LOW);
  digitalWrite(RS485_RE_PIN, LOW);

  if (npkSensorSerial.available() >= sizeof(npkResponseData)) {
    npkSensorSerial.readBytes(npkResponseData, sizeof(npkResponseData));

    unsigned int soilHumidity = (npkResponseData[3] << 8) | npkResponseData[4];
    unsigned int soilTemperature = (npkResponseData[5] << 8) | npkResponseData[6];
    unsigned int soilConductivity = (npkResponseData[7] << 8) | npkResponseData[8];
    unsigned int soilPH = (npkResponseData[9] << 8) | npkResponseData[10];
    unsigned int nitrogenValue = (npkResponseData[11] << 8) | npkResponseData[12];
    unsigned int phosphorusValue = (npkResponseData[13] << 8) | npkResponseData[14];
    unsigned int potassiumValue = (npkResponseData[15] << 8) | npkResponseData[16];

    Serial.print("Soil Humidity: ");
    Serial.println((float)soilHumidity / 10.0);
    Serial.print("Soil Temperature: ");
    Serial.println((float)soilTemperature / 10.0);
    Serial.print("Soil Conductivity: ");
    Serial.println(soilConductivity);
    Serial.print("Soil pH: ");
    Serial.println((float)soilPH / 10.0);
    Serial.print("Nitrogen: ");
    Serial.println(nitrogenValue);
    Serial.print("Phosphorus: ");
    Serial.println(phosphorusValue);
    Serial.print("Potassium: ");
    Serial.println(potassiumValue);

    mqttClient.publish(NPK_N_TOPIC, String(nitrogenValue).c_str(), true);
    mqttClient.publish(NPK_P_TOPIC, String(phosphorusValue).c_str(), true);
    mqttClient.publish(NPK_K_TOPIC, String(potassiumValue).c_str(), true);
  }
}