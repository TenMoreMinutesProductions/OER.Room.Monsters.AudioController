# OER.Room.Monsters.AudioController

ESP32-S3 button controller for the Monsters escape room audio system.

## Purpose

A physical button interface that remotely controls the AudioPlayer prop via ESP-NOW. Provides play, stop, and volume controls with connection status indication.

## Hardware

**Target Board:** ESP32-S3-DevKitC-1 (N8R2 variant)

### Wiring

| GPIO | Function | Notes |
|------|----------|-------|
| 4 | Play Button | Active LOW, internal pullup |
| 5 | Stop Button | Active LOW, internal pullup |
| 6 | Volume Up Button | Active LOW, internal pullup |
| 7 | Volume Down Button | Active LOW, internal pullup |
| 13 | Connection LED | HIGH = connected to AudioPlayer |
| 48 | Heartbeat LED | Onboard RGB (status indicator) |

### Button Wiring

Each button connects between the GPIO pin and GND. Internal pullup resistors are enabled, so no external resistors needed.

```
GPIO Pin ----+---- Button ---- GND
             |
        (internal pullup)
```

### Connection LED

Standard LED with current-limiting resistor (330-470 ohm) between GPIO 13 and GND.

## Features

- **4 control buttons**: Play, Stop, Volume Up, Volume Down
- **Connection monitoring**: Pings AudioPlayer every 5 seconds
- **Status LED**: Indicates connection to AudioPlayer
- **Heartbeat LED**: Shows device is running normally

## ESP-NOW Commands

Commands sent to AudioPlayer:

| Command | Trigger |
|---------|---------|
| `PLAY` | Play button pressed |
| `STOP` | Stop button pressed |
| `VOL_UP` | Volume up button pressed |
| `VOL_DOWN` | Volume down button pressed |
| `PING` | Automatic every 5 seconds |

AudioPlayer responds with `ACK` to confirm receipt.

## Connection Status

- **LED ON**: ACK received within last 10 seconds
- **LED OFF**: No ACK received (AudioPlayer offline or out of range)

## Modules Enabled

- ESP-NOW (Host mode - broadcasts to all clients)
- Heartbeat (RGB LED status indicator)

## Build Commands

```bash
pio run                  # Build
pio run --target upload  # Upload to board
pio run --target clean   # Clean build
pio device monitor       # Serial monitor (115200 baud)
```

## Related Projects

- [OER.Room.Monsters.AudioPlayer](https://github.com/TenMoreMinutesProductions/OER.Room.Monsters.AudioPlayer) - Receives commands and controls DY-HV20T audio module
