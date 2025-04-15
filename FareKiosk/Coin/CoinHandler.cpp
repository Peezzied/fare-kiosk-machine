#include "CoinHandler.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj) 
  : credit(creditObj), coinInsert(false) {
  instance = this;
}

void CoinHandler::begin() {
  attachInterrupt(COIN_PIN, coinIsr, FALLING);
}

void CoinHandler::processCoin() {
  if (millis() - lastPulseTime > debounceDelay) {
    coinInsert = false;
    int pulses = coinCount;
    coinCount = 0;

    switch (pulses) {
      case 1:
        Serial.printf("Coin inserted: 10 pesos\n");
        credit.coin += 10;
        break;
      case 2:
        Serial.printf("Coin inserted: 1 peso\n");
        credit.coin += 1;
        break;
      case 3:
        Serial.printf("Coin inserted: 5 pesos\n");
        credit.coin += 5;
        break;
      case 4:
        Serial.printf("Coin inserted: 20 pesos\n");
        credit.coin += 20;
        break;
      default:
        break;
    }
  }
}

Credit CoinHandler::getCredit() const {
  return credit;
}

bool CoinHandler::isCoinInserted() const {
  return coinInsert;
}

void IRAM_ATTR CoinHandler::coinIsr() {
  if (instance) {
    instance->coinCount++;
    instance->lastPulseTime = millis();
    instance->coinInsert = true;
  }
}
