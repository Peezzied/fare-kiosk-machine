#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

#define DEV_MODE

//Servos
Servo dispense1;
Servo dispense2;
Servo dispense3;
Servo dispense4;
Servo gate;
Servo servos[] = {dispense1, dispense2, dispense3, dispense4, gate};

// Crediting
struct Credit {
  int coin;
  int bill;
};
Credit credit;

// Coin
const int COIN = 33;
const int debounceDelay = 120;
unsigned long lastPulseTime = 0;
volatile int coinCount = 0;
volatile bool isCoinInsert = false;

// Server
struct TransportData {
  String origin;
  String destination;
  String passenger;
  int fare;
};
TransportData tripData;
const char *ssid = "Manila to Novaliches Transit";
const char *ssid_sta = "Converge_2.4GHz_23F7";
const char *pass = "5bDRg6Tc";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  initCoin();

  initWiFi();
  initWebSocket();

  server.begin();
}

void loop() {
  coin();
}
