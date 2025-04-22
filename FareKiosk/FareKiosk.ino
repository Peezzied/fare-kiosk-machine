#include <ESP32Servo.h>
#include "models/Credit.h"
#include "Coin/CoinHandler.h"
#include "Coin/CoinHandler.cpp"
#include "InterfaceServer/InterfaceServer.h"
#include "InterfaceServer/InterfaceServer.cpp"

//Servos
Servo dispense1;
Servo dispense2;
Servo dispense3;
Servo dispense4;
Servo gate;
Servo servos[] = {dispense1, dispense2, dispense3, dispense4, gate};


Credit credit;

CoinHandler coinHandler(credit);
InterfaceServer interfaceServer;



void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  coinHandler.begin();
  interfaceServer.begin();
  interfaceServer.beginWebsocket();

}

void loop() {
  // coin();
}
