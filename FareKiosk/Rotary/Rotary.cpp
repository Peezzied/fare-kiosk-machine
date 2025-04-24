Rotary::Rotary(TaskHandle_t &handle) : taskHandle(handle) {}

void Rotary::begin() {
// stepper init
}

void Rotary::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "Rotary",
    2048,
    this,
    1,
    &taskHandle,
    1
  );
}

void Rotary::taskEntryPoint(void* pvParameters) {
  Rotary* instance = static_cast<Rotary*>(pvParameters);
  instance->taskLoop();  // Call the actual member function
}

void Rotary::taskLoop() {
  uint32_t notificationValue;

  while (true) {
    stepContinous();
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &notificationValue, portMAX_DELAY) == pdTRUE) {
      if (notificationValue == COIN_READY) {
        // set a flag for stepContinous to consume
      } 
    }
  }
}

void Rotary::stepContinous() {
  // consume flag
}