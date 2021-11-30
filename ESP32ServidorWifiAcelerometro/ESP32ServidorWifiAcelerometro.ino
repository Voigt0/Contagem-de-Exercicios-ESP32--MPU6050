/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-mpu-6050-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <DNSServer.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char ssid[] = "Unifique_WIFI_382609";
const char* password = "17598023";

// Create AsyncWebServer object on port 80
DNSServer dnsServer;
AsyncWebServer server(80);


// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 100;
unsigned long lastTimeTimer = 0;
unsigned long timerDelay = 100;

// Create a sensor object
Adafruit_MPU6050 mpu;

sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ, accA;
int numEx = 0;
int acumul = 0;
int timerMili = 0;
int timerSeg = 0;
int timerMin = 0;
int timerHora = 0;
float temperature;
int estTempo = 0;

//Gyroscope sensor deviation
float gyroXerror = 0.03;
float gyroYerror = 0.03;
float gyroZerror = 0.03;

// Init MPU6050
void initMPU(){
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("voigtTCCKKK", "12345678");
  dnsServer.start(DNS_PORT, "acelerxise1560", apIP);
}

String getGyroReadings(){
  mpu.getEvent(&a, &g, &temp);

  float gyroX_temp = (g.gyro.x-0.26);
  if(abs(gyroX_temp) > gyroXerror)  {
    gyroX += gyroX_temp/50.00;
  }
  
  float gyroY_temp = (g.gyro.y-0.063);
  if(abs(gyroY_temp) > gyroYerror) {
    gyroY += gyroY_temp/70.00;
  }

  float gyroZ_temp = (g.gyro.z+0.007);
  if(abs(gyroZ_temp) > gyroZerror) {
    gyroZ += gyroZ_temp/90.00;
  }

  readings["gyroX"] = String(gyroX);
  readings["gyroY"] = String(gyroY);
  readings["gyroZ"] = String(gyroZ);

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String getAccReadings() {
  mpu.getEvent(&a, &g, &temp);
  // Get current acceleration values
  accX = a.acceleration.x+0.49;
  accY = a.acceleration.y+0.15;
  accZ = a.acceleration.z-0.40;
  accA = (accX)+(accY)+(accZ);
  if(accA < 0){
    accA = accA*-1;
  }
  if(accA > 14){
    acumul = 1;
  }
  if(accA < 11){
    if(acumul > 0){
      numEx = numEx + 1;
      acumul = 0;
    }
  }
  readings["accX"] = String(accX);
  readings["accY"] = String(accY);
  readings["accZ"] = String(accZ);
  readings["accA"] = String(accA);
  readings["numEx"] = String(numEx);
  String accString = JSON.stringify (readings);
  return accString;
}

String getTemperature(){
  mpu.getEvent(&a, &g, &temp);
  temperature = temp.temperature;
  return String(temperature);
}

String getTime() {
  mpu.getEvent(&a, &g, &temp);

  if(estTempo == 1){
  timerMili = timerMili + 10;
  if(timerMili >= 100){
    timerSeg = timerSeg + 1;
    timerMili = 0;
  }
  if(timerSeg >= 60){
    timerMin = timerMin + 1;
    timerSeg = 0;
  }
  }else{
  timerMili = 0;
  timerSeg = 0;
  timerMin = 0;
  }
  readings["timerMili"] = String(timerMili);
  readings["timerSeg"] = String(timerSeg);
  readings["timerMin"] = String(timerMin);
  String timeString = JSON.stringify (readings);
  return timeString;
}


void setup() {
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initMPU();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroX=0;
    gyroY=0;
    gyroZ=0;
    accX = 0;
    accY = 0;
    accZ = 0;
    accA = 0;
    numEx = 0;
    acumul = 0;
    temperature = 0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetNum", HTTP_GET, [](AsyncWebServerRequest *request){
    numEx = 0;
    acumul = 0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroY=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/initTempor", HTTP_GET, [](AsyncWebServerRequest *request){
    if(estTempo == 0){
      estTempo = 1;
      } else {
        estTempo = 0;
        }
    request->send(200, "text/plain", "OK");
  });
  
  server.serveStatic("/", SPIFFS, "/");
  
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();
}

void loop() {
    dnsServer.processNextRequest();
  if ((millis() - lastTime) > gyroDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getGyroReadings().c_str(),"gyro_readings",millis());
    lastTime = millis();
  }
  if ((millis() - lastTimeAcc) > accelerometerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getAccReadings().c_str(),"accelerometer_readings",millis());
    lastTimeAcc = millis();
  }
  if ((millis() - lastTimeTemperature) > temperatureDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getTemperature().c_str(),"temperature_reading",millis());
    lastTimeTemperature = millis();
  }
  if ((millis() - lastTimeTimer) > timerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getTime().c_str(),"timer_readings",millis());
    lastTimeTimer = millis();
  }
}
