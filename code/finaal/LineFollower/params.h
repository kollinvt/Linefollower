#pragma once
#include <Arduino.h>

// Alle instelbare parameters. Een nieuwe parameter toevoegen = 1 veld hier
// + 1 regel in de tabel in params.cpp. De webpagina bouwt zich vanzelf op.
struct Params {
  // PID
  volatile float kp, ki, kd, dFilter, iMax;
  // Snelheid
  volatile float vMax, vMin, curveSlow, curveStart, accel;
  // Motoren
  volatile float runPwm, kickPwm, kickMs, maxRev, trimL, trimR;
  // Lijn
  volatile float lostError, crossN, crossMs, noise, lineHigh, flip, steerInv;
};

extern Params P;

struct ParamDef {
  const char *key;      // korte naam (ook sleutel in flash, max 15 tekens)
  const char *label;    // tekst op de webpagina
  const char *group;    // kaart op de webpagina
  volatile float *ptr;
  float def, mn, mx, step;
};

extern const ParamDef PARAMS[];
extern const int PARAM_COUNT;

void paramsDefaults();
void paramsLoad();
void paramsSave();                    // naar flash: alleen als de robot stilstaat
int paramFind(const char *key);       // -1 als niet gevonden
float paramSet(int idx, float v);     // begrenst en geeft de opgeslagen waarde terug
bool paramIsBool(int idx);
