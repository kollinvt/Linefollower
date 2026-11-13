#pragma once
#include <Arduino.h>

// =====================================================================
// Vaste hardware-instellingen. Alles wat je tijdens het rijden wilt
// aanpassen staat in params.cpp en is instelbaar via de webpagina.
// =====================================================================

// ---------- Motoren (DRV8833 + N20) ----------
// Motor A = LINKS, motor B = RECHTS. Draait een wiel verkeerd om: zet INVERT op true.
#define AIN1 26
#define AIN2 27
#define BIN1 25
#define BIN2 33
#define MOTOR_A_INVERT false
#define MOTOR_B_INVERT false

#define PWM_FREQ 20000   // Hz. Lager (bv. 2000-5000) geeft N20's meer koppel bij lage PWM, maar je hoort het
#define PWM_BITS 8       // 0..255

// false = fast decay: 1 pin PWM, andere pin laag (zoals nu, werkt bij jou)
// true  = slow decay: 1 pin hoog, andere pin geïnverteerde PWM. Vaak lineairder en
//         meer koppel bij lage snelheid. Probeer dit als je "Min. PWM rijden" lager wilt.
#define DECAY_SLOW false

// ---------- Sensor (Yahboom 8-kanaals, UART, analoog) ----------
// ESP32 RX = 16 (naar TX van de sensor), ESP32 TX = 17 (naar RX van de sensor)
#define SENSOR_RX 16
#define SENSOR_TX 17
#define SENSOR_BAUD 115200
#define SENSOR_TIMEOUT_MS 150   // geen frame binnen deze tijd -> motoren uit

#define LOST_WEIGHT 0.25f       // totale lijnsterkte onder dit niveau = lijn kwijt
#define ACTIVE_LEVEL 0.5f       // sensor "ziet de lijn" (voor kruispuntdetectie)
#define CAL_MIN_RANGE 200       // minimaal verschil min/max per sensor voor een geldige kalibratie

// ---------- WiFi access point ----------
#define AP_SSID "LineFollower"
#define AP_PASS "12345678"      // minstens 8 tekens
#define WS_PORT 81
#define TELEMETRY_MS 50         // telemetrie naar de webpagina (20x per seconde)

// ---------- Start/stop knop + run led ----------
// Standaard de BOOT-knop van het ESP32 DevKit bord (GPIO0, naar GND bij indrukken).
// Externe knop? Sluit hem aan tussen een vrije GPIO (bv. 4) en GND en pas BUTTON_PIN aan.
#define BUTTON_PIN 0
#define BUTTON_DEBOUNCE_MS 250
#define RUN_LED_PIN 2           // blauwe led op het DevKit bord: brandt als de robot rijdt

// ---------- Cores ----------
#define CONTROL_CORE 1  // sensor + PID + motoren
#define WEB_CORE 0      // WiFi + webserver + websocket
