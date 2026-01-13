# OER.Room.Monsters.AudioController

ESP32-S3 button controller for the Monsters escape room audio system.

## Purpose

A physical button interface that remotely controls the AudioPlayer prop via ESP-NOW. Provides play, stop, and volume controls with connection status indication.

## Hardware

**Target Board:** ESP32-S3-DevKitC-1 (N8R2 variant)

### Wiring

| GPIO | Function | Notes |
|------|----------|-------|
| 35 | Play Button | Active LOW, internal pullup |
| 36 | Stop Button | Active LOW, internal pullup |
| 37 | Volume Up Button | Active LOW, internal pullup |
| 38 | Volume Down Button | Active LOW, internal pullup |
| 40 | Connection LED | HIGH = connected to AudioPlayer |
| 8 | ACK LED | Flashes 200ms on ACK, low drive strength |
| 3 | Speaker | 100ms beep on button press (2kHz) |
| 48 | Heartbeat LED | Onboard RGB (status indicator) |

### Button Wiring

Each button connects between the GPIO pin and GND. Internal pullup resistors are enabled, so no external resistors needed.

```
GPIO Pin ----+---- Button ---- GND
             |
        (internal pullup)
```

### LEDs

- **Connection LED (GPIO 40)**: Standard LED with current-limiting resistor (330-470 ohm)
- **ACK LED (GPIO 8)**: Uses low drive strength (~5mA), flashes briefly when ACK received

### Speaker

Small piezo or speaker on GPIO 3. Produces a 2kHz beep for 100ms when any button is pressed.

## Features

- **4 control buttons**: Play, Stop, Volume Up, Volume Down
- **Audio feedback**: Speaker beeps on each button press
- **Connection monitoring**: Pings AudioPlayer every 5 seconds
- **Connection LED**: Solid when connected to AudioPlayer
- **ACK LED**: Flashes briefly when AudioPlayer responds
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
