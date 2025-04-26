#pragma once

#include "Adafruit_Thermal.h"
#include "HardwareSerial.h"

class Receipt {
private:
  TaskHandle_t &taskHandle;
  HardwareSerial &mySerial;
  Adafruit_Thermal printer;

  InterfaceServer *interfaceServer;
  Credit &credit;

  static const uint8_t TX_PIN = 17;
  static const uint8_t RX_PIN = 16;

  void printTicket(const String &fare, const String &origin, const String &destination, const String &serialNumber, const String &date, const String &time, const String &pay, const String &passenger);
  static void taskEntryPoint(void *pvParameters);
  void taskLoop();

public:
  Receipt(Credit &creditObj, TaskHandle_t &handle);
  void begin(InterfaceServer *interfaceServerObj);
  void task();
};