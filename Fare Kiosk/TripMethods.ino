TransportData handleTransport(WebServer &server) {
  TransportData result;

  if (server.hasHeader("Content-Type") && server.header("Content-Type") == "application/json") {
    String postData = server.arg("plain");

    Serial.printf("Received %s data:\n", server.uri().c_str());
    Serial.println(postData);

    StaticJsonDocument<200> data;
    DeserializationError error = deserializeJson(data, postData);

    if (!error) {
      result.origin = data["origin"].as<String>();
      result.destination = data["destination"].as<String>();
      result.passenger = data["passenger"].as<String>();
      result.fare = data["fare"].as<int>();

      server.send(200, "application/json", "{\"status\":\"OK\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid Content-Type\"}");
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
