#include "loop.h"
#include "config.h"
#include "setup.h"
#include "ButtonController.h"
#include "esp_task_wdt.h"

// MQTT runs on Core 1 (main loop)
#if USE_MQTT
#endif

// OTA status check (task runs on Core 0)
#if USE_OTA
#endif

// Forward declaration of reset handler (implemented in callbacks.cpp)
extern void onPropReset();

void loopMain() {
  // ============================================================
  // Core 0 Tasks (run automatically via FreeRTOS):
  //   - ESP-NOW (callbacks are event-driven)
  //   - Heartbeat LED
  // ============================================================

  // Feed the watchdog - proves loop is running
  esp_task_wdt_reset();

  // ============================================================
  // Core 1 Tasks (run here in main loop):
  // ============================================================

  #if USE_MQTT
    mqttUpdate();
  #endif

  #if USE_OTA
    // Skip game logic during OTA update
    if (otaIsUpdating()) {
      return;
    }
  #endif

  // Check for reset request (from button or MQTT)
  if (propResetRequested()) {
    propClearResetRequest();
    onPropReset();
  }

  // ============================================================
  // Button controller - reads buttons, sends commands, manages LED
  // ============================================================
  buttonControllerUpdate();
}
