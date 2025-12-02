# IoT Development Lab 1 - MQTT Client

This project contains MQTT clients for IoT device communication with CoreIoT platform.

## Project Structure

```
.
├── client.py       # IoT Device Client 1 (DN-CLI-001)
├── client_2.py     # IoT Device Client 2 (DN-CLI-002)
└── README.md       # This file
```

## Prerequisites

- Python 3.10 or higher
- pip (Python package installer)

## Installation

### 1. Create Virtual Environment

```bash
# Windows
python -m venv venv

# macOS/Linux
python3 -m venv venv
```

### 2. Activate Virtual Environment

```bash
# Windows (Command Prompt)
venv\Scripts\activate
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process; .\.venv\Scripts\Activate.ps1

# Windows (PowerShell)
venv\Scripts\Activate.ps1

# macOS/Linux
source venv/bin/activate
```

### 3. Install Dependencies

```bash
pip install paho-mqtt==2.1.0
```

## Running the Clients

### Run Client 1 (DN-CLI-001)

```bash
python client.py
```

**Device Configuration:**
- Device Name: `DN-CLI-001`
- Username: `DN-CLI-001`
- Location: 10.795442523782544, 106.678183991483
- Telemetry Interval: 1 second

### Run Client 2 (DN-CLI-002)

```bash
python client_2.py
```

**Device Configuration:**
- Device Name: `DN-CLI-002`
- Username: `DN-CLI-002`
- Location: 10.79681556298107, 106.67916475760599
- Telemetry Interval: 3 seconds

## Features

Both clients support:

### Telemetry Publishing
- Temperature
- Humidity
- Light intensity
- GPS coordinates (longitude, latitude)

### RPC Methods
- `setDoorValue` - Set door state (open/closed)
- `setLEDValue` - Set LED state (on/off)
- `getDoorValue` - Get current door state
- `getLEDValue` - Get current LED state
- `getValue` - Get all current values

### State Management
- Door state (boolean)
- LED state (boolean)
- Automatic attribute synchronization with server

## MQTT Topics

- **Telemetry**: `v1/devices/me/telemetry`
- **Attributes**: `v1/devices/me/attributes`
- **RPC Requests**: `v1/devices/me/rpc/request/+`
- **RPC Responses**: `v1/devices/me/rpc/response/{request_id}`
- **Attribute Requests**: `v1/devices/me/attributes/request/{request_id}`
- **Attribute Responses**: `v1/devices/me/attributes/response/+`

## Stopping the Client

Press `Ctrl+C` to gracefully stop the client.

## Deactivating Virtual Environment

```bash
deactivate
```

## Troubleshooting

### Connection Issues
- Verify broker address: `app.coreiot.io:1883`
- Check username and access token
- Ensure network connectivity

### Module Not Found
```bash
# Ensure virtual environment is activated
# Reinstall dependencies
pip install -r requirements.txt
```

### Permission Errors (Windows PowerShell)
```bash
# If you get execution policy error, run:
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

## Notes

- Both clients use paho-mqtt version 2.1.0 with `CallbackAPIVersion.VERSION1` for compatibility
- Telemetry data is published with QoS 1
- RPC responses are sent automatically when requests are received
- State synchronization happens on connection and when attributes are updated

## Link to the Dashboard
- [CoreIoT Dashboard](https://app.coreiot.io/dashboards/16731540-b7e5-11f0-b5f4-25fce636d3ff)