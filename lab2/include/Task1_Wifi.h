#ifndef TASK1_WIFI_H
#define TASK1_WIFI_H

#define MAX_MSG_SIZE 256
#define LED_PIN 48

#include <Arduino.h> 
#include <WiFi.h>   
#include <Arduino_MQTT_Client.h>
#include <Server_Side_RPC.h>
#include <ThingsBoard.h>
#include <ArduinoJson.h>
#include <array>

#include "0_Global.h" // Queue handles and global definitions

void Task1_Wifi_MQTT(void *pvParams); 


#endif // TASK1_WIFI_H