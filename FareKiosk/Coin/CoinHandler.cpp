#include "CoinHandler.h"
#include "../models/Config.h"

CoinHandler* CoinHandler::instance = nullptr;

CoinHandler::CoinHandler(Credit &creditObj, SensorData &sensorDataObj, TaskHandle_t &handle) 
  : credit(creditObj), taskHandle(handle), sensorData(sensorDataObj), coinSensor(nullptr), sensorDataMutex(nullptr), interfaceServer(nullptr) {
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
    Serial.println("TRANSACTION SUCCESS");
    xTaskNotify(receiptTask, (1 << 1), eSetBits); 
  }
  taskEXIT_CRITICAL(&coinMux);
}

void CoinHandler::begin(CoinSensor *coinSensorObj, InterfaceServer *interfaceServerObj, SemaphoreHandle_t *sensorDataMutexObj, TaskHandle_t &receiptTaskObj) {
  receiptTask = receiptTaskObj;
  Serial.println("CoinHandler Initialized");
  pinMode(COIN_PIN, INPUT_PULLUP);  // Set the coin pin as input
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinIsr, FALLING);
  
  interfaceServer = interfaceServerObj;
  coinSensor = coinSensorObj;
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
    // coinSensor->checkSensors();

    // rotary notification

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

    // Disable coin acceptor if full

    // if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE) {
    //   if (sensorData.full) {
    //     digitalWrite(INHIBIT_PIN, HIGH);
    //   } else {
    //     digitalWrite(INHIBIT_PIN, LOW);
    //   }
    //   xSemaphoreGive(sensorDataMutex);
    // }

    // maybe use mutex here if problem arises
    processCoin(pulseCount);
    checkFare(interfaceServer->getTripData().fare);
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


