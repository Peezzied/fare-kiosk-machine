#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <freertos/semphr.h>
#include "../models/Credit.h"
#include "../models/SensorData.h"
#include "../CoinSensor/CoinSensor.h"

class BillHandler {
private:
  TaskHandle_t &taskHandle;
  
  Credit &credit;

  static BillHandler* instance;

  static const uint8_t BILL_PIN = 19;
  volatile unsigned long lastPulseMicros = 0;
  const unsigned long debounceMicros = 75000;
  const unsigned long pulseTimeoutMicros = 300000;
  portMUX_TYPE billMux = portMUX_INITIALIZER_UNLOCKED;


  static void IRAM_ATTR billIsr();
  void addCredit(int amount);
  void processBill(int &pulse);
  static void taskEntryPoint(void* pvParameters); 
  void taskLoop();

public:
  BillHandler(Credit &creditObj, TaskHandle_t &handle);
  void begin();
  void task();
};
