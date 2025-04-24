#include "CoinSensor.h"
#include "../models/Config.h"

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
  if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE) {
    SensorData.ready = false;

    int coinEmpty = coinEmpty();
    if (coinEmpty != -1) { 
      SensorData.empty = coinEmpty;
      xSemaphoreGive(sensorDataMutex);
      return;
    }

    int coinFull = coinFull();
    if (coinFull != -1) {
      SensorData.full = coinFull;
      xSemaphoreGive(sensorDataMutex);
      return;
    }

    SensorData.ready = true;

    xSemaphoreGive(sensorDataMutex);
  }
}


CoinSensor::CoinSensor(SensorData &sensorDataObj, TaskHandle_t &handle) : taskHandle(handle), sensorData(sensorDataObj) {}

void CoinSensor::begin(MutexHandle_t &sensorDataMutexObj) {
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

