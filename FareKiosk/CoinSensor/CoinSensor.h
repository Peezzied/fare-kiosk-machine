class CoinSensor {
private: 
  static constexpr int NUM_SENSORS = 4;
  const int pinsShort[NUM_SENSORS] = { 32, 33, 27, 14 };
  const int pinsFull[NUM_SENSORS] = { 32, 33, 27, 14 };

  SensorData &sensorData;

  TaskHandle_t taskHandle;
  TaskHandle_t coinTaskHandle;
  
  int coinShort();
  int coinFull();
  static void taskEntryPoint(void* pvParameters);
  void taskLoop();

public:
  CoinSensor(SensorData &sensorDataObj, TaskHandle_t handle);
  void begin(TaskHandle_t *coinTaskHandle);
  void task();
}
