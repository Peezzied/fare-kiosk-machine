#pragma once

#include <ESP32Servo.h>

class Servos {
private:
  Servo servos[5];
  // int pinsServo[5] = {  };
  static const uint8_t GATE_PIN = 19;

  TaskHandle_t &taskHandle;
  static void taskEntryPoint(void *pvParameters);
  void taskLoop();

public:
  Servos(TaskHandle_t &handle);
  void begin();
  void task();
};