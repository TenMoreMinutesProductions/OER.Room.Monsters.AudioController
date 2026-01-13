#include "ButtonController.h"
#include "config.h"
#include "modules/espnow_module.h"

// Button state tracking
static bool lastButtonState[4] = {true, true, true, true};  // HIGH = not pressed (pullup)
static unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
static const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce

// Connection tracking
static unsigned long lastAckTime = 0;
static unsigned long lastPingTime = 0;
static bool isConnected = false;

// Commands for each button
static const char* BUTTON_COMMANDS[] = {"PLAY", "STOP", "VOL_UP", "VOL_DOWN"};
static const int BUTTON_PINS[] = {BTN_PLAY_PIN, BTN_STOP_PIN, BTN_VOLUP_PIN, BTN_VOLDOWN_PIN};

// Send command via ESP-NOW broadcast
static void sendCommand(const char* cmd) {
  Serial.print("[Button] Sending: ");
  Serial.println(cmd);
  espnowBroadcast((const uint8_t*)cmd, strlen(cmd));
}

void buttonControllerInit() {
  // Initialize button pins with internal pullup
  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  // Initialize connection LED
  pinMode(CONNECTION_LED_PIN, OUTPUT);
  digitalWrite(CONNECTION_LED_PIN, LOW);  // Start off (not connected)

  Serial.println("[ButtonController] Initialized");
  Serial.print("  Play:     GPIO ");
  Serial.println(BTN_PLAY_PIN);
  Serial.print("  Stop:     GPIO ");
  Serial.println(BTN_STOP_PIN);
  Serial.print("  Vol Up:   GPIO ");
  Serial.println(BTN_VOLUP_PIN);
  Serial.print("  Vol Down: GPIO ");
  Serial.println(BTN_VOLDOWN_PIN);
  Serial.print("  LED:      GPIO ");
  Serial.println(CONNECTION_LED_PIN);
}

void buttonControllerUpdate() {
  unsigned long now = millis();

  // Check each button
  for (int i = 0; i < 4; i++) {
    bool reading = digitalRead(BUTTON_PINS[i]);

    // Check if button state changed (with debounce)
    if (reading != lastButtonState[i]) {
      if (now - lastDebounceTime[i] > DEBOUNCE_DELAY) {
        lastDebounceTime[i] = now;
        lastButtonState[i] = reading;

        // Button pressed (LOW because of pullup)
        if (reading == LOW) {
          sendCommand(BUTTON_COMMANDS[i]);
        }
      }
    }
  }

  // Send periodic ping
  if (now - lastPingTime >= PING_INTERVAL_MS) {
    lastPingTime = now;
    sendCommand("PING");
  }

  // Update connection status
  bool wasConnected = isConnected;
  isConnected = (now - lastAckTime) < CONNECTION_TIMEOUT_MS;

  // Update LED
  digitalWrite(CONNECTION_LED_PIN, isConnected ? HIGH : LOW);

  // Log connection state changes
  if (wasConnected && !isConnected) {
    Serial.println("[ButtonController] Connection LOST - no ACK received");
  } else if (!wasConnected && isConnected) {
    Serial.println("[ButtonController] Connection ESTABLISHED");
  }
}

void buttonControllerOnAck() {
  lastAckTime = millis();
  Serial.println("[ButtonController] ACK received");
}

bool buttonControllerIsConnected() {
  return isConnected;
}

unsigned long buttonControllerLastAckAge() {
  return millis() - lastAckTime;
}
