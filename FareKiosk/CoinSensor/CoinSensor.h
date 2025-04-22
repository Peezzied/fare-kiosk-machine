class CoinSensor {
private: 
  const int pinsShort[] = { 32, 33, 27, 14 };
  const int pinsFull[] = { 32, 33, 27, 14 };

  TaskHandle_t taskHandle;
  
  int coinShort();
  int coinFull();
  static void taskEntryPoint(void* pvParameters);
  void taskLoop();

public:
  CoinSensor(TaskHandle_t taskHandle) : taskHandle(taskHandle) {}
  void begin();
  void task();
  void notifyExternal(TaskHandle_t *externalHandle);
}
