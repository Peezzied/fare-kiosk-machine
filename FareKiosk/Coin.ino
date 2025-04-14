void IRAM_ATTR coinIsr() {
  coinCount++;
  lastPulseTime = millis();
  isCoinInsert = true;
}

void initCoin() {
  attachInterrupt(COIN, coinIsr, FALLING);
}

void coin() {
  if (millis() - lastPulseTime > debounceDelay) {
    isCoinInsert = false;
    int pulses = coinCount;
    coinCount = 0;

    switch (pulses) {
      case 1:
        Serial.printf("Coin inserted: 10 pesos");
        credit.coin += 10;
        break;
      case 2:
        Serial.printf("Coin inserted: 1 peso");
        credit.coin += 1;
        break;
      case 3:
        Serial.printf("Coin inserted: 5 pesos");
        credit.coin += 5;
        break;
      case 4:
        Serial.printf("Coin inserted: 20 pesos");
        credit.coin += 20;
        break;
    }
  }
}