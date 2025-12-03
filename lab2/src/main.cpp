#include <Arduino.h>

#include "0_Global.h"
#include "Task1_Wifi.h"
#include "Task2_DHT20.h"



void setup() {

  Serial.begin(115200);
  // For debug purposes and connecting to serial monitor
  for (auto i = 0; i < 10; i++) {
    Serial.println("Starting up...");
    delay(1000);
  }
  
  pinMode(LED_PIN, OUTPUT); 
  Wire.begin(11, 12); 
  
  xTaskCreate(
    Task1_Wifi_MQTT,    // Task function
    "WiFi_MQTT_Task",   // Name of the task (for debugging)
    8192,               // Stack size in words
    NULL,               // Task input parameter
    1,                  // Priority of the task
    NULL                // Task handle
  );
  
  xTaskCreate(
    Task2_DHT20,        // Task function
    "DHT20_Task",      // Name of the task (for debugging)
    8192,               // Stack size in words
    NULL,               // Task input parameter
    1,                  // Priority of the task
    NULL                // Task handle
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}

