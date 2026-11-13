#include "control.h"
#include "config.h"
#include "params.h"
#include "sensor.h"
#include "motors.h"
#include <math.h>

Telemetry T;

static volatile bool runFlag = false;
static volatile bool resetPid = true;

void controlRun(bool run) {
  if (run && !runFlag) resetPid = true;
  runFlag = run;
}

bool controlIsRunning() { return runFlag; }

// Positie van elke sensor in fout-eenheden (links -> rechts)
static const float X[8] = {-7, -5, -3, -1, 1, 3, 5, 7};

// =====================================================================
// Regeltaak op core 1: elk sensorframe -> positie -> PID -> motoren.
// Geen delays in het pad; de PID rekent per frame (Kd werkt op de
// verandering per frame).
// =====================================================================
static void controlTask(void *) {
  sensorBegin();   // hier gestart zodat de UART-interrupt op core 1 draait

  float integ = 0, prevE = 0, dF = 0, lastSide = 1, base = 0;
  uint32_t crossUntil = 0, fpsT = millis();
  uint16_t frames = 0, idle = 0;
  bool stopped = true;
  float s[8];

  for (;;) {
    bool got = sensorPoll();
    uint32_t now = millis();

    if (now - fpsT >= 1000) {
      T.fps = frames;
      frames = 0;
      fpsT = now;
    }

    bool ok = (now - sensorLastFrameMs()) < SENSOR_TIMEOUT_MS;
    T.sensorOk = ok;
    bool active = runFlag && ok && !calibRunning();

    if (!active && !stopped) {
      motorsStop();
      stopped = true;
      base = 0;
      T.cmdL = T.cmdR = 0;
      T.base = 0;
    }

    if (!got) {
      delayMicroseconds(50);
      if (++idle >= 2000) {     // ~elke 100 ms 1 tick ademruimte voor de idle-taak
        idle = 0;
        vTaskDelay(1);
      }
      continue;
    }

    uint32_t t0 = micros();
    frames++;

    // ---------------- Lijnpositie (gewogen gemiddelde, sub-sensor nauwkeurig) ----------------
    sensorStrength(s);
    float noise = P.noise;
    float wsum = 0, wtot = 0;
    uint8_t act = 0, cnt = 0;
    for (uint8_t i = 0; i < 8; i++) {
      T.str[i] = (uint8_t)(s[i] * 100.0f + 0.5f);
      if (s[i] >= ACTIVE_LEVEL) { act |= (1 << i); cnt++; }
      float w = s[i] - noise;
      if (w > 0) { wsum += w * X[i]; wtot += w; }
    }
    bool lost = wtot < LOST_WEIGHT;

    // Kruispunt: veel sensoren EN beide uiterste sensoren zien de lijn -> rechtdoor.
    // Een hoek van 90 graden heeft maar aan 1 kant een arm en telt dus niet.
    bool crossNow = (cnt >= (uint8_t)P.crossN) && ((act & 0x81) == 0x81);
    if (crossNow) crossUntil = now + (uint32_t)P.crossMs;
    bool crossing = crossNow || (int32_t)(crossUntil - now) > 0;

    float e;
    if (crossing) {
      e = 0;
      lost = false;
    } else if (lost) {
      e = lastSide * P.lostError;
    } else {
      e = wsum / wtot;
      if (e >= 1.0f) lastSide = 1;
      else if (e <= -1.0f) lastSide = -1;
    }
    T.error = e;
    T.lost = lost;
    T.cross = crossing;

    if (!active) {
      T.loopUs = (uint16_t)(micros() - t0);
      continue;
    }

    stopped = false;
    if (resetPid) {
      integ = 0;
      prevE = e;
      dF = 0;
      base = 0;           // zacht vertrekken via "Optrekken per frame"
      resetPid = false;
    }

    // ---------------- Snelheidsprofiel ----------------
    // Rechtdoor: vMax. Hoe groter de fout, hoe lager (naar vMin). Afremmen gebeurt
    // meteen, optrekken met maximaal "accel" per frame (geen doorslippende wielen).
    float absE = fabsf(e);
    float over = absE - P.curveStart;
    if (over < 0) over = 0;
    float vMax = P.vMax, vMin = P.vMin;
    if (vMin > vMax) vMin = vMax;
    float target = vMax - P.curveSlow * over;
    if (lost) target = vMin;
    if (target < vMin) target = vMin;
    if (target > base) {
      base += P.accel;
      if (base > target) base = target;
    } else {
      base = target;
    }

    // ---------------- PID ----------------
    if (!lost) {
      integ += e;
      float im = P.iMax;
      if (integ > im) integ = im;
      else if (integ < -im) integ = -im;
    }
    float d = e - prevE;
    prevE = e;
    dF += P.dFilter * (d - dF);
    float out = P.kp * e + P.ki * integ + P.kd * dF;
    if (P.steerInv >= 0.5f) out = -out;

    // ---------------- Naar de wielen ----------------
    float L = base + out;
    float R = base - out;
    // Boven 255: beide wielen zakken samen, zodat het stuurverschil behouden blijft
    if (L > 255.0f) { R -= L - 255.0f; L = 255.0f; }
    if (R > 255.0f) { L -= R - 255.0f; R = 255.0f; }
    float lo = -P.maxRev;
    if (L < lo) L = lo;
    if (R < lo) R = lo;

    L *= P.trimL;
    R *= P.trimR;
    if (L > 255.0f) L = 255.0f;
    if (R > 255.0f) R = 255.0f;

    motorsDrive(L, R);

    T.base = base;
    T.out = out;
    T.cmdL = L;
    T.cmdR = R;
    T.loopUs = (uint16_t)(micros() - t0);
  }
}

void controlStart() {
  xTaskCreatePinnedToCore(controlTask, "control", 4096, NULL, 3, NULL, CONTROL_CORE);
}
