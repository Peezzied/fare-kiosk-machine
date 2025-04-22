#include "Arduino.h"
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/portmacro.h"
}

class Dispenseer {
private:
  TaskHandle_t taskHandle;

  void getStorage() {
    taskENTER_CRITICAL()
    
    taskEXIT_CRITICAL()
  }

  
  static void taskEntryPoint(void* pvParameters) {
    Dispenseer* instance = static_cast<Dispenseer*>(pvParameters);
    instance->taskLoop();  // Call the actual member function
  }

  void taskLoop() {
    while (true) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      Serial.println("Running task...");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

public:
  Dispenser(TaskHandle_t handle) : taskHandle(handle) {}

  void begin() {
  #ifdef DEV_MODE
    dispense1.attach(14);
    dispense2.attach(27);
    dispense3.attach(26);
    dispense4.attach(25);
    // gate.attach(33);
  #else
    dispense1.attach(25);
    dispense2.attach(26);
    dispense3.attach(12);
    dispense4.attach(23);
    // gate.attach(15);
  #endif
    actionArrayInvoke([&](int i) {
      servos[i].write(0);
    }, (sizeof(servos) / sizeof(servos[0])));
  }
  
  void task() {
    
  }
}

void initDispense() {
#ifdef DEV_MODE
  dispense1.attach(14);
  dispense2.attach(27);
  dispense3.attach(26);
  dispense4.attach(25);
  // gate.attach(33);
#else
  dispense1.attach(25);
  dispense2.attach(26);
  dispense3.attach(12);
  dispense4.attach(23);
  // gate.attach(15);
#endif
  actionArrayInvoke([&](int i) {
    servos[i].write(0);
  }, (sizeof(servos) / sizeof(servos[0])));
}

void dispense() {
  for (int i = 0; i < 3; i++) {
  }
}

// void actionInvoke(std::function<void(int)> servo) {
//   for (int i = 0; i < numServos; i++) {
//     servo(i)
//   }
// }