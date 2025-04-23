#pragma once

#include <Arduino.h>
#include "../models/Credit.h"
extern "C" {
  #include "freertos/portmacro.h"
}

class CoinHandler {
private:
  TaskHandle_t taskHandle;

  Credit &credit;
  static CoinHandler* instance;
  bool coinInsert;

  static constexpr int COIN_PIN = 33;
  const int debounceDelay = 120;
  unsigned long lastPulseTime = 0;
  volatile int coinCount = 0;

  static void IRAM_ATTR coinIsr();
  void addCredit();
  static void taskEntryPoint(void* pvParameters);
  void taskLoop();

public:
  CoinHandler(Credit &creditObj);
  void begin();
  void task();
  void processCoin();
  Credit getCredit() const;
  bool isCoinInserted() const;
};
