#include "InterfaceServer/InterfaceServer.h"
#include "InterfaceServer/InterfaceServer.cpp"
#include "Coin/CoinHandler.h"
#include "Coin/CoinHandler.cpp"
#include "Bill/BillHandler.h"
#include "Bill/BillHandler.cpp"
#include "CoinSensor/CoinSensor.h"
#include "CoinSensor/CoinSensor.cpp"

SemaphoreHandle_t dataAvailableSemaphore;
TaskHandle_t coinTask = NULL;
TaskHandle_t billTask = NULL;


Credit credit;
SensorData sensorData;
SemaphoreHandle_t sensorDataMutex;


InterfaceServer interfaceServer();

CoinHandler coinHandler(credit, sensorData, coinTask);
BillHandler billHandler(credit, coinTask);
CoinSensor coinSensor(sensorData);



void setup() {
  sensorDataMutex = xSemaphoreCreateMutex();
  dataAvailableSemaphore = xSemaphoreCreateBinary();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  interfaceServer.begin(&dataAvailableSemaphore);
  interfaceServer.beginWebsocket();
  
  coinSensor.begin(&sensorDataMutex);

  coinHandler.begin(&coinSensor, &sensorDataMutex, &interfaceServer);
  coinHandler.task();

  billHandler.begin(&interfaceServer);
  billHandler.task();

  // coinSensor.task();

}

void loop() {
  // coin();
}
