class Rotary {
private:
  TaskHandle_t &taskHandle;

  bool isReady;

  void stepContinous();
  static void taskEntryPoint(void* pvParameters);
  void taskLoop();
public:
  Rotary(TaskHandle_t &handle);
  void begin();
  void task();
}