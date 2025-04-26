#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../models/TransportData.h"

class InterfaceServer {
// public:
private:

  const char *apSSID = "Malinta to Novaliches Transit";
  // const char *staSSID = "Converge_2.4GHz_23F7";
  const char *staSSID = "GlobeAtHome_82B2E_2.4";
  const char *password = "RMM7cCbU";

  SemaphoreHandle_t *dataAvailableSemaphore;
  AsyncWebSocketClient* connectedClient = nullptr;

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
  void sendJsonToClient();
  SemaphoreHandle_t& getSemaphore();
};

