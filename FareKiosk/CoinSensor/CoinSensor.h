#pragma once

#include "Arduino.h"
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/portmacro.h"
}
#include "../models/SensorData.h" // Assuming you have this


class CoinSensor {
private: 
  static constexpr int NUM_SENSORS = 4;
  const int pinsShort[NUM_SENSORS] = { 32, 33, 27, 14 };
  const int pinsFull[NUM_SENSORS] = { 32, 33, 27, 14 };

  SensorData &sensorData;
  TaskHandle_t &taskHandle;
  TaskHandle_t coinTaskHandle;
  
  int coinShort();
  int coinFull();
  static void taskEntryPoint(void* pvParameters);
  void taskLoop();

public:
  CoinSensor(SensorData &sensorDataObj, TaskHandle_t &handle);
  void begin(TaskHandle_t &handle);
  void task();
}
