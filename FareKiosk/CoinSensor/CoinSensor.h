#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include "../models/SensorData.h" // Assuming you have this


class CoinSensor {
private: 
  static constexpr int NUM_SENSORS = 4;
  const int pinsShort[NUM_SENSORS] = { 32, 33, 27, 14 };
  const int pinsFull[NUM_SENSORS] = { 32, 33, 27, 14 };

  SensorData &sensorData;
  MutexHandle_t &sensorDataMutex;
  
  int coinShort();
  int coinFull();

public:
  CoinSensor(SensorData &sensorDataObj);
  void begin(MutexHandle_t &sensorDataMutexObj);
  void checkSensors();
}
