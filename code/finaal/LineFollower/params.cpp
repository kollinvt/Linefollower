#include "params.h"
#include <Preferences.h>
#include <string.h>

Params P;

// key, label, groep, pointer, standaard, min, max, stap
const ParamDef PARAMS[] = {
  // ---- PID (fout loopt van -7 tot +7, zelfde schaal als de vorige versies) ----
  {"kp",       "Kp",                        "PID",       &P.kp,         67.0f,  0,    2000, 1},
  {"ki",       "Ki",                        "PID",       &P.ki,          0.0f,  0,    50,   0.01f},
  {"kd",       "Kd",                        "PID",       &P.kd,         80.0f,  0,    5000, 1},
  {"dFilter",  "D-filter (1 = geen)",       "PID",       &P.dFilter,     0.6f,  0.05f, 1,   0.05f},
  {"iMax",     "I-max",                     "PID",       &P.iMax,      200.0f,  0,    5000, 10},

  // ---- Snelheid (0..255, wordt geschaald naar de echte PWM) ----
  {"vMax",     "Snelheid rechtdoor",        "Snelheid",  &P.vMax,      120.0f,  0,    255,  1},
  {"vMin",     "Min. snelheid in bocht",    "Snelheid",  &P.vMin,        0.0f,  0,    255,  1},
  {"curve",    "Bochtafremming",            "Snelheid",  &P.curveSlow,  70.0f,  0,    400,  1},
  {"curveSt",  "Afremmen vanaf fout",       "Snelheid",  &P.curveStart,  1.0f,  0,    7,    0.1f},
  {"accel",    "Optrekken per frame",       "Snelheid",  &P.accel,       6.0f,  0.5f, 255,  0.5f},

  // ---- Motoren ----
  {"runPwm",   "Min. PWM rijden",           "Motoren",   &P.runPwm,    160.0f,  0,    254,  1},
  {"kickPwm",  "Startpuls PWM",             "Motoren",   &P.kickPwm,   220.0f,  0,    255,  1},
  {"kickMs",   "Startpuls ms",              "Motoren",   &P.kickMs,     15.0f,  0,    100,  1},
  {"maxRev",   "Max. achteruit (0 = nooit)","Motoren",   &P.maxRev,    255.0f,  0,    255,  1},
  {"trimL",    "Trim links",                "Motoren",   &P.trimL,       1.0f,  0.5f, 1.5f, 0.01f},
  {"trimR",    "Trim rechts",               "Motoren",   &P.trimR,       1.0f,  0.5f, 1.5f, 0.01f},

  // ---- Lijn ----
  {"lostErr",  "Fout als lijn kwijt",       "Lijn",      &P.lostError,   9.0f,  0,    30,   0.5f},
  {"crossN",   "Kruispunt: min. sensoren",  "Lijn",      &P.crossN,      6.0f,  2,    9,    1},
  {"crossMs",  "Kruispunt: rechtdoor ms",   "Lijn",      &P.crossMs,    60.0f,  0,    500,  5},
  {"noise",    "Ruisdrempel sensor",        "Lijn",      &P.noise,       0.15f, 0,    0.9f, 0.01f},
  {"lineHigh", "Lijn geeft hoge waarde",    "Lijn",      &P.lineHigh,    0.0f,  0,    1,    1},
  {"flip",     "Sensor links/rechts om",    "Lijn",      &P.flip,        0.0f,  0,    1,    1},
  {"steerInv", "Stuurrichting omkeren",     "Lijn",      &P.steerInv,    0.0f,  0,    1,    1},
};

const int PARAM_COUNT = sizeof(PARAMS) / sizeof(PARAMS[0]);

static Preferences prefs;
static const char *NS = "lf3";   // nieuwe naamruimte: oude opgeslagen waarden worden genegeerd

void paramsDefaults() {
  for (int i = 0; i < PARAM_COUNT; i++) *PARAMS[i].ptr = PARAMS[i].def;
}

void paramsLoad() {
  paramsDefaults();
  prefs.begin(NS, false);
  for (int i = 0; i < PARAM_COUNT; i++) {
    float v = prefs.getFloat(PARAMS[i].key, PARAMS[i].def);
    paramSet(i, v);
  }
  prefs.end();
}

void paramsSave() {
  prefs.begin(NS, false);
  for (int i = 0; i < PARAM_COUNT; i++) prefs.putFloat(PARAMS[i].key, *PARAMS[i].ptr);
  prefs.end();
}

int paramFind(const char *key) {
  for (int i = 0; i < PARAM_COUNT; i++) {
    if (strcmp(PARAMS[i].key, key) == 0) return i;
  }
  return -1;
}

bool paramIsBool(int idx) {
  const ParamDef &d = PARAMS[idx];
  return d.mn == 0 && d.mx == 1 && d.step == 1;
}

float paramSet(int idx, float v) {
  if (idx < 0 || idx >= PARAM_COUNT) return 0;
  const ParamDef &d = PARAMS[idx];
  if (isnan(v)) v = d.def;
  if (v < d.mn) v = d.mn;
  if (v > d.mx) v = d.mx;
  if (paramIsBool(idx)) v = (v >= 0.5f) ? 1.0f : 0.0f;
  *d.ptr = v;
  return v;
}
