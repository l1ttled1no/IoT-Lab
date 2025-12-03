#include "Task2_DHT20.h"

float Glb_humidity = 0.0f;
float Glb_temperature = 0.0f;


void Task2_DHT20(void *pvParams) {
    DHT20 dht20;
    // Wire.begin(11, 12); The I2C handles are initialized in main.cpp
    
    while (dht20.begin() == false) {
        Serial.println("[DEBUG][DHT20] DHT20 sensor not connected. Retrying...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    };
    
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for sensor to stabilize
    int8_t err_dht_status = DHT20_OK;
    for (;;){
        err_dht_status = dht20.read();
        if (err_dht_status != DHT20_OK) {
            Serial.printf("[DEBUG][DHT20] Failed to read from DHT20. Error code: %d\n", err_dht_status);
        } else {
            Glb_humidity = dht20.getHumidity();
            Glb_temperature = dht20.getTemperature();
            Serial.printf("[DEBUG][DHT20] Humidity: %.2f %%\n", Glb_humidity);
            Serial.printf("[DEBUG][DHT20] Temperature: %.2f °C\n", Glb_temperature);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Read every 1 second
    }
}