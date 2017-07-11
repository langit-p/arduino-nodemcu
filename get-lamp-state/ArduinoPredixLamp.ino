#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
//#include <Ethernet.h>
#include <SPI.h>
//ArduinoJson
//#include <ArduinoJson.h>
// RestClient Library
#include "RestClient.h"

#define DEBUG_PRINT 1

#define LEDPIN D7
#define STATUSLEDPIN D0
#define LIGHTSENSOR D1
#define RELAY1 D5
#define RELAY2 D6
#define DHTTYPE DHT22 //Set for 11, 21, or 22
#define USE_SERIAL Serial1

WiFiClientSecure client;

// ---------- /Config ----------//
const char* DEVICE_ID = "devicejogja";
//const char* WIFI_SSID = "Windows Phone4569";
//const char* WIFI_PASSWORD = "87654322";
const char* WIFI_SSID = "Smartpret";
const char* WIFI_PASSWORD = "Kipasangin_99";
const char* SDKHosting = "langit-iot.herokuapp.com";

//variable for REST Client
RestClient rsclient = RestClient(SDKHosting);
String response;

//variable for lightsensor
int lastLightState;
int currentLightState = LOW;
int numberLoop = 0;

//variable for lamp
unsigned long GET_LAMP_INTERVAL = 4000;
unsigned long DETECT_LIGHT_INTERVAL = 4000;
unsigned long SUBSCRIBE_TOPIC_INTERVAL = 2000;
//# of connections
long connection = 0;

unsigned long prevLampMillis = 0;

//function for turning on or off led
void switchLed(bool isOn) {
  digitalWrite(LEDPIN, isOn ? HIGH : LOW);
}

void getLampState(){
  String respons = "";
  rsclient.setContentType("application/json");
  int statusCode = rsclient.get("/services/windservices/lamp_state", &response);
  Serial.print("Status code from server: ");
  if (statusCode >= 200 && statusCode < 300) {
    Serial.println("SUCCESS");  
    Serial.print(response); 
  } else {
    Serial.print("Failed with code ");
    Serial.println(statusCode);
    Serial.print("Response body from server: ");
    Serial.println(response);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");

  rsclient.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Setup!");
}

void loop() {
   unsigned long currentMillis = millis();
  if (currentMillis - prevLampMillis >= GET_LAMP_INTERVAL) {
    prevLampMillis = currentMillis;
    getLampState();  }
  
}
