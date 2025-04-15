#pragma once

#include <Arduino.h>
#include "../models/Credit.h"

class CoinHandler {
private:
  Credit &credit;
  static CoinHandler* instance;
  bool coinInsert;

  static constexpr int COIN_PIN = 33;
  const int debounceDelay = 120;
  unsigned long lastPulseTime = 0;
  volatile int coinCount = 0;

  static void IRAM_ATTR coinIsr();

public:
  CoinHandler(Credit &creditObj);
  void begin();
  void processCoin();
  Credit getCredit() const;
  bool isCoinInserted() const;
};
