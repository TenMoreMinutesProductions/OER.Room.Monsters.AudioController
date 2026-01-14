#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
//                    PROJECT IDENTITY
// ============================================================
// PROJECT_NAME: Set to your repo folder name (e.g., "OER.Room.UpsideDown.Radio")
// DEVICE_IDENTIFIER: Used for MQTT client/topics, mDNS hostname, and OTA
//                    Naming convention: oer-{room}-{prop} (e.g., oer-ud-radio)
#define PROJECT_NAME "OER.Room.Monsters.AudioController"
#define DEVICE_IDENTIFIER "oer-monsters-audiocontroller"

// ============================================================
//                  MODULE ENABLE/DISABLE
// ============================================================
#define USE_WIFI 0      // WiFi connectivity
#define USE_MQTT 0      // MQTT pub/sub (requires WiFi)
#define USE_MDNS 0      // mDNS discovery (requires WiFi)
#define USE_OTA 0       // Over-the-air updates (requires WiFi)
#define USE_ESPNOW 1    // ESP-NOW communication
#define USE_HEARTBEAT 1 // Heartbeat LED status indicator

// Dependency checks
#if USE_MQTT && !USE_WIFI
#error "MQTT requires WiFi to be enabled (USE_WIFI 0)"
#endif
#if USE_MDNS && !USE_WIFI
#error "mDNS requires WiFi to be enabled (USE_WIFI 0)"
#endif
#if USE_OTA && !USE_WIFI
#error "OTA requires WiFi to be enabled (USE_WIFI 0)"
#endif

// ============================================================
//                   WIFI CONFIGURATION
// ============================================================
#if USE_WIFI
#define WIFI_SSID "[QUARANTINED] Virus Detected"
#define WIFI_PASS "Vince1138"
#endif

// ============================================================
//                   MQTT CONFIGURATION
// ============================================================
#if USE_MQTT
#define MQTT_BROKER "192.168.1.218"
#define MQTT_PORT 1883
#define MQTT_TOPIC_PREFIX "SP/"
// Derived values use DEVICE_IDENTIFIER:
// - Client name: "client_" + DEVICE_IDENTIFIER
// - Base topic:  MQTT_TOPIC_PREFIX + DEVICE_IDENTIFIER
// - Status topic: base + "/status"
// - Command topic: base + "/cmd" (auto-subscribed)
#endif

// ============================================================
//                    OTA CONFIGURATION
// ============================================================
#if USE_OTA
#define OTA_PASSWORD "" // Leave empty for no password
#endif

// ============================================================
//                  ESP-NOW CONFIGURATION
// ============================================================
#if USE_ESPNOW
#define ESPNOW_HOST 1 // 1 = Host (broadcasts), 0 = Client (listens)
// Host MAC address - set this to your host device's MAC
// Only used when ESPNOW_HOST is 0 (client mode)
#define ESPNOW_HOST_MAC {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
#endif

// ============================================================
//                HEARTBEAT CONFIGURATION
// ============================================================
#if USE_HEARTBEAT
#define HEARTBEAT_PIN 48 // GPIO48 = onboard RGB LED on ESP32-S3-DevKitC-1
// Heartbeat states - set initial state based on your boot sequence
// HB_BOOTING:    Fast blink (100ms)  - Initializing
// HB_AP_ONLY:    Double pulse        - Awaiting config (captive portal)
// HB_CONNECTING: Triple pulse        - Connecting to WiFi
// HB_NORMAL:     Slow blink (1000ms) - Normal operation
// HB_ERROR:      SOS pattern         - Error state
#endif

// ============================================================
//                     PIN DEFINITIONS
// ============================================================
// ESP32-S3 Pin Notes (YD-ESP32-S3 / DevKitC-1):
// - GPIO 0, 3, 45, 46: Strapping pins (affect boot mode) - avoid for outputs
// - GPIO 19, 20: Reserved for native USB - do not use
// - GPIO 26-32: Not available on N8R2 (used for PSRAM/Flash)
// - GPIO 33-34: Do not exist on ESP32-S3 (chip architecture difference from ESP32)
// - GPIO 35-37: Reserved for PSRAM on N8R2/N8R8 variants; available on N8 (no PSRAM)
// - GPIO 38-42: Available for general use (38-39 fully available; 40-42 may overlap JTAG)
// - GPIO 43-44: Reserved for UART0 (TX/RX)
// - GPIO 48: Onboard RGB LED (directly usable for NeoPixel output)
// - ADC1: GPIO 1-10 (safe to use with WiFi)
// - ADC2: GPIO 11-20 (conflicts with WiFi - avoid for ADC when WiFi active)
// - No DAC pins on S3 - use I2S or external DAC for audio
//
// Safe general-purpose pins: 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 38, 39, 40, 41, 42, 47

// ============================================================
//                  BUTTON CONFIGURATION
// ============================================================
// All buttons are active LOW with internal pullup
#define BTN_PLAY_PIN 35     // Play/resume audio
#define BTN_STOP_PIN 36     // Stop audio
#define BTN_VOLUP_PIN 37    // Volume up
#define BTN_VOLDOWN_PIN 38  // Volume down

// ============================================================
//                CONNECTION LED CONFIGURATION
// ============================================================
// LED is ON when connected to AudioPlayer, OFF when disconnected
#define CONNECTION_LED_PIN 40

// ============================================================
//                  ACK LED CONFIGURATION
// ============================================================
// LED flashes briefly (200ms) when ACK received from AudioPlayer
// Uses low drive strength (GPIO_DRIVE_CAP_0) for minimal current
#define ACK_LED_PIN 8

// ============================================================
//                  SPEAKER CONFIGURATION
// ============================================================
// Small speaker for button press feedback beep
#define SPEAKER_PIN 3
#define BEEP_FREQUENCY 2000  // Hz
#define BEEP_DURATION_MS 100 // milliseconds

// ============================================================
//                  PING/ACK CONFIGURATION
// ============================================================
#define PING_INTERVAL_MS 5000      // Send PING every 5 seconds
#define CONNECTION_TIMEOUT_MS 10000 // Consider disconnected after 10s without ACK

#endif
