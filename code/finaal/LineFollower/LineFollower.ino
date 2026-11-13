// =====================================================================
// Line follower - ESP32 + DRV8833 + 2x N20 + Yahboom 8-kanaals sensor (analoog, UART)
//
// Core 1: control.cpp  -> sensor lezen, positie, PID, motoren (elk sensorframe)
// Core 0: web.cpp      -> WiFi access point, webpagina en websocket
//
// Nodig: bibliotheek "WebSockets" van Markus Sattler (Bibliotheekbeheer)
//
// Verbind met WiFi "LineFollower" (wachtwoord 12345678) en open http://192.168.4.1
// Spatiebalk op de laptop = STOP.
// Start/stop ook met de knop op de robot (BOOT-knop, GPIO0) via een interrupt.
// =====================================================================

#include "config.h"
#include "params.h"
#include "sensor.h"
#include "motors.h"
#include "control.h"
#include "web.h"

// ---------------- Start/stop knop (hardware interrupt) ----------------
// De ISR zet enkel een vlag; loop() voert de start/stop uit. Zo roepen we
// vanuit de interrupt geen code in flash aan (veilig tijdens flash-schrijven).
static volatile bool buttonPressed = false;
static volatile uint32_t lastPress = 0;

static void IRAM_ATTR onButton() {
  uint32_t now = millis();
  if (now - lastPress < BUTTON_DEBOUNCE_MS) return;   // contactdender
  lastPress = now;
  buttonPressed = true;
}

void setup() {
  Serial.begin(115200);
  motorsBegin();     // motoren meteen uit
  paramsLoad();
  calibLoad();

  pinMode(RUN_LED_PIN, OUTPUT);
  digitalWrite(RUN_LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);

  webBegin();        // core 0
  controlStart();    // core 1
}

void loop() {
  // sensor, PID en motoren draaien in hun eigen taak op core 1, de webserver op core 0
  if (buttonPressed) {
    buttonPressed = false;
    if (!calibRunning()) controlRun(!controlIsRunning());
  }
  digitalWrite(RUN_LED_PIN, controlIsRunning());
  vTaskDelay(10);
}
