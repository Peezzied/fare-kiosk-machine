#include "CoinHandler.h"
#include "../models/Config.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj, TaskHandle_t &handle) 
  : credit(creditObj), taskHandle(handle), interfaceServer(nullptr) {
  instance = this;
}

void CoinHandler::addCredit(int amount) {
  taskENTER_CRITICAL(&coinMux);
  credit.coin += amount;
  taskEXIT_CRITICAL(&coinMux);
}

void CoinHandler::checkFare(int fare) {
  taskENTER_CRITICAL(&coinMux);
  if ((credit.bill + credit.coin) >= fare) {
    instance->isPulseReady = false;
    Serial.println("TRANSACTION SUCCESS");
    xTaskNotify(receiptTask, (1 << 1), eSetBits); 
  }
  taskEXIT_CRITICAL(&coinMux);
}

void CoinHandler::begin(InterfaceServer *interfaceServerObj, TaskHandle_t &receiptTaskObj, TaskHandle_t &servosTaskObj) {
  servosTask = servosTaskObj;
  receiptTask = receiptTaskObj;
  Serial.println("CoinHandler Initialized");
  pinMode(COIN_PIN, INPUT_PULLUP);  // Set the coin pin as input
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinIsr, FALLING);
  
  interfaceServer = interfaceServerObj;
  // Add inhibit pin logic if needed
}

void IRAM_ATTR CoinHandler::coinIsr() {
  if (instance && instance->isPulseReady) {
    unsigned long now = micros();
    if (now - instance->lastPulseMicros > instance->debounceMicros) {
      instance->lastPulseMicros = now;
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xTaskNotifyFromISR(instance->taskHandle, 3, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      Serial.println("Coin ISR");
    }
  }
}

void CoinHandler::processCoin(int &pulseCount) {
  // Decode and print coin value
  int value = 0;
  switch (pulseCount - 1) {
    case 1: value = 10; break;
    case 2: value = 1; break;
    case 3: value = 5; break;
    case 4: value = 20; break;
    default:
      Serial.printf("Unknown coin: %d pulses\n", pulseCount);
      break;
  }

  if (value > 0) {
    addCredit(value);
    Serial.printf("Pulse value Php %d\n", value);
    Serial.printf("Coin inserted: Value = %d\n", credit.coin);
  }

  pulseCount = 0;
}

void CoinHandler::taskEntryPoint(void* pvParameters) {
  uint32_t val = 0;
  // Wait indefinitely for notification with value 1
  if (xTaskNotifyWait(0, 0, &val, portMAX_DELAY)) {
    if (val == 1) {
      // Notification with value 1 received, set the flag
      instance->isPulseReady = true;
      Serial.println("→ Coin task");
      CoinHandler* instance = static_cast<CoinHandler*>(pvParameters);
      instance->taskLoop();  // Call the task loop after flag is set
    }
  }
}

void CoinHandler::taskLoop() {
  int pulseCount = 0;
  unsigned long lastPulseTime = 0;

  for (;;) {
    // Wait for ISR to notify pulse
    uint32_t val = 0;

    // Wait for notification with value 3 (this will block the task until notified)
    if (xTaskNotifyWait(0, 0, &val, pdMS_TO_TICKS(10))) {
      if (val == 3) {
        pulseCount++;
        lastPulseTime = micros();

        // checkSensors
        // coinSensor->checkSensors();

        // Wait for end of pulse train
        while (true) {
          if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10))) {
            pulseCount++;
            lastPulseTime = micros();
          } else if (micros() - lastPulseTime > pulseTimeoutMicros) {
            break;
          }
          // maybe use mutex here if problem arises
        }
        processCoin(pulseCount);
        // checkFare(interfaceServer->getTripData().fare);
      }
    }
  }
}

void CoinHandler::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "CoinHandler",
    2048,
    this,
    1,
    &taskHandle,
    1
  );
}


