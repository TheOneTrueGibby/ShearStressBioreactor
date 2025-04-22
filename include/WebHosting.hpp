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
#include "BioreactorVariables.hpp"

//Function delcerations
void initSPIFFS();
void initWebServer();
void initWebServer();
void initWebSocket();
void routineTaskFunction();
void settingsTaskFunction();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

//Task Scheduler object & temp storage of submitted routines
Scheduler scheduler;
String routineDetails = "";
String settingsDetails = "";

//Make the task functions to schedule
Task routineTask(0, 1, routineTaskFunction, &scheduler, false);
Task settingsTask(0, 1, settingsTaskFunction, &scheduler, false);

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
  float shearStress = shearStressStr.toFloat();
  float runTime = runTimeStr.toFloat();
  float breakTime = breakTimeStr.toFloat();
  int repetitions = repetitionsStr.toInt();

  //Call the setRoutine function with the extracted values
  setRoutine(routineName.c_str(), runTime, breakTime, shearStress, repetitions);

  //After the task is executed, we can reset the global variable to avoid running the same routine again
  routineDetails = "";
  routineTask.disable();
}

void settingsTaskFunction() {
  //Access the routine details from the global variable
  String settingsDetailsLocal = settingsDetails;

  //Parse routine details
  int separator1 = settingsDetailsLocal.indexOf(';');
  int separator2 = settingsDetailsLocal.indexOf(';', separator1 + 1);
  int separator3 = settingsDetailsLocal.indexOf(';', separator2 + 1);

  String heightStr = settingsDetailsLocal.substring(0, separator1);
  String widthStr = settingsDetailsLocal.substring(separator1 + 1, separator2);
  String muStr = settingsDetailsLocal.substring(separator2 + 1, separator3);
  String rhoStr = settingsDetailsLocal.substring(separator3 + 1);

  //Convert the string values to appropriate types for the settings function
  float height = heightStr.toFloat();
  float width = widthStr.toFloat();
  float mu = muStr.toFloat();
  float rho = rhoStr.toFloat();

  //Serial.printf("%f\n", height);
  //Serial.printf("%f\n", width);
  //Serial.printf("%f\n", mu);
  //Serial.printf("%f\n", rho);

  //Call the saveBioreactorSettings function with the extracted values
  saveBioreactorSettings(height, width, mu, rho);

  //After the task is executed, we can reset the global variable to avoid running the same routine again
  settingsDetails = "";
  settingsTask.disable();
}

//Function to handle WebSocket messages and schedule tasks using TaskScheduler library
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  //Serial.print("Message webscoket");

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;

      //Sepreate the part of which thing to chnage and the actual message
      String incomingMessageDetails = String((char*)data);
      String mode = "";
      String message = "";

      int semicolonIndex = incomingMessageDetails.indexOf(';');
      mode = incomingMessageDetails.substring(0, semicolonIndex); //Before the first semicolon
      message = incomingMessageDetails.substring(semicolonIndex + 1); //After the first semicolon
      //Serial.printf("The mode is: %s\n", mode);
      //Serial.printf("The message is: %s\n", message);


      //if the mode is for routine or settings do which is appropriate
      if (mode == "routine") {
        //Serial.printf("The message is: %s\n", message);
        //Parse the incoming message, and make sure it is in the format: routineName;shearStress;runTime;breakTime;repetitions
        //Set the global routine variable to store routine details and that it runs only once
        routineDetails = message;
        routineTask.setIterations(1);
        //routineTask.enableIfNot();

        //Add the task to the scheduler (it will run once based on the task's configuration)
        scheduler.addTask(routineTask);

        //Enable the task to start executing
        routineTask.enable();
      }
      else if (mode == "settings") {
        //Serial.printf("The message is: %s\n", message);
        //Parse the incoming message, and make sure it is in the format: channelHeightValue;channelWidthValue;MUValue;RHOValue
        //Set the global settings variable to store routine details
        settingsDetails = message;
        settingsTask.setIterations(1);
        //settingsTask.enableIfNot();

        //Add the task to the scheduler (it will run once based on the task's configuration)
        scheduler.addTask(settingsTask);

        //Enable the task to start executing
        settingsTask.enable();
      }
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