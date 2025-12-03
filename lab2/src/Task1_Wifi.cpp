#include "Task1_Wifi.h" 

// RPC method names
constexpr const char RPC_LED_METHOD[] = "setLedSwitchValue";
constexpr const char RPC_LED_KEY[] = "value";
constexpr uint8_t MAX_RPC_SUBSCRIPTIONS = 1U;
constexpr uint8_t MAX_RPC_RESPONSE = 5U;

// Declare mqtt
WiFiClient wfClient; 
Arduino_MQTT_Client mqttClient(wfClient);

// Initialize used apis
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, MAX_RPC_RESPONSE> rpc;
const std::array<IAPI_Implementation*, 1U> apis = {
    &rpc
};

// Initialize ThingsBoard instance with the maximum needed buffer size
ThingsBoard coreiot(mqttClient, MAX_MSG_SIZE, MAX_MSG_SIZE, Default_Max_Stack_Size, apis);

// Statuses for subscribing to rpc
bool subscribed = false;

void processLedSwitchChange(const JsonVariantConst &data, JsonDocument &response) {
    Serial.println("[DEBUG][RPC] Received LED switch state command");
    
    // Debug: Print the entire JSON received
    Serial.print("[DEBUG][RPC] Raw data received: ");
    serializeJson(data, Serial);
    Serial.println();
    
    // Process data - get the switch state
    const bool newState = data.as<bool>();
    
    Serial.print("[DEBUG][RPC] LED state change to: ");
    Serial.println(newState);
    
    // Control LED based on received state
    digitalWrite(LED_PIN, newState);
    
    // Set response - confirms the state change
    response.set(newState);
}

const char* wf_ssid = WIFI_SSID;
const char* wf_password = WIFI_PASSWORD;

void Task1_Wifi_MQTT(void *pvParams) {
    // Initialize serial and WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while(WiFi.status() != WL_CONNECTED) {
        Serial.println("[DEBUG][WIFI] Connecting to WiFi...");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
    
    Serial.println("[DEBUG][WIFI] Connected to WiFi!");
    
    for (;;) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        // Check WiFi connection and reconnect if needed
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[DEBUG][WIFI] WiFi disconnected. Attempting reconnection...");
            WiFi.disconnect();
            while (WiFi.status() != WL_CONNECTED) {
                WiFi.reconnect();
                Serial.println("[DEBUG][WIFI] Reconnecting to WiFi...");
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            Serial.println("[DEBUG][WIFI] Reconnected to WiFi!");
        }
        
        // Check MQTT connection and reconnect if needed
        if (!coreiot.connected()) {
            Serial.printf("[DEBUG][MQTT] Connecting to: (%s) with token (%s)\n", MQTT_BROKER_ADDRESS, MQTT_ACCESS_TOKEN);
            if (!coreiot.connect(MQTT_BROKER_ADDRESS, MQTT_ACCESS_TOKEN, MQTT_BROKER_PORT)) {
                Serial.println("[DEBUG][MQTT] Failed to connect to MQTT broker");
                continue;
            }
            Serial.println("[DEBUG][MQTT] Connected to MQTT broker!");
            // Send device info as attributes upon reconnection
            coreiot.sendAttributeData("macAddress", WiFi.macAddress().c_str());
            coreiot.sendAttributeData("ipAddress", WiFi.localIP().toString().c_str());
            coreiot.sendAttributeData("deviceName", DEVICE_NAME);
            // Latitude and Longitude can be set here if available: 10.795444, 106.678267
            coreiot.sendAttributeData("latitude", LATITUDE);
            coreiot.sendAttributeData("longitude", LONGITUDE);
        }
        
        // Subscribe to RPC if not already subscribed
        if (!subscribed) {
            Serial.println("[DEBUG][RPC] Subscribing for RPC...");
            const std::array<RPC_Callback, MAX_RPC_SUBSCRIPTIONS> callbacks = {
                RPC_Callback{ RPC_LED_METHOD, processLedSwitchChange }
            };
            
            // Perform subscription
            if (!rpc.RPC_Subscribe(callbacks.cbegin(), callbacks.cend())) {
                Serial.println("[DEBUG][RPC] Failed to subscribe for RPC");
                continue;
            }
            
            Serial.println("[DEBUG][RPC] Subscribe done");
            subscribed = true;
            
            // Send device MAC address as attribute
            coreiot.sendAttributeData("macAddress", WiFi.macAddress().c_str());
        }
        // Send temperature and humidity data to telemetry
        coreiot.sendTelemetryData("temperature", Glb_temperature);
        coreiot.sendTelemetryData("humidity", Glb_humidity);
        // Process MQTT messages
        coreiot.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}