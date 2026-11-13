#include "sensor.h"
#include "config.h"
#include "params.h"
#include <Preferences.h>

// Frame van de module: $A,x1:4095,x2:4095,x3:4095,x4:4095,x5:4095,x6:4095,x7:4095,x8:4095#
static HardwareSerial &S = Serial2;

static char buf[128];
static uint8_t len = 0;
static bool inFrame = false;

volatile uint16_t sensorRaw[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static volatile uint32_t lastFrame = 0;
static uint32_t lastCmd = 0;

// ---------------- Kalibratie ----------------
static volatile bool calRunning = false;
static volatile bool calValid = false;
static uint16_t calMin[8], calMax[8];
static float calScale[8];          // 1 / (max - min), vooraf berekend voor snelheid
static Preferences prefs;

static void calcScale() {
  for (uint8_t i = 0; i < 8; i++) {
    float r = (float)calMax[i] - (float)calMin[i];
    if (r < 100.0f) r = 100.0f;
    calScale[i] = 1.0f / r;
  }
}

void calibLoad() {
  prefs.begin("lfcal3", false);
  for (uint8_t i = 0; i < 8; i++) {
    char a[6], b[6];
    snprintf(a, sizeof(a), "n%u", i);
    snprintf(b, sizeof(b), "x%u", i);
    calMin[i] = prefs.getUShort(a, 0);
    calMax[i] = prefs.getUShort(b, 4095);
  }
  calValid = prefs.getBool("ok", false);
  prefs.end();
  calcScale();
}

void calibStart() {
  for (uint8_t i = 0; i < 8; i++) {
    calMin[i] = 4095;
    calMax[i] = 0;
  }
  calRunning = true;
}

void calibStop() {
  if (!calRunning) return;
  calRunning = false;
  bool all = true;
  for (uint8_t i = 0; i < 8; i++) {
    if (calMax[i] <= calMin[i] || (calMax[i] - calMin[i]) < CAL_MIN_RANGE) {
      calMin[i] = 0;
      calMax[i] = 4095;
      all = false;
    }
  }
  calValid = all;
  calcScale();
  prefs.begin("lfcal3", false);
  for (uint8_t i = 0; i < 8; i++) {
    char a[6], b[6];
    snprintf(a, sizeof(a), "n%u", i);
    snprintf(b, sizeof(b), "x%u", i);
    prefs.putUShort(a, calMin[i]);
    prefs.putUShort(b, calMax[i]);
  }
  prefs.putBool("ok", calValid);
  prefs.end();
}

bool calibRunning() { return calRunning; }
bool calibOk() { return calValid; }

// ---------------- UART ----------------
static void sendCmd() {
  S.print("$0,1,0#");   // geen kalibratie in de module, wel analoog, geen digitaal
  lastCmd = millis();
}

void sensorBegin() {
  S.setRxBufferSize(512);
  S.begin(SENSOR_BAUD, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
  delay(50);
  sendCmd();
}

static bool parseFrame() {
  if (len < 2 || buf[0] != 'A') return false;
  uint16_t v[8];
  uint8_t seen = 0;
  for (uint8_t i = 0; i + 3 < len; i++) {
    if (buf[i] == 'x' && buf[i + 1] >= '1' && buf[i + 1] <= '8' && buf[i + 2] == ':') {
      uint8_t ch = buf[i + 1] - '1';
      uint8_t j = i + 3;
      uint32_t val = 0;
      while (j < len && buf[j] >= '0' && buf[j] <= '9') {
        val = val * 10 + (uint32_t)(buf[j] - '0');
        if (val > 100000UL) break;
        j++;
      }
      if (j == i + 3) return false;
      if (val > 4095) val = 4095;
      v[ch] = (uint16_t)val;
      seen |= (1 << ch);
      i = j - 1;
    }
  }
  if (seen != 0xFF) return false;
  for (uint8_t k = 0; k < 8; k++) sensorRaw[k] = v[k];
  if (calRunning) {
    for (uint8_t k = 0; k < 8; k++) {
      if (v[k] < calMin[k]) calMin[k] = v[k];
      if (v[k] > calMax[k]) calMax[k] = v[k];
    }
  }
  return true;
}

bool sensorPoll() {
  bool got = false;
  while (S.available()) {
    char c = (char)S.read();
    if (c == '$') {
      inFrame = true;
      len = 0;
    } else if (c == '#') {
      if (inFrame && parseFrame()) {
        lastFrame = millis();
        got = true;
      }
      inFrame = false;
    } else if (inFrame) {
      if (len < sizeof(buf) - 1) buf[len++] = c;
      else inFrame = false;
    }
  }
  // De module zendt pas na een commando: opnieuw sturen zolang er niets binnenkomt
  uint32_t now = millis();
  if (now - lastFrame > 200 && now - lastCmd > 500) sendCmd();
  return got;
}

uint32_t sensorLastFrameMs() { return lastFrame; }

void sensorStrength(float s[8]) {
  bool high = P.lineHigh >= 0.5f;
  bool flip = P.flip >= 0.5f;
  for (uint8_t i = 0; i < 8; i++) {
    float n = ((float)sensorRaw[i] - (float)calMin[i]) * calScale[i];
    if (n < 0.0f) n = 0.0f;
    if (n > 1.0f) n = 1.0f;
    s[flip ? 7 - i : i] = high ? n : 1.0f - n;
  }
}
