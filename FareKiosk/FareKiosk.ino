#include "models/Credit.h"
#include "InterfaceServer/InterfaceServer.h"
#include "InterfaceServer/InterfaceServer.cpp"
#include "Coin/CoinHandler.h"
#include "Coin/CoinHandler.cpp"
// #include "CoinSensor/CoinSensor.h"
// #include "CoinSensor/CoinSensor.cpp"

TaskHandle_t coinTask = NULL;


Credit credit;

CoinHandler coinHandler(credit, coinTask);
// InterfaceServer interfaceServer;



void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  coinHandler.begin();
  coinHandler.task();

  // interfaceServer.begin();
  // interfaceServer.beginWebsocket();
}

void loop() {
  // coin();
}
