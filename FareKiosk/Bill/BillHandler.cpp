#include "BillHandler.h"
#include "../models/Config.h"

BillHandler* BillHandler::instance = nullptr;

BillHandler::BillHandler(Credit &creditObj, TaskHandle_t &handle) 
  : credit(creditObj), taskHandle(handle), interfaceServer(nullptr) {
  instance = this;
}

void BillHandler::addCredit(int amount) {
  taskENTER_CRITICAL(&billMux);
  credit.bill += amount;
  taskEXIT_CRITICAL(&billMux);
}

void BillHandler::checkFare(int fare) {
  taskENTER_CRITICAL(&billMux);
  if ((credit.bill + credit.coin) == fare) {
    // do something
  }
  taskEXIT_CRITICAL(&billMux);
}

void BillHandler::begin(InterfaceServer *interfaceServerObj) {
  Serial.println("BillHandler Initialized");
  pinMode(BILL_PIN, INPUT_PULLUP);  // Set the bill pin as input
  attachInterrupt(digitalPinToInterrupt(BILL_PIN), billIsr, FALLING);

  interfaceServer = interfaceServerObj;

  // Add inhibit pin logic if needed
}

void IRAM_ATTR BillHandler::billIsr() {
  if (instance && instance->isPulseReady) {
    unsigned long now = micros();
    if (now - instance->lastPulseMicros > instance->debounceMicros) {
      instance->lastPulseMicros = now;
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(instance->taskHandle, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
}

void BillHandler::processBill(int &pulseCount) {
  // Decode and print bill value
  int value = 0;
  switch (pulseCount) {
    case 2: value = 20; break;
    case 5: value = 50; break;
    case 10: value = 100; break;
    default:
      Serial.printf("Unknown bill: %d pulses\n", pulseCount);
      break;
  }

  if (value > 0) {
    addCredit(value);
    Serial.printf("Bill inserted: Value = %d\n", credit.bill);
  }

  pulseCount = 0;
}

void BillHandler::taskEntryPoint(void* pvParameters) {
  uint32_t val = 0;
  // Wait indefinitely for notification with value 1
  if (xTaskNotifyWait(0, 0, &val, portMAX_DELAY)) {
    if (val == 2) {
      // Notification with value 1 received, set the flag
      instance->isPulseReady = true;
      BillHandler* instance = static_cast<BillHandler*>(pvParameters);
      instance->taskLoop(); // Call the task loop after flag is set
    }
  }
}

void BillHandler::taskLoop() {



  int pulseCount = 0;
  unsigned long lastPulseTime = 0;

  for (;;) {
    // Wait for ISR to notify pulse
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    pulseCount++;
    lastPulseTime = micros();

    // Wait for end of pulse train
    while (true) {
      checkFare(interfaceServer->getTripData().fare);
      if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10))) {
        pulseCount++;
        lastPulseTime = micros();
      } else if (micros() - lastPulseTime > pulseTimeoutMicros) {
        break;
      }
    }

    processBill(pulseCount);
  }
}

void BillHandler::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "BillSensor",
    2048,
    this,
    1,
    &taskHandle,
    1
  );
}


