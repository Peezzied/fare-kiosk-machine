#include "Servos.h"

Servos::Servos(TaskHandle_t &handle) : taskHandle(handle) {}

void Servos::taskEntryPoint(void* pvParameters) {
  Servos* instance = static_cast<Servos*>(pvParameters);
  instance->taskLoop();
}

void Servos::taskLoop() {
  uint32_t notificationValue;
  TickType_t lastActionTime = 0;
  bool isServoAt90 = false;

  for (;;) {
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &notificationValue, 0) == pdTRUE) {
      if (notificationValue == 1) {
        servos[0].write(90);
        isServoAt90 = true;
        lastActionTime = xTaskGetTickCount();
      }
    }

    if (isServoAt90 && (xTaskGetTickCount() - lastActionTime >= pdMS_TO_TICKS(400))) {
      servos[0].write(0);
      isServoAt90 = false;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // small delay to prevent busy loop
  }
}


// void dispenseChange(int changeAmount) {
//   // Initialize counters for each coin type
//   int tens = 0;
//   int fives = 0;
//   int ones = 0;
  
//   // STEP 1: Try to use as many 10-unit coins as possible
//   while (changeAmount >= 10 && coin10Count > 0) {
//     tens++;                // Increment counter for 10-unit coins
//     changeAmount -= 10;    // Subtract 10 from remaining change
//     coin10Count--;         // Decrease available 10-unit coins
//   }
  
//   // STEP 2: Try to use as many 5-unit coins as possible with what's left
//   while (changeAmount >= 5 && coin5Count > 0) {
//     fives++;               // Increment counter for 5-unit coins
//     changeAmount -= 5;     // Subtract 5 from remaining change
//     coin5Count--;          // Decrease available 5-unit coins
//   }
  
//   // STEP 3: Use 1-unit coins for any remaining amount
//   while (changeAmount >= 1 && coin1Count > 0) {
//     ones++;                // Increment counter for 1-unit coins
//     changeAmount -= 1;     // Subtract 1 from remaining change
//     coin1Count--;          // Decrease available 1-unit coins
//   }
  
//   // If changeAmount is still > 0, we couldn't make exact change
//   if (changeAmount > 0) {
//     Serial.println("ERROR: Unable to dispense exact change!");
//     return;
//   }
  
//   // Proceed to actually dispense the coins...
// }

// void dispenseCoin(Servo &servo, int ledPin, int count) {
//   for (int i = 0; i < count; i++) {
//     // Flash LED
//     digitalWrite(ledPin, HIGH);
    
//     // Move servo to dispense position
//     servo.write(180);
//     delay(300);  // Time for servo to move
    
//     // Return servo to rest position
//     servo.write(0);
//     delay(300);  // Time for servo to move back
    
//     // Turn off LED
//     digitalWrite(ledPin, LOW);
    
//     // Small delay between dispensing coins
//     delay(200);
//   }
// }


void Servos::begin() {
  // actionArrayInvoke([&](int i){
  //   servos[i].attach(pinsServo[i]);
  // }, 5);

  // actionArrayInvoke([&](int i){
  //   servos[i].write(0);
  // }, 5);
  servos[0].attach(GATE_PIN);
  servos[0].write(0);
}

void Servos::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "Servos",
    2048,
    this,
    1,
    &taskHandle,
    1);
}