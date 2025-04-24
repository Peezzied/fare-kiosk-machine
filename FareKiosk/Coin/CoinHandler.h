#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include "../models/Credit.h"
#include "../models/SensorData.h"
#include "../CoinSensor/CoinSensor.h"

class CoinHandler {
private:
  TaskHandle_t &taskHandle;
  TaskHandle_t rotaryHandle;
  
  CoinSensor &coinSensor;
  SensorData &sensorData;
  Credit &credit;
  MutexHandle_t &sensorDataMutex;

  static CoinHandler* instance;

  static const uint8_t COIN_PIN = 27;
  volatile unsigned long lastPulseMicros = 0;
  const unsigned long debounceMicros = 2000;    // 3 ms
  const unsigned long pulseTimeoutMicros = 300000; // 300 ms
  portMUX_TYPE coinMux = portMUX_INITIALIZER_UNLOCKED;


  static void IRAM_ATTR coinIsr();
  void addCredit(int amount);
  static void taskEntryPoint(void* pvParameters); 
  void taskLoop();

public:
  CoinHandler(Credit &creditObj, CoinSensor &coinSensorObj, TaskHandle_t &handle);
  void begin(SensorData &sensorDataObj, MutexHandle_t &sensorDataMutexObj);
  void task();
  void processCoin(int &pulse);
};
