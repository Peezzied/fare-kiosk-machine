#include "CoinSensor.h"
#include "../models/Config.h"

int CoinSensor::coinShort() {
  int result = -1;
  actionArrayInvoke([&](int i) {
    if (digitalRead(pinsShort[i]) == LOW) {
      result = pinsShort[i];
    }
  }, NUM_SENSORS);
  return result;
}

int CoinSensor::coinFull() {
  int result = -1;
  actionArrayInvoke([&](int i) {
    if (digitalRead(pinsFull[i]) == LOW) {
      result = pinsFull[i];
    }
  }, NUM_SENSORS);
  return result;
}

static void CoinSensor::taskEntryPoint(void* pvParameters) {
  CoinSensor* instance = static_cast<CoinSensor*>(pvParameters);
  instance->taskLoop();  // Call the actual member function
}

void CoinSensor::taskLoop() {
  while (true) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
    int fullPin = coinFull();
    if (fullPin != -1) {
      Serial.printf("Coin full detected on pin: %d\n", fullPin);
      // handle full condition
      taskENTER_CRITICAL();
      sensorData.full = fullPin;
      taskEXIT_CRITICAL();
      xTaskNotifyIndexed(coinTaskHandle, 1, COIN_FULL, eSetBits);
    }

    int shortPin = coinShort();
    if (shortPin != -1) {
      Serial.printf("Coin short detected on pin: %d\n", shortPin);
      // handle short condition
      taskENTER_CRITICAL();
      sensorData.shortEmpty = shortPin;
      taskEXIT_CRITICAL();
      xTaskNotifyIndexed(coinTaskHandle, 1, COIN_SHORT, eSetBits);
    }
  }
}

CoinSensor::CoinSensor(SensorData &sensorDataObj, TaskHandle_t &handle) : taskHandle(handle), sensorData(sensorDataObj) {}

void CoinSensor::begin(TaskHandle_t &handle) {
  coinTaskHandle = handle;
  // coin full
  actionArrayInvoke([&](int i) {
    pinMode(pinsFull[i], INPUT_PULLUP);
  }, NUM_SENSORS);

  // coin short
  actionArrayInvoke([&](int i) {
    pinMode(pinsShort[i], INPUT_PULLUP);
  }, NUM_SENSORS);
}

void CoinSensor::task() {
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
}
