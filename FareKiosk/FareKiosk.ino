// #include "InterfaceServer/InterfaceServer.h"
// #include "InterfaceServer/InterfaceServer.cpp"
#include "Coin/CoinHandler.h"
#include "Coin/CoinHandler.cpp"
#include "CoinSensor/CoinSensor.h"
#include "CoinSensor/CoinSensor.cpp"

TaskHandle_t coinTask = NULL;


Credit credit;
SensorData sensorData;
SemaphoreHandle_t sensorDataMutex;


CoinHandler coinHandler(credit, sensorData, coinTask);
CoinSensor coinSensor(sensorData);
// InterfaceServer interfaceServer;



void setup() {
  sensorDataMutex = xSemaphoreCreateMutex();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  coinSensor.begin(&sensorDataMutex);

  coinHandler.begin(&coinSensor, &sensorDataMutex);
  coinHandler.task();

  // coinSensor.task();

  // interfaceServer.begin();
  // interfaceServer.beginWebsocket();
}

void loop() {
  // coin();
}
