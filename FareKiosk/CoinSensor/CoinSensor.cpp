#include "Arduino.h"
#include "KioskInstance.h"
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/portmacro.h"
}

class CoinSensor {
private: 
  const int pinsShort[] = { 32, 33, 27, 14 };
  const int pinsFull[] = { 32, 33, 27, 14 };

  TaskHandle_t taskHandle;

  int CoinSensor::coinShort() {
    actionArrayInvoke([&](int *i) {
      if (digitalRead(pinsShort[i]) == LOW) {
        return pins[i];  // Return the pin number that is LOW
      }
      return -1;
    });
  }

  int CoinSensor::coinFull() {
    actionArrayInvoke([&](int *i){
      if (digitalRead(pinsFull[i]) == LOW) {
        return pins[i];
      }
      return -1;
    });
  }

  static void CoinSensor::taskEntryPoint(void* pvParameters) {
    CoinSensor* instance = static_cast<CoinSensor*>(pvParameters);
    instance->taskLoop();  // Call the actual member function
  }

  void CoinSensor::taskLoop() {
    while (true) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      Serial.println("Running task...");
      vTaskDelay(pdMS_TO_TICKS(1000));

      if (isCoinFull()) {
        // Set the COIN_FULL bit
        // xEventGroupSetBits(machineEventGroup, COIN_FULL_BIT);
      }

      if (isCoinEmpty()) {
        // Set the COIN_EMPTY bit
        // xEventGroupSetBits(machineEventGroup, COIN_EMPTY_BIT);
      }
    }
  }


public:
  CoinSensor::CoinSensor(TaskHandle_t handle) : taskHandle(handle) {}

 void CoinSensor::begin() {

  // coin short
  actionArrayInvoke([&](int *i){
    pinMode(pinsShort[i], INPUT_PULLUP);
  }, sizeof(pins) / sizeof(pinsShort[0]));

  // coinFull
  actionArrayInvoke([&](int *i){
    pinMode(pinsShort[i], INPUT_PULLUP)
  }, sizeof(pins) / sizeof(pinsFull[0]));

 }

  void CoinSensor::task() {
    xTaskCreatePinnedToCore(
      taskEntryPoint,  // Static function
      "CoinSensor",    // Task name
      2048,            // Stack size
      this,            // Pass 'this' to access member data
      1,               // Priority
      taskHandle,            // Task handle
      1                // Core (0 or 1)
    );
  }
  
  void CoinSensor::notifyExternal(TaskHandle_t *externalHandle) {
    if (externalHandle != nullptr) {
      xTaskNotifyGive(externalHandle);
    }
  }
}
