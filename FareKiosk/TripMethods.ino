TransportData handleTransport(AsyncWebSocketClient *client, uint8_t *data, size_t len) {
  TransportData result;

  // Parse the received data (JSON)
  StaticJsonDocument<200> dataDoc;
  DeserializationError error = deserializeJson(dataDoc, data);

  if (!error) {
    result.origin = dataDoc["origin"].as<String>();
    result.destination = dataDoc["destination"].as<String>();
    result.passenger = dataDoc["passenger"].as<String>();
    result.fare = dataDoc["fare"].as<int>();

    // printTransportData(result);

    // Respond back to the client
    String response = "{\"status\":\"OK\"}";
    client->text(response);
  } else {
    // Handle JSON parsing error
    String errorResponse = "{\"error\":\"Invalid JSON\"}";
    client->text(errorResponse);
  }

  return result;
}

void printTransportData(const TransportData &data) {
  // Create an array of property names
  String propertyNames[] = { "origin", "destination", "passenger", "fare" };
  String propertyValues[] = { data.origin, data.destination, data.passenger, String(data.fare) };

  // Loop through the properties and print each one
  for (int i = 0; i < 4; i++) {
    Serial.print(propertyNames[i]);
    Serial.print(": ");
    Serial.println(propertyValues[i]);
  }
}

