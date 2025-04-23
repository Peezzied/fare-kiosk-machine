#include "CoinHandler.h"
#include "../models/Config.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj, TaskHandle_t &handle) 
  : credit(creditObj), coinInsert(false), taskHandle(handle) {
  instance = this;
}

void CoinHandler::addCredit(int amount) {
  taskENTER_CRITICAL(&coinMux);
  credit.coin += amount;
  taskEXIT_CRITICAL(&coinMux);
}

void CoinHandler::begin() {
  Serial.println("CoinHandler Initialized");
  pinMode(COIN_PIN, INPUT_PULLUP);  // Set the coin pin as input
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinIsr, FALLING);
  // Add inhibit pin logic if needed
}

void CoinHandler::taskEntryPoint(void* pvParameters) {
  CoinHandler* instance = static_cast<CoinHandler*>(pvParameters);
  instance->taskLoop(); 
}

void CoinHandler::taskLoop() {
  uint32_t notificationValue;

  while (true) {
    // Wait for any notification
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &notificationValue, portMAX_DELAY) == pdTRUE) {
      if (notificationValue == 0) {
        // ISR coin pulse signal (no flags)
        processCoin();
      } else {
        // Handle coin acceptor status flags
        if (notificationValue & COIN_FULL) {
          // Disable coin acceptor
        }
        if (notificationValue & COIN_READY) {
          // Enable coin acceptor
          coinInsert = false;
        }
      }
    }

    vTaskDelay(1);
  }
}

void CoinHandler::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "CoinSensor",
    2048,
    this,
    1,
    &taskHandle,
    1
  );
}

void IRAM_ATTR CoinHandler::coinIsr() {
  if (instance) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Send empty notification (value 0) to signal coin detected
    xTaskNotifyFromISR(instance->taskHandle, 0, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken);
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    instance->coinCount++;
    instance->lastPulseTime = millis();
    instance->coinInsert = true;
  }
}

void CoinHandler::processCoin() {
  if ((millis() - lastPulseTime > debounceDelay) && coinInsert) {
    coinInsert = false;
    int pulses = coinCount;
    coinCount = 0;

    switch (pulses) {
      case 1:
        Serial.printf("Coin inserted: 10 pesos\n");
        addCredit(10);
        break;
      case 2:
        Serial.printf("Coin inserted: 1 peso\n");
        addCredit(1);
        break;
      case 3:
        Serial.printf("Coin inserted: 5 pesos\n");
        addCredit(5);
        break;
      case 4:
        Serial.printf("Coin inserted: 20 pesos\n");
        addCredit(20);
        break;
      default:
        break;
    }
  }
}
