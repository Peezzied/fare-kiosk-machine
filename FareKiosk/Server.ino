#ifdef DEV_MODE
  void initWiFi() {
    WiFi.begin(ssid_sta, pass);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());
  }
#else
  void initWiFI() {
    if (!WiFi.softAP(ssid, pass)) {
      log_e("Soft AP creation failed.");
      while (1);
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
#endif

void initWebSocket() {
  // Capture the required variables (client, data, and len) in the lambda's capture list
  Serial.println("\nInitialized Websocket");
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    // Capture client, data, and len in the lambda and pass them to the webSocketEvent function
    webSocketEvent(server, client, type, arg, data, len, [&]() {
      // Pass client, data, and len to handleTransport
      tripData = handleTransport(client, data, len);  // Pass data and len here
      printTransportData(tripData);
    });
  });
  server.addHandler(&ws);
}

void webSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, std::function<void()> handleEvtData) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.println("\nWebSocket client connected");
      break;

    case WS_EVT_DISCONNECT:
      Serial.println("\nWebSocket client disconnected");
      break;

    case WS_EVT_DATA:
      Serial.println("\nWebSocket client sent data");
      handleEvtData();  // Invoke the callback function (handleEvtData)
      break;

    case WS_EVT_PONG:
      // Handle PONG event if necessary
      Serial.println("\nWebSocket PONG received");
      break;

    case WS_EVT_ERROR:
      // Handle error event if necessary
      Serial.println("\nWebSocket error occurred");
      break;

    default:
      // Handle unknown events
      Serial.println("\nUnknown WebSocket event");
      break;
  }
}
