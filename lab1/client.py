import json
import time
import paho.mqtt.client as mqtt # ver 2.1.0


class CoreIoTClient:
    def __init__(self, broker, port, username, access_token, device_name="IOT_DEVICE_2"):
        self.broker = broker
        self.port = port
        self.username = username
        self.access_token = access_token
        self.device_name = device_name
        # Add callback_api_version for paho-mqtt 2.0+ compatibility
        self.client = mqtt.Client(
            client_id=device_name,
            callback_api_version=mqtt.CallbackAPIVersion.VERSION1
        )
        # State variables for RPC control
        self.door_state = True
        self.led_state = False
        self._configure_client()

    def _configure_client(self):
        """Set up MQTT client callbacks and credentials."""
        self.client.username_pw_set(self.username, self.access_token)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.on_subscribe = self._on_subscribe

    def _on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("[INFO] Connected successfully to broker.")
            client.subscribe("v1/devices/me/rpc/request/+")
            # Subscribe to attribute updates to sync door state from server
            # client.subscribe("v1/devices/me/attributes")
            client.subscribe("v1/devices/me/attributes/response/+")
        else:
            print(f"[ERROR] Connection failed with code {rc}")

    def _on_subscribe(self, client, userdata, mid, granted_qos):
        print("[INFO] Subscribed to RPC topic.")

    def _on_message(self, client, userdata, message):
        print(f"[RECV] {message.topic} → {message.payload.decode('utf-8')}")
        try:
            # Handle RPC requests
            if "rpc/request" in message.topic:
                payload = json.loads(message.payload)
                method = payload.get("method")
                params = payload.get("params")
                
                # Extract request ID from topic for response
                request_id = message.topic.split("/")[-1]
                
                print(f"[DEBUG] Method: {method}, Params: {params}, Request ID: {request_id}")
                
                # Call the unified RPC handler
                self._handle_rpc_value(method, params, request_id)
            
            # Handle attribute updates from server
            elif "attributes" in message.topic:
                payload = json.loads(message.payload)
                if "door" in payload:
                    self.door_state = bool(payload["door"])
                    print(f"[INFO] Door state synced from server → {'OPEN' if self.door_state else 'CLOSED'}")
                if "led" in payload:
                    self.led_state = bool(payload["led"])
                    print(f"[INFO] LED state synced from server → {'ON' if self.led_state else 'OFF'}")
                
        except json.JSONDecodeError:
            print("[WARN] Received invalid JSON message.")  
        except KeyError as e:
            print(f"[WARN] Missing key in RPC message: {e}")
        except Exception as e:
            print(f"[ERROR] Unexpected error: {e}")

    def _handle_rpc_value(self, method, params, request_id):
        """Handle setValue and getValue RPC methods."""
        match method: 
            case "setDoorValue":
                self.door_state = bool(params)
                print(f"[INFO] Door set to → {'OPEN' if self.door_state else 'CLOSED'}")
                # Update attributes
                self.client.publish("v1/devices/me/attributes", json.dumps({"door": self.door_state}), qos=1)
                # Send response
                response = {"result": "success", "door": self.door_state}
            case "setLEDValue":
                self.led_state = bool(params)
                print(f"[INFO] LED set to → {'ON' if self.led_state else 'OFF'}")
                # Update attributes
                self.client.publish("v1/devices/me/attributes", json.dumps({"led": self.led_state}), qos=1)
                # Send response
                response = {"result": "success", "led": self.led_state}
            case "getDoorValue":
                response = {"door": self.door_state}
                print(f"[INFO] Sent current door value → {response}")
            case "getLEDValue":
                response = {"led": self.led_state}
                print(f"[INFO] Sent current LED value → {response}")
            case "getValue":
                response = {"door": self.door_state, "led": self.led_state}
                print(f"[INFO] Sent all values → {response}")
            case _:
                print(f"[WARN] Unknown RPC method: {method}")
                response = {"result": "error", "message": f"Unknown method: {method}"}
                
        self.client.publish(f"v1/devices/me/rpc/response/{request_id}", json.dumps(response), qos=1)

    def connect(self):
        """Connect to the MQTT broker and start the network loop."""
        print(f"[INFO] Connecting to {self.broker}:{self.port} as {self.device_name}")
        try:
            self.client.connect(self.broker, self.port)
            self.client.loop_start()
            # Request current attributes after connection
            time.sleep(1)  # Wait for connection to establish
            self.request_attributes()
        except Exception as e:
            print(f"[ERROR] Connection failed: {e}")
            raise

    def request_attributes(self):
        """Request current attribute values from the server."""
        request_id = int(time.time())
        request_topic = f"v1/devices/me/attributes/request/{request_id}"
        self.client.publish(request_topic, json.dumps({"sharedKeys": "door,led"}), qos=1)
        print("[INFO] Requested current attributes from server")

    def disconnect(self):
        """Disconnect from the MQTT broker."""
        print("[INFO] Disconnecting from broker...")
        self.client.loop_stop()
        self.client.disconnect()

    def publish_telemetry(self, temperature, humidity, light_intensity, longitude, latitude):
        """Publish telemetry data to ThingsBoard."""
        telemetry = {
            "temperature": temperature,
            "humidity": humidity,
            "light": light_intensity,
            "longitude": longitude,
            "latitude": latitude,
            # "door": self.door_state,
            # "led": self.led_state
        }
        self.client.publish("v1/devices/me/telemetry", json.dumps(telemetry), qos=1)
        print(f"[DATA] Published telemetry → {telemetry}")

    def run(self, interval=5):
        """Continuously send telemetry data and handle RPC requests."""
        temp, humi, light = 30, 50, 100
        latitude, longitude = 10.795442523782544, 106.678183991483
        print("[INFO] Starting main loop - RPC messages handled automatically in background")
        print("[INFO] Available RPC methods: setValue, getValue, toggleDoor, setLED")
        
        try:
            while True:
                # Publish telemetry (RPC handled automatically via callbacks)
                self.publish_telemetry(temp, humi, light, longitude, latitude)
                
                # Simulate sensor changes
                ## Randomly vary temperature, humidity, and light intensity
                temp += (0.5 - time.time() % 1)
                humi += (0.3 - time.time() % 0.6)
                light += (1 - time.time() % 2)

                time.sleep(interval)
        except KeyboardInterrupt:
            print("\n[STOP] Client stopped manually.")
        finally:
            self.disconnect()


if __name__ == "__main__":
    print("Hello Core IoT")

    client = CoreIoTClient(
        broker="app.coreiot.io",
        port=1883,
        username="DN-CLI-001",
        access_token="l1ttled1no",
        device_name="DN-CLI-001"
    )

    client.connect()
    client.run(interval=1)