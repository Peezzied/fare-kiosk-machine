#include "InterfaceServer.h"
#include "../models/Config.h"

InterfaceServer::InterfaceServer() : server(80), ws("/ws"), tripData{"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", 0}, billTask(nullptr), coinTask(nullptr) {}

void InterfaceServer::begin(TaskHandle_t *billTaskObj, TaskHandle_t *coinTaskObj) {
  billTask = billTaskObj;
  coinTask = coinTaskObj;
#ifdef DEV_MODE
  WiFi.begin(staSSID, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());
#else
  if (!WiFi.softAP(apSSID, password)) {
    log_e("Soft AP creation failed.");
    while (true);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(apSSID);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
#endif
}

void InterfaceServer::beginWebsocket() {
  Serial.println("\nInitialized WebSocket");

  ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    this->_webSocketEvent(server, client, type, arg, data, len, [=]() {
      tripData = this->_handleTransport(client, data, len);
      this->_printTransportData(tripData);
    });
  });

  server.addHandler(&ws);
  server.begin();
}

void InterfaceServer::_webSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, std::function<void()> handleEvtData) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.println("\nWebSocket client connected");
      connectedClient = client;
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("\nWebSocket client disconnected");
      break;
    case WS_EVT_DATA:
      Serial.println("\nWebSocket client sent data");
      tripData = this->_handleTransport(client, data, len);

      // Notify tasks that new data is available
      xTaskNotify(*coinTask, 1, eSetValueWithOverwrite);
      xTaskNotify(*billTask, 2, eSetValueWithOverwrite);
      break;
    case WS_EVT_PONG:
      Serial.println("\nWebSocket PONG received");
      break;
    case WS_EVT_ERROR:
      Serial.println("\nWebSocket error occurred");
      break;
    default:
      Serial.println("\nUnknown WebSocket event");
      break;
  }
}


TransportData InterfaceServer::_handleTransport(AsyncWebSocketClient *client, uint8_t *data, size_t len) {
  TransportData result;
  StaticJsonDocument<200> dataDoc;
  DeserializationError error = deserializeJson(dataDoc, data);

  if (!error) {
    result.origin = dataDoc["origin"].as<String>();
    result.destination = dataDoc["destination"].as<String>();
    result.passenger = dataDoc["passenger"].as<String>();
    result.date = dataDoc["date"].as<String>();
    result.time = dataDoc["time"].as<String>();
    result.fare = dataDoc["fare"].as<int>();

    String response = "{\"status\":\"OK\"}";
    client->text(response);
  } else {
    String errorResponse = "{\"error\":\"Invalid JSON\"}";
    client->text(errorResponse);
  }

  _printTransportData(result);

  return result;
}

void InterfaceServer::_printTransportData(const TransportData &data) {
  String propertyNames[] = { "origin", "destination", "passenger", "date", "time", "fare" };
  String propertyValues[] = { data.origin, data.destination, data.passenger, data.date, data.time, String(data.fare) };

  for (int i = 0; i < 4; i++) {
    Serial.print(propertyNames[i]);
    Serial.print(": ");
    Serial.println(propertyValues[i]);
  }
}

void InterfaceServer::sendJsonToClient() {
  StaticJsonDocument<200> doc;

  Serial.println("Sent Successful Transaction");
  
  doc["type"] = "update";
  doc["status"] = "complete";

  String jsonString;
  serializeJson(doc, jsonString);

  connectedClient->text(jsonString);
}

SemaphoreHandle_t& InterfaceServer::getSemaphore() {
  return *dataAvailableSemaphore;
}

TransportData& InterfaceServer::getTripData() {
  return tripData;
}
