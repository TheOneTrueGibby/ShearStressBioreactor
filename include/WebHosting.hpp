/************************************************************************
Gibson Moseley - WebHosting.hpp
set up to host website and data exchange between esp32 hosted website and software
used this tutorial: https://m1cr0lab-esp32.github.io/remote-control-with-websocket/web-server-setup/ for starting point
*************************************************************************/

#ifndef WEBHOSTING_HPP
#define WEBHOSTING_HPP

//All necessary library includes
#include <WiFi.h>
#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

//Set up server hosting on esp32
WiFiManager wifiManager;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//File includes
//#include "Routine.hpp"

//Function delcerations
void initSPIFFS();
void initWebServer();
void initWebServer();
void initWebSocket();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

//Call all necassry function to setup website/websocket hosting
void initWebSetup() {
  initSPIFFS();
  //wifiManager.startConfigPortal(); //used to force setup page if needing change wifi
  wifiManager.setSTAStaticIPConfig(IPAddress(192,168,1,141), IPAddress(192,168,1,142), IPAddress(255,255,255,0));
  wifiManager.autoConnect("BioCapstoneESP"); //first parameter is name of access point, second is the password (if used) for the autoconnect feild
  initWebSocket();
  initWebServer();
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html", false);
}

void initWebServer() {
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    
    //Parse the incoming message, which is in the format: routineName;shearStress;runTime;breakTime;repetitions
    //Get the index of all the semicolon sperators
    String message = String((char*)data);
    int separator1 = message.indexOf(';');
    int separator2 = message.indexOf(';', separator1 + 1);
    int separator3 = message.indexOf(';', separator2 + 1);
    int separator4 = message.indexOf(';', separator3 + 1);
    
    //Get each part of the string making sure to not include the ';'
    String routineName = message.substring(0, separator1);
    String shearStressStr = message.substring(separator1 + 1, separator2);
    String runTimeStr = message.substring(separator2 + 1, separator3);
    String breakTimeStr = message.substring(separator3 + 1, separator4);
    String repetitionsStr = message.substring(separator4 + 1);

    //Convert the string values to appropriate types for the routine function
    float shearStress = shearStressStr.toDouble();
    float runTime = runTimeStr.toDouble();
    float breakTime = breakTimeStr.toDouble();
    int repetitions = repetitionsStr.toInt();

    //Call setRoutine function with the extracted values
    //Serial.print("Running Routine");
    //setRoutine(routineName.c_str(), runTime, breakTime, shearStress, repetitions);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

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

#endif