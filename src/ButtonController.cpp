#include "ButtonController.h"
#include "config.h"
#include "modules/espnow_module.h"
#include "driver/gpio.h"

// Button state tracking
static bool lastButtonState[4] = {true, true, true, true};  // HIGH = not pressed (pullup)
static unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
static const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce

// Connection tracking
static unsigned long lastAckTime = 0;
static unsigned long lastPingTime = 0;
static bool isConnected = false;

// ACK LED tracking
static unsigned long ackLedOnTime = 0;
static bool ackLedActive = false;
static const unsigned long ACK_LED_DURATION_MS = 500;

// Disconnected blink pattern: 5 short blinks every 2 seconds
static const unsigned long DISCONNECT_BLINK_ON_MS = 50;
static const unsigned long DISCONNECT_BLINK_OFF_MS = 50;
static const int DISCONNECT_BLINK_COUNT = 5;
static const unsigned long DISCONNECT_CYCLE_MS = 2000;
static unsigned long disconnectCycleStart = 0;
static int disconnectBlinkIndex = 0;
static bool disconnectBlinkState = false;

// Speaker LEDC configuration
static const int SPEAKER_LEDC_CHANNEL = 0;
static const int SPEAKER_LEDC_RESOLUTION = 8;
static unsigned long beepEndTime = 0;
static bool beepActive = false;

// Commands for each button
static const char* BUTTON_COMMANDS[] = {"PLAY", "STOP", "VOL_UP", "VOL_DOWN"};
static const int BUTTON_PINS[] = {BTN_PLAY_PIN, BTN_STOP_PIN, BTN_VOLUP_PIN, BTN_VOLDOWN_PIN};

// Send command via ESP-NOW broadcast
static void sendCommand(const char* cmd) {
  Serial.print("[Button] Sending: ");
  Serial.println(cmd);
  espnowBroadcast((const uint8_t*)cmd, strlen(cmd));
}

// Start a beep on the speaker
static void startBeep() {
  ledcWriteTone(SPEAKER_LEDC_CHANNEL, BEEP_FREQUENCY);
  beepEndTime = millis() + BEEP_DURATION_MS;
  beepActive = true;
}

// Stop the beep
static void stopBeep() {
  ledcWriteTone(SPEAKER_LEDC_CHANNEL, 0);
  beepActive = false;
}

void buttonControllerInit() {
  // Initialize button pins with internal pullup
  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  // Initialize connection LED
  pinMode(CONNECTION_LED_PIN, OUTPUT);
  digitalWrite(CONNECTION_LED_PIN, LOW);  // Start off (not connected)

  // Initialize ACK LED with low drive strength
  pinMode(ACK_LED_PIN, OUTPUT);
  digitalWrite(ACK_LED_PIN, LOW);
  gpio_set_drive_capability((gpio_num_t)ACK_LED_PIN, GPIO_DRIVE_CAP_0);

  // Initialize speaker with LEDC
  ledcSetup(SPEAKER_LEDC_CHANNEL, BEEP_FREQUENCY, SPEAKER_LEDC_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_LEDC_CHANNEL);
  ledcWriteTone(SPEAKER_LEDC_CHANNEL, 0);  // Start silent

  Serial.println("[ButtonController] Initialized");
  Serial.print("  Play:       GPIO ");
  Serial.println(BTN_PLAY_PIN);
  Serial.print("  Stop:       GPIO ");
  Serial.println(BTN_STOP_PIN);
  Serial.print("  Vol Up:     GPIO ");
  Serial.println(BTN_VOLUP_PIN);
  Serial.print("  Vol Down:   GPIO ");
  Serial.println(BTN_VOLDOWN_PIN);
  Serial.print("  Conn LED:   GPIO ");
  Serial.println(CONNECTION_LED_PIN);
  Serial.print("  ACK LED:    GPIO ");
  Serial.println(ACK_LED_PIN);
  Serial.print("  Speaker:    GPIO ");
  Serial.println(SPEAKER_PIN);
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
          startBeep();  // Audio feedback
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

  // Update connection LED
  digitalWrite(CONNECTION_LED_PIN, isConnected ? HIGH : LOW);

  // Handle ACK LED timeout (only when connected)
  if (ackLedActive && (now - ackLedOnTime >= ACK_LED_DURATION_MS)) {
    digitalWrite(ACK_LED_PIN, LOW);
    ackLedActive = false;
  }

  // Handle disconnected blink pattern: 5 rapid blinks every 2 seconds
  if (!isConnected && !ackLedActive) {
    unsigned long cycleTime = now - disconnectCycleStart;

    // Reset cycle every 2 seconds
    if (cycleTime >= DISCONNECT_CYCLE_MS || disconnectCycleStart == 0) {
      disconnectCycleStart = now;
      disconnectBlinkIndex = 0;
      cycleTime = 0;
    }

    // 5 blinks: ON-OFF-ON-OFF-ON-OFF-ON-OFF-ON-OFF (500ms total)
    // Each ON and OFF period is 50ms
    unsigned long blinkPeriod = DISCONNECT_BLINK_ON_MS + DISCONNECT_BLINK_OFF_MS;  // 100ms per blink
    unsigned long totalBlinkTime = blinkPeriod * DISCONNECT_BLINK_COUNT;  // 500ms

    if (cycleTime < totalBlinkTime) {
      // Which blink are we in (0-4)?
      int blinkNum = cycleTime / blinkPeriod;
      // Are we in the ON or OFF portion of this blink?
      unsigned long withinBlink = cycleTime % blinkPeriod;
      bool shouldBeOn = (withinBlink < DISCONNECT_BLINK_ON_MS);

      digitalWrite(ACK_LED_PIN, shouldBeOn ? HIGH : LOW);
    } else {
      // In the pause between blink sequences
      digitalWrite(ACK_LED_PIN, LOW);
    }
  } else if (isConnected) {
    // Connection restored - reset disconnect pattern
    disconnectCycleStart = 0;
  }

  // Handle beep timeout
  if (beepActive && (now >= beepEndTime)) {
    stopBeep();
  }

  // Log connection state changes
  if (wasConnected && !isConnected) {
    Serial.println("[ButtonController] Connection LOST - no ACK received");
  } else if (!wasConnected && isConnected) {
    Serial.println("[ButtonController] Connection ESTABLISHED");
  }
}

void buttonControllerOnAck() {
  lastAckTime = millis();

  // Flash ACK LED
  digitalWrite(ACK_LED_PIN, HIGH);
  ackLedOnTime = millis();
  ackLedActive = true;

  Serial.println("[ButtonController] ACK received");
}

bool buttonControllerIsConnected() {
  return isConnected;
}

unsigned long buttonControllerLastAckAge() {
  return millis() - lastAckTime;
}
