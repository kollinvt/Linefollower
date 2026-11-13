#pragma once
#include <Arduino.h>

struct Telemetry {
  volatile float    error;      // -7..+7 (positief = lijn rechts)
  volatile float    base;       // huidige basissnelheid na bochtafremming/optrekken
  volatile float    out;        // PID-uitvoer
  volatile float    cmdL, cmdR; // commando's naar de motoren (-255..255)
  volatile uint8_t  str[8];     // lijnsterkte per sensor 0..100
  volatile bool     lost;
  volatile bool     cross;
  volatile bool     sensorOk;
  volatile uint16_t fps;        // sensorframes per seconde
  volatile uint16_t loopUs;     // rekentijd per frame (µs)
};

extern Telemetry T;

void controlStart();            // start de regeltaak op CONTROL_CORE
void controlRun(bool run);
bool controlIsRunning();
