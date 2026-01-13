#include "callbacks.h"
#include "config.h"
#include "setup.h"
#include "ButtonController.h"

// ============================================================
//                   CALLBACK FUNCTIONS
// ============================================================

#if USE_MQTT
// Called when MQTT message is received on subscribed topics
void onMqttMessage(String topic, String payload) {
  propLog("[MQTT] Received: " + payload);

  // Handle reset command (case-insensitive)
  if (payload.equalsIgnoreCase("reset")) {
    propLog("[MQTT] Reset command received");
    propRequestReset();
    return;
  }
}
#endif

#if USE_ESPNOW
// Called when ESP-NOW message is received
void onEspNowReceive(const uint8_t* mac, const uint8_t* data, int len) {
  // Convert to string for parsing
  char msg[32];
  int copyLen = (len < 31) ? len : 31;
  memcpy(msg, data, copyLen);
  msg[copyLen] = '\0';

  Serial.print("[ESP-NOW] Received: ");
  Serial.print(msg);
  Serial.print(" from ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Parse response
  String response = String(msg);
  response.trim();
  response.toUpperCase();

  // Handle ACK from AudioPlayer
  if (response == "ACK") {
    buttonControllerOnAck();
  }
}

// Called when ESP-NOW send completes
void onEspNowSend(const uint8_t* mac, bool success) {
  // Minimal logging to avoid spam during pings
  if (!success) {
    Serial.println("[ESP-NOW] Send FAILED");
  }
}
#endif

// ============================================================
//                    RESET HANDLER
// ============================================================
// Called when reset is triggered via MQTT command.

void onPropReset() {
  propLog("[Reset] Resetting AudioController...");

  // Turn off connection LED
  digitalWrite(CONNECTION_LED_PIN, LOW);

  propLog("[Reset] Complete");
}
