#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../models/TransportData.h"

class InterfaceServer {
// public:
private:

  const char *apSSID = "Manila to Novaliches Transit";
  const char *staSSID = "Converge_2.4GHz_23F7";
  const char *password = "5bDRg6Tc";

  SemaphoreHandle_t *dataAvailableSemaphore;

  TaskHandle_t *billTask;
  TaskHandle_t *coinTask;

  AsyncWebServer server;
  AsyncWebSocket ws;
  TransportData tripData;

  void _webSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, std::function<void()> handleEvtData);
  TransportData _handleTransport(AsyncWebSocketClient *client, uint8_t *data, size_t len);
  void _printTransportData(const TransportData &data);

public:
  InterfaceServer();

  void begin(TaskHandle_t *billTaskObj, TaskHandle_t *coinTaskObj);
  void beginWebsocket();

  TransportData &getTripData();
  SemaphoreHandle_t& getSemaphore();
};

