#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

struct TransportData {
  String origin;
  String destination;
  String passenger;
  int fare;
};


// Set these to your desired credentials.
const char *ssid = "Manila to Novaliches Transit";
// const char *password = "yourPassword";
const char *ssid_sta = "Converge_2.4GHz_23F7";
const char *pass_sta = "5bDRg6Tc";

// TCP server at port 80 will respond to HTTP requests
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

TransportData tripData;
bool isComplete;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  Serial.println("");


  // // access point
  // if (!WiFi.softAP(ssid)) {
  //   log_e("Soft AP creation failed.");
  //   while (1);
  // }
  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  // Serial.print("AP IP address: ");
  // Serial.println(WiFi.softAPIP());

  initWiFi();
  initWebSocket();

  server.begin();
}

void loop() {

  // server.handleClient();
  // ws.cleanupClients();
  // client.stop();
  // Serial.println("Done with client");
}
