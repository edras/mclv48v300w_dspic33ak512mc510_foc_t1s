# Smart Motor MQTT Dashboard

Python/Tkinter UI to monitor and control the dsPIC33AK FOC motor drive over MQTT.

## Setup

```bash
cd mqtt_app
python -m venv venv
```

Activate the virtual environment:

- **Windows:** `venv\Scripts\activate`
- **Linux/macOS:** `source venv/bin/activate`

Install dependencies:

```bash
pip install -r requirements.txt
```

## Usage

```bash
python smart_motor.py [broker_ip]
```

`broker_ip` defaults to `192.168.0.5` (the T1S network MQTT broker).

## Controls

| Control | Action | Requires Remote |
|---------|--------|:---:|
| Remote ON/OFF | Enables remote control mode on the board | No |
| Start/Stop | Starts or stops the motor | Yes |
| Speed setpoint | Sends speed reference (400–2000 RPM) | Yes |

When **Remote** is OFF, the dashboard is read-only — telemetry is displayed but controls do not publish.
