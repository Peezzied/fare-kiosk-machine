#include "InterfaceServer/InterfaceServer.h"
#include "InterfaceServer/InterfaceServer.cpp"
#include "Coin/CoinHandler.h"
#include "Coin/CoinHandler.cpp"
#include "Bill/BillHandler.h"
#include "Bill/BillHandler.cpp"
#include "CoinSensor/CoinSensor.h"
#include "CoinSensor/CoinSensor.cpp"
#include "Receipt/Receipt.h"
#include "Receipt/Receipt.cpp"
#include "Servos/Servos.h"
#include "Servos/Servos.cpp"

SemaphoreHandle_t dataAvailableSemaphore;
TaskHandle_t coinTask = NULL;
TaskHandle_t billTask = NULL;
TaskHandle_t receiptTask = NULL;
TaskHandle_t servosTask = NULL;


Credit credit;
// SensorData sensorData;
// SemaphoreHandle_t sensorDataMutex;


InterfaceServer interfaceServer;
Receipt receipt(credit, receiptTask);
CoinHandler coinHandler(credit, coinTask);
BillHandler billHandler(credit, billTask);
// CoinSensor coinSensor(sensorData);
Servos servos(servosTask);


void setup() {
  dataAvailableSemaphore = xSemaphoreCreateBinary();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");

  interfaceServer.begin(&billTask, &coinTask);
  interfaceServer.beginWebsocket();

  servos.begin();
  servos.task();
  
  // coinSensor.begin(&sensorDataMutex);
  receipt.begin(&interfaceServer);
  receipt.task();

  coinHandler.begin(&interfaceServer, receiptTask, servosTask);
  coinHandler.task();

  billHandler.begin(&interfaceServer, receiptTask);
  billHandler.task();



}

void loop() {

}
