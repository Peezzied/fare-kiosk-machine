#include "CoinHandler.h"
#include "../models/Config.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj, CoinSensor &coinSensorObj, TaskHandle_t &handle) 
  : credit(creditObj), taskHandle(handle), coinSensor(coinSensorObj) {
  instance = this;
}

void CoinHandler::addCredit(int amount) {
  taskENTER_CRITICAL(&coinMux);
  credit.coin += amount;
  taskEXIT_CRITICAL(&coinMux);
}

void CoinHandler::begin(SensorData &sensorDataObj, MutexHandle_t &sensorDataMutexObj) {
  Serial.println("CoinHandler Initialized");
  pinMode(COIN_PIN, INPUT_PULLUP);  // Set the coin pin as input
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinIsr, FALLING);
  
  sensorData = sensorDataObj;
  sensorDataMutex = sensorDataMutexObj;
  // Add inhibit pin logic if needed
}

void IRAM_ATTR CoinHandler::coinIsr() {
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

void CoinHandler::processCoin(int &pulseCount) {
  // Decode and print coin value
  int value = 0;
  switch (pulseCount) {
    case 1: value = 10; break;
    case 2: value = 5; break;
    case 3: value = 1; break;
    case 4: value = 20; break;
    default:
      Serial.printf("Unknown coin: %d pulses\n", pulseCount);
      break;
  }

  if (value > 0) {
    addCredit(value);
    Serial.printf("Coin inserted: Value = %d\n", credit.coin);
  }

  pulseCount = 0;
}

void CoinHandler::taskEntryPoint(void* pvParameters) {
  CoinHandler* instance = static_cast<CoinHandler*>(pvParameters);
  instance->taskLoop(); 
}

void CoinHandler::taskLoop() {
  int pulseCount = 0;
  unsigned long lastPulseTime = 0;

  for (;;) {
    // Wait for ISR to notify pulse
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    pulseCount++;
    lastPulseTime = micros();

    // checkSensors 
    coinSensor.checkSensors();

    // rotary notification

    // Wait for end of pulse train
    while (true) {
      if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10))) {
        if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE) {
          if (sensorData.full) {
            pulseCount++;
            lastPulseTime = micros();
          }
          // Release mutex after accessing shared data
          xSemaphoreGive(sensorDataMutex);
        }
      } else if (micros() - lastPulseTime > pulseTimeoutMicros) {
        break;
      }
    }

    // maybe use mutex here if problem arises
    processCoin(pulseCount);
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


