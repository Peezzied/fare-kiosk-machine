#include "CoinSensor.h"
#include <freertos/semphr.h>
#include "../models/Config.h"
#include "../Utilities.h"

int CoinSensor::coinEmpty() {
  int result = -1;
  actionArrayInvoke([&](int i) {
    if (digitalRead(pinsShort[i]) == HIGH) {
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

void CoinSensor::checkSensors() {
  if (xSemaphoreTake(*sensorDataMutex, portMAX_DELAY) == pdTRUE) {
    sensorData.ready = false;

    int coinEmpty = this->coinEmpty();
    if (coinEmpty != -1) { 
      sensorData.empty = coinEmpty;
      xSemaphoreGive(*sensorDataMutex);
      return;
    }

    int coinFull = this->coinFull();
    if (coinFull != -1) {
      sensorData.full = coinFull;
      xSemaphoreGive(*sensorDataMutex);
      return;
    }

    sensorData.ready = true;

    xSemaphoreGive(*sensorDataMutex);
  }
}


CoinSensor::CoinSensor(SensorData &sensorDataObj) : sensorData(sensorDataObj) {}

void CoinSensor::begin(SemaphoreHandle_t *sensorDataMutexObj) {
  // coin full
  actionArrayInvoke([&](int i) {
    pinMode(pinsFull[i], INPUT_PULLUP);
  }, NUM_SENSORS);

  // coin short
  actionArrayInvoke([&](int i) {
    pinMode(pinsShort[i], INPUT_PULLUP);
  }, NUM_SENSORS);

  sensorDataMutex = sensorDataMutexObj;
}

