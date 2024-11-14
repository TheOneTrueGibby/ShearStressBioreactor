/*  
    This is ThingSpeak secction, remove or add comments accordingly to run properly. Make sure ESP32 is connect to Wi-Fi router in order to push data to ThingSpeak.
    Create your own account and channel at ThingSpeak.com
    For this specific Bioreactor Project channel, login with
     ID: bui2848@vandals.uidaho.edu
     PW: Bioreactor@24
    Channel infomation if needed:
        Link: https://thingspeak.com/channels/2479428/
        Channel number: 2479428
        Write API Key: QXK1OGL1BSWQ4LPP
        Read API Key: LRSWKH2NV2MAPBW0
        Write to channel field: https://api.thingspeak.com/update?api_key=QXK1OGL1BSWQ4LPP&field1=0 //replace field number and value accordingly, refer to function sendToThingSpeak below
        All field list:
            1/ Wi-Fi Signal
            2/ ESP32 LED
            3/ Actual Flow Rate
            4/ Target Flow Rate
            5/ Pump Flow Rate
            6/ Estimated Shear Stress
            7/ System Runtime
            8/ Calculated Reynold's number
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
//#include <ThingSpeak.h>
#include "thingSpeak.hpp"

const char *ssidn = "AirVandalGuest";       // Replace with your WiFi SSID
const char *passwordn = "GoVandals!";       // Replace with your WiFi password

const int LED_BUILTIN = 2;

const char *host = "api.thingspeak.com";    // ThingSpeak API endpoint
const unsigned long postingInterval = 15000; // Interval at which to post (in milliseconds)
unsigned long lastConnectionTime = 0; // Last time data was posted to ThingSpeak
WiFiClient client;

// Default constructor
ThingSpeak::ThingSpeak(){}

void ThingSpeak::init()
{
    /*
        ThingSpeak section
        For University of Idaho, to bypass daily re-authentication on VandalGuest network, remember to register
        the ESP32's MAC address with UI system at help.uidaho.edu/avsetup
        (optional) Print ESP32 IP address and MAC address:
            Serial.print("IP Address: "); Serial.println(WiFi.localIP());
            Serial.print("MAC Address: "); Serial.println(WiFi.macAddress());
    */
        // Assuming Wi-Fi credentials are already assigned in ssid and password (either directly in code or through GUI)
        Serial.print("Connecting to "); Serial.println(ssidn);
        WiFi.begin(ssidn, passwordn);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");
    pinMode(LED_BUILTIN, OUTPUT); // Built-in blue LED on the ESP32 (using this to confirm system ON/OFF)
}

void ThingSpeak::sendToThingSpeak_field1(int32_t rssi)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Wi-Fi Signal Strength (RSSI): ");
    Serial.println(rssi);
}

void ThingSpeak::sendToThingSpeak_field2(int ledStatus)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("LED Status: ");
    Serial.println(ledStatus);
}

void ThingSpeak::sendToThingSpeak_field3(double actualFlow)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Actual Flow Rate: ");
    Serial.println(actualFlow);
}

void ThingSpeak::sendToThingSpeak_field4(double targetFlow)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Target Flow Rate: ");
    Serial.println(targetFlow);
}

void ThingSpeak::sendToThingSpeak_field5(double pumpFlow)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Pump Flow Rate: ");
    Serial.println(pumpFlow);
}

void ThingSpeak::sendToThingSpeak_field6(double shearStress)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Estimated Shear Stress: ");
    Serial.println(shearStress);
}

void ThingSpeak::sendToThingSpeak_field7(long runTime)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("System Runtime (ms): ");
    Serial.println(runTime);
}

void ThingSpeak::sendToThingSpeak_field8(double calculatedR)
{
    Serial.println("Sending data to serial monitor...");
    Serial.print("Calculated Reynold's Number: ");
    Serial.println(calculatedR);
}

void ThingSpeak::getWifiAndLed()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // Measure WiFi signal strength and print
        int32_t rssi = WiFi.RSSI();
        Serial.print("Signal strength (RSSI): ");
        Serial.println(rssi);

        // LED turn on when Wi-Fi connected, update LED status on ThingSpeak as well
        digitalWrite(LED_BUILTIN, HIGH);
        int ledStatus = 1;
        sendToThingSpeak_field2(ledStatus);

        // Check if it's time to send a new measurement to ThingSpeak
        unsigned long currentTime = millis();
        if (currentTime - lastConnectionTime >= postingInterval)
        {
            // Send the data to ThingSpeak
            sendToThingSpeak_field1(rssi);
        }
    } 
    else 
    {
        // LED turn off when Wi-Fi is not connected, update LED status on ThingSpeak as well
        digitalWrite(LED_BUILTIN, LOW);
        int ledStatus = 0;
        sendToThingSpeak_field2(ledStatus);

        // Not connected to WiFi, attempt to reconnect
        Serial.println("WiFi not connected, attempting to reconnect...");
        //WiFi.begin(ssid, password);
    }
    
    delay(15000); // Send to ThingSpeak every 15 seconds

}

void ThingSpeak::getSystemRuntime()
{
    unsigned long runTime = millis();
    sendToThingSpeak_field7(runTime);   
    delay(15000); // Send to ThingSpeak every 15 seconds
}