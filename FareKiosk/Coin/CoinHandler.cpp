#include "CoinHandler.h"
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/portmacro.h"
}
#include "../models/Config.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj, TaskHandle_t handle) 
  : credit(creditObj), coinInsert(false), taskHandle(handle) {
  instance = this;
}

void CoinHandler::addCredit(int amount) {
  taskENTER_CRITICAL();
  credit.coin += amount;
  taskEXIT_CRITICAL();
}

void CoinHandler::begin() {
  attachInterrupt(COIN_PIN, coinIsr, FALLING);
  // add pinMode for inhibit
}

void CoinHandler::taskEntryPoint(void* pvParameters) {
  CoinHandler* instance = static_cast<CoinHandler*>(pvParameters);
  instance->taskLoop(); 
}

void CoinHandler::taskLoop() {
  uint32_t notificationValue;

  while (true) {
    if (ulTaskNotifyTakeIndexed(0, pdTRUE, 0) > 0) {
      processCoin();
    }
    if (xTaskNotifyWait(1, 0x00, 0xFFFFFFFF, &notificationValue, 0) == pdTRUE) {
      if (notificationValue & COIN_FULL) {
        // disable coin acceptor
      }
      if (notificationValue & COIN_READY) {
        // enable coin acceptor
        // coinInsert false
      }
    }
    vTaskDelay(1);
  }
}

void CoinHandler::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,  // Static function
    "CoinSensor",    // Task name
    2048,            // Stack size
    this,            // Pass 'this' to access member data
    1,               // Priority
    &taskHandle,      // Task handle
    1                // Core (0 or 1)
  );
}

void IRAM_ATTR CoinHandler::coinIsr() {
  if (instance) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Notify the task from ISR
    xTaskNotifyIndexedFromISR(taskHandle, 0, &xHigherPriorityTaskWoken);

    // Request context switch if needed
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

Credit CoinHandler::getCredit() const {
  return credit;
}

bool CoinHandler::isCoinInserted() const {
  return coinInsert;
}

