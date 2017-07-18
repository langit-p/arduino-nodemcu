#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"

#define DEBUG_PRINT 1

#define DHTPIN D2
#define LEDPIN D7
#define STATUSLEDPIN D0
#define LIGHTSENSOR D1
#define RELAY1 D5
#define RELAY2 D6
#define DHTTYPE DHT22 //Set for 11, 21, or 22
#define USE_SERIAL Serial1

DHT dht(DHTPIN, DHTTYPE, 26);

WiFiClientSecure client;

// ---------- /Config ----------//
const char* DEVICE_ID = "devicejogja";
const char* WIFI_SSID = "Andromax-M2Y-DA7A";
const char* WIFI_PASSWORD = "22800314";
const char* SDKHosting = "langit-iot.herokuapp.com";

//variable for thermal
unsigned long PUBLISH_THERMAL_INTERVAL = 4000;
unsigned long DETECT_LIGHT_INTERVAL = 4000;
unsigned long SUBSCRIBE_TOPIC_INTERVAL = 2000;
unsigned long prevThermalMillis = 0;
unsigned long prevLightMillis = 0;
unsigned long prevSubscriptionMillis = 0;

//variable for lamp
unsigned long GET_LAMP_INTERVAL = 4000;

//variable for REST Client
RestClient rsclient = RestClient(SDKHosting);
String response;

unsigned long prevLampMillis = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Connecting to wifi");

  rsclient.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connected!");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevLampMillis >= GET_LAMP_INTERVAL) {
    prevLampMillis = currentMillis;
    updateReadThermalData();
    getLampState();
  }  
}

//periodically reads the current thermal data and sends to the web server
void updateReadThermalData() {
  unsigned long currentMillis = millis();
  float curTemp = dht.readTemperature();
  String h = String(dht.readHumidity());    // Read temperature as Fahrenheit (isFahrenheit = true)
  String c = String(curTemp);

  if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) {
    if (DEBUG_PRINT) {
      Serial.println("Failed to read from DHT sensor!");
    }
    return;
  } else {
    if (DEBUG_PRINT) {
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(c);
      Serial.print(" *C\t\n");
    }
  };

  //Generate JSON
  //String values = "{\"name\": \"ArduinoPredix " + c + "\", \"datapoints\": [[1, "+ h +"], [2, "+ c +"], [0, 0]]}";
  String values = "{\"name\": \"arduino-test\", \"temperature\": "+ c +", \"humidity\": "+ h +"}";

  //Post into webservice
  response = "";
  rsclient.setContentType("application/json");
  int statusCode = rsclient.post("/services/windservices/temperature", values.c_str(), &response);
  Serial.print("Status code from server: ");
  if (statusCode >= 200 && statusCode < 300) {
    Serial.println("SUCCESS");
    
  } else {
    Serial.print("Failed with code ");
    Serial.println(statusCode);
    Serial.print("Response body from server: ");
    Serial.println(response);
  }
}

void getLampState(){
  String response = "";
  StaticJsonBuffer<200> jsonBuffer;
  rsclient.setContentType("application/json");
  int statusCode = rsclient.get("/services/windservices/lamp_state/sensor_id/arduino-test", &response);
  if (statusCode >= 200 && statusCode < 300) {    
    response.remove(0,1);
    response.trim();      
    Serial.print("Response : ");
    Serial.println(response);
    JsonObject& root = jsonBuffer.parseObject(response);
    if (root.success()) {
      boolean lampState = root["state"];
      if(lampState){
        Serial.println("Turning on lamp");
        digitalWrite(LED_BUILTIN, LOW);
      }else{
        Serial.println("Turning off lamp");
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }else{
      Serial.println("parseObject() failed");
      return;
    }
  } else {
    Serial.print("Failed with code ");
    Serial.println(statusCode);
    Serial.print("Response body from server: ");
    Serial.println(response);
  }
  Serial.println();
  Serial.println();
}
