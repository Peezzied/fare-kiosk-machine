


#include "Receipt.h"

Receipt::Receipt(Credit &creditObj, TaskHandle_t &handle)
  : taskHandle(handle), mySerial(Serial2), printer(&mySerial), credit(creditObj) {
}

void Receipt::begin(InterfaceServer *interfaceServerObj) {
  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Baud rate, config, RX, TX
  printer.begin();

  interfaceServer = interfaceServerObj;
}

void Receipt::printTicket(const String &fare, const String &origin, const String &destination,
                          const String &serialNumber, const String &date, const String &time, const String &pay, const String &passenger) {
  printer.begin();
  delay(2000);

  // Text formatting
  printer.setCharset(0);
  printer.setCodePage(0);
  printer.justify('C');
  printer.setSize('M');
  printer.boldOn();

  // Header
  printer.println("Malinta to Novaliches Jeepney Transport Services");

  // Fare
  printer.setSize('L');
  printer.println("\n" + fare);

  // Route
  printer.setSize('M');
  printer.println(origin + " - " + destination);
  printer.boldOff();

  // Serial number, date, time, type
  printer.setSize('S');
  printer.justify('L');
  printer.println("\n\nSN: " + serialNumber + "  DATE: " + date);
  printer.println("TIME: " + time + "    TYPE: " + passenger);
  printer.println("Paid: " + pay);

  // Barcode
  printer.justify('C');
  printer.setBarcodeHeight(100);
  printer.printBarcode(serialNumber.c_str(), ITF);

  printer.println("\n\n");
}

void Receipt::taskEntryPoint(void *pvParameters) {
  Receipt *instance = static_cast<Receipt *>(pvParameters);
  instance->taskLoop();  // Call the task loop after flag is set
}


void Receipt::taskLoop() {
  // Block until a notification is received
  uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  // Check if any of the bits are set (either bit 0 or bit 1)
  if (notificationValue & ((1 << 0) | (1 << 1))) {
    // Proceed with the task logic once any bit is set
    String fare = String(interfaceServer->getTripData().fare);
    String origin = interfaceServer->getTripData().origin;
    String destination = interfaceServer->getTripData().destination;
    String serialNumber = String(312000000000 + random(100000000, 999999999));
    String date = interfaceServer->getTripData().date;
    String time = interfaceServer->getTripData().time;
    String pay = String(credit.bill + credit.coin);
    String passenger = interfaceServer->getTripData().passenger;

    printTicket(fare, origin, destination, serialNumber, date, time, pay, passenger);
    interfaceServer->sendJsonToClient();

  }
}

void Receipt::task() {
  xTaskCreatePinnedToCore(
    taskEntryPoint,
    "Receipt",
    2048,
    this,
    1,
    &taskHandle,
    1);
}
