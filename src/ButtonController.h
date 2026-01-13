#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H

#include <Arduino.h>

// Initialize button controller (buttons, LED, ping timer)
void buttonControllerInit();

// Update button controller (call from main loop)
// Checks buttons, handles debouncing, sends commands
void buttonControllerUpdate();

// Called when ACK received from AudioPlayer
void buttonControllerOnAck();

// Check if connected to AudioPlayer
bool buttonControllerIsConnected();

// Get time since last ACK (milliseconds)
unsigned long buttonControllerLastAckAge();

#endif
