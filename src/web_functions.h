#include <WiFi.h> 
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Arduino_JSON.h>

#include "keys/keys_wifi.h"


IPAddress local_ip; // To Capture IP address for display on screen
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");// Create a WebSocket object
#define NUM_OUTPUTS  4 // Set number of outputs

int outputGPIOs[NUM_OUTPUTS] = {2, 4, 12, 14};// Assign each GPIO to an output

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}



void initWiFi() {
  tft.fillScreen(TFT_WHITE);
  WiFi.begin(ssid, password);
  tft.setCursor(50,50); tft.setTextColor(TFT_BLACK, TFT_WHITE); tft.setTextSize(3);
  Serial.print("Connecting to WiFi ...");
  tft.println("Connecting to WiFi..."); 
  tft.setCursor(50,150);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    tft.print('.');
    delay(1000);
  }
  tft.fillScreen(TFT_BLACK); tft.setTextColor(TFT_WHITE,TFT_BLACK); tft.setTextSize(2);
  tft.setCursor(0, 0); tft.print("SSID: "); tft.println(ssid);
  tft.setCursor(0, 20); tft.print("IP: "); tft.println(WiFi.localIP());
  Serial.println(WiFi.localIP());Serial.println();

  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  }

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html",false);
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  
  // Start server
  server.begin();
}

