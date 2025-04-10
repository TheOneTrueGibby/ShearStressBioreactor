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
#include <TaskScheduler.h>

//Set up server hosting on esp32
WiFiManager wifiManager;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//File includes
#include "Routine.hpp"

//Task Scheduler object & handeler
Scheduler scheduler;
String routineDetails = "";
//Task routineTask(0, TASK_FOREVER, &routineTaskFunction);

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

// void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
//   AwsFrameInfo *info = (AwsFrameInfo*)arg;

//   if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
//       data[len] = 0;

//       //Parse the incoming message, which is in the format: routineName;shearStress;runTime;breakTime;repetitions
//       String routineDetails = String((char*)data);

//       //FreeRTOS Task to handle the routine
//       xTaskCreatePinnedToCore(
//           [](void *pvParameters) {
//               String routineDetails = *(String*)pvParameters;

//               //Get the index of all the semicolon sperators
//               int separator1 = routineDetails.indexOf(';');
//               int separator2 = routineDetails.indexOf(';', separator1 + 1);
//               int separator3 = routineDetails.indexOf(';', separator2 + 1);
//               int separator4 = routineDetails.indexOf(';', separator3 + 1);

//               //Get each part of the string making sure to not include the ';'
//               String routineName = routineDetails.substring(0, separator1);
//               String shearStressStr = routineDetails.substring(separator1 + 1, separator2);
//               String runTimeStr = routineDetails.substring(separator2 + 1, separator3);
//               String breakTimeStr = routineDetails.substring(separator3 + 1, separator4);
//               String repetitionsStr = routineDetails.substring(separator4 + 1);

//               //Convert the string values to appropriate types for the routine function
//               float shearStress = shearStressStr.toDouble();
//               float runTime = runTimeStr.toDouble();
//               float breakTime = breakTimeStr.toDouble();
//               int repetitions = repetitionsStr.toInt();

//               //Call the setRoutine function with the extracted values
//               setRoutine(routineName.c_str(), runTime, breakTime, shearStress, repetitions);

//               //After the routine finishes, delete the task
//               vTaskDelete(NULL);
//           },
//           "Routine Task",            //Task name
//           2048,                      //Stack size
//           &routineDetails,           //Task parameter (routine details)
//           1,                         //Task priority
//           NULL,                      //No task handle needed
//           0                          //Core (0 or 1)
//       );
//   }
// }

//Task function to execute the routine
void routineTaskFunction() {
  //Access the routine details from the global variable
  String routineDetailsLocal = routineDetails;

  //Parse routine details
  int separator1 = routineDetailsLocal.indexOf(';');
  int separator2 = routineDetailsLocal.indexOf(';', separator1 + 1);
  int separator3 = routineDetailsLocal.indexOf(';', separator2 + 1);
  int separator4 = routineDetailsLocal.indexOf(';', separator3 + 1);

  String routineName = routineDetailsLocal.substring(0, separator1);
  String shearStressStr = routineDetailsLocal.substring(separator1 + 1, separator2);
  String runTimeStr = routineDetailsLocal.substring(separator2 + 1, separator3);
  String breakTimeStr = routineDetailsLocal.substring(separator3 + 1, separator4);
  String repetitionsStr = routineDetailsLocal.substring(separator4 + 1);

  //Convert the string values to appropriate types for the routine function
  float shearStress = shearStressStr.toDouble();
  float runTime = runTimeStr.toDouble();
  float breakTime = breakTimeStr.toDouble();
  int repetitions = repetitionsStr.toInt();

  //Call the setRoutine function with the extracted values
  setRoutine(routineName.c_str(), runTime, breakTime, shearStress, repetitions);

  //After the task is executed, we can reset the global variable to avoid running the same routine again
  routineDetails = "";
}

Task routineTask(1000, TASK_FOREVER, routineTaskFunction);

//Function to handle WebSocket messages and schedule tasks using TaskScheduler
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;

      //Parse the incoming message, which is in the format: routineName;shearStress;runTime;breakTime;repetitions
      String incomingRoutineDetails = String((char*)data);

      //Set the global variable to store routine details
      routineDetails = incomingRoutineDetails;

      //Add the task to the scheduler (it will run once based on the task's configuration)
      scheduler.addTask(routineTask);

      //Enable the task to start executing
      routineTask.enable();
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