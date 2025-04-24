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
  volatile bool coinInsert;

  static constexpr int COIN_PIN = 33;
  const int debounceDelay = 120;
  volatile unsigned long lastPulseTime = 0;
  volatile int coinCount = 0;
  portMUX_TYPE coinMux = portMUX_INITIALIZER_UNLOCKED;


  static void IRAM_ATTR coinIsr();
  void addCredit(int amount);
  static void taskEntryPoint(void* pvParameters); 
  void taskLoop();

public:
  CoinHandler(Credit &creditObj, TaskHandle_t &handle);
  void begin(TaskHandle_t &rotary);
  void task();
  void processCoin();
};
