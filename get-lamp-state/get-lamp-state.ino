#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"

WiFiClientSecure client;

// ---------- /Config ----------//
const char* DEVICE_ID = "devicejogja";
const char* WIFI_SSID = "Smartpret";
const char* WIFI_PASSWORD = "Kipasangin_99";
const char* SDKHosting = "langit-iot.herokuapp.com";
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
    getLampState();
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
