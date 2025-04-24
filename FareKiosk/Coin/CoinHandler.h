#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include "../models/Credit.h"

class CoinHandler {
private:
  TaskHandle_t &taskHandle;
  TaskHandle_t rotaryHandle;
  Credit &credit;

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
  CoinHandler(Credit &creditObj, TaskHandle_t &handle);
  void begin();
  void task();
  void processCoin(int &pulse);
};
