#include "BillHandler.h"
#include "../models/Config.h"

BillHandler* BillHandler::instance = nullptr;

BillHandler::BillHandler(Credit &creditObj, TaskHandle_t &handle) 
  : credit(creditObj), taskHandle(handle) {
  instance = this;
}

void BillHandler::addCredit(int amount) {
  taskENTER_CRITICAL(&billMux);
  credit.bill += amount;
  taskEXIT_CRITICAL(&billMux);
}

void BillHandler::begin() {
  Serial.println("BillHandler Initialized");
  pinMode(BILL_PIN, INPUT_PULLUP);  // Set the bill pin as input
  attachInterrupt(digitalPinToInterrupt(BILL_PIN), billIsr, FALLING);

  // Add inhibit pin logic if needed
}

void IRAM_ATTR BillHandler::billIsr() {
  if (instance) {
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
  BillHandler* instance = static_cast<BillHandler*>(pvParameters);
  instance->taskLoop(); 
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
      if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10))) {
        pulseCount++;
        lastPulseTime = micros();
      } else if (micros() - lastPulseTime > pulseTimeoutMicros) {
        break;
      }
    }

    // Disable bill acceptor if full

    // if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE) {
    //   if (sensorData.full) {
    //     digitalWrite(INHIBIT_PIN, HIGH);
    //   } else {
    //     digitalWrite(INHIBIT_PIN, LOW);
    //   }
    //   xSemaphoreGive(sensorDataMutex);
    // }

    // maybe use mutex here if problem arises
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


