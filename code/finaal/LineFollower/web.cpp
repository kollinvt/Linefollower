#include "web.h"
#include "config.h"
#include "params.h"
#include "control.h"
#include "sensor.h"
#include "motors.h"
#include "page.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>   // bibliotheek "WebSockets" van Markus Sattler (links2004)
#include <stdlib.h>
#include <string.h>

static WebServer server(80);
static WebSocketsServer ws(WS_PORT);
static char jbuf[4096];

// ---------------- Berichten naar de pagina (JSON) ----------------

static void fmtNum(char *dst, size_t n, float v) {
  snprintf(dst, n, "%g", (double)v);
}

// {"t":"P","p":[[key,label,groep,waarde,min,max,stap,bool],...]}
static void sendParams(int8_t client) {
  int n = snprintf(jbuf, sizeof(jbuf), "{\"t\":\"P\",\"p\":[");
  for (int i = 0; i < PARAM_COUNT && n < (int)sizeof(jbuf) - 200; i++) {
    const ParamDef &d = PARAMS[i];
    char v[20], mn[20], mx[20], st[20];
    fmtNum(v, sizeof(v), *d.ptr);
    fmtNum(mn, sizeof(mn), d.mn);
    fmtNum(mx, sizeof(mx), d.mx);
    fmtNum(st, sizeof(st), d.step);
    n += snprintf(jbuf + n, sizeof(jbuf) - n, "%s[\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%d]",
                  i ? "," : "", d.key, d.label, d.group, v, mn, mx, st, paramIsBool(i) ? 1 : 0);
  }
  n += snprintf(jbuf + n, sizeof(jbuf) - n, "]}");
  if (client < 0) ws.broadcastTXT(jbuf, n);
  else ws.sendTXT((uint8_t)client, jbuf, n);
}

static void sendValue(int idx) {
  char v[20], b[80];
  fmtNum(v, sizeof(v), *PARAMS[idx].ptr);
  int n = snprintf(b, sizeof(b), "{\"t\":\"V\",\"k\":\"%s\",\"v\":%s}", PARAMS[idx].key, v);
  ws.broadcastTXT(b, n);
}

static void sendMsg(uint8_t client, const char *msg) {
  char b[200];
  int n = snprintf(b, sizeof(b), "{\"t\":\"M\",\"m\":\"%s\"}", msg);
  ws.sendTXT(client, b, n);
}

static void sendTelemetry() {
  char b[400];
  int n = snprintf(b, sizeof(b),
      "{\"t\":\"T\",\"run\":%d,\"ok\":%d,\"cal\":%d,\"calok\":%d,\"fps\":%u,\"us\":%u,"
      "\"e\":%.2f,\"b\":%.0f,\"o\":%.0f,\"l\":%d,\"r\":%d,\"lost\":%d,\"x\":%d,\"s\":[",
      (int)controlIsRunning(), (int)T.sensorOk, (int)calibRunning(), (int)calibOk(),
      (unsigned)T.fps, (unsigned)T.loopUs, (double)T.error, (double)T.base, (double)T.out,
      (int)motorPwmL, (int)motorPwmR, (int)T.lost, (int)T.cross);
  for (int i = 0; i < 8; i++) n += snprintf(b + n, sizeof(b) - n, "%s%u", i ? "," : "", (unsigned)T.str[i]);
  n += snprintf(b + n, sizeof(b) - n, "]}");
  ws.broadcastTXT(b, n);
}

// ---------------- Berichten van de pagina ----------------
// "run" | "stop" | "cal1" | "cal0" | "save" | "def" | "set <key> <waarde>"

static void handleText(uint8_t client, char *msg) {
  if (strcmp(msg, "stop") == 0) {
    controlRun(false);
  } else if (strcmp(msg, "run") == 0) {
    if (calibRunning()) sendMsg(client, "Stop eerst de kalibratie.");
    else controlRun(true);
  } else if (strcmp(msg, "cal1") == 0) {
    if (controlIsRunning()) sendMsg(client, "Stop de robot eerst.");
    else calibStart();
  } else if (strcmp(msg, "cal0") == 0) {
    if (calibRunning()) {
      calibStop();
      sendMsg(client, calibOk() ? "Kalibratie opgeslagen."
                                : "Kalibratie onvolledig: beweeg alle sensoren over lijn en wit vlak en probeer opnieuw.");
    }
  } else if (strcmp(msg, "save") == 0) {
    // Flash schrijven pauzeert kort beide cores: nooit tijdens het rijden.
    if (controlIsRunning()) sendMsg(client, "Stop de robot eerst, opslaan kan alleen als hij stilstaat.");
    else { paramsSave(); sendMsg(client, "Opgeslagen in de robot."); }
  } else if (strcmp(msg, "def") == 0) {
    paramsDefaults();
    sendParams(-1);
    sendMsg(client, "Standaardwaarden geladen (nog niet opgeslagen).");
  } else if (strncmp(msg, "set ", 4) == 0) {
    char *key = msg + 4;
    char *sp = strchr(key, ' ');
    if (!sp) return;
    *sp = 0;
    int idx = paramFind(key);
    if (idx < 0) return;
    paramSet(idx, strtof(sp + 1, NULL));
    sendValue(idx);
  }
}

static void wsEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_CONNECTED) {
    sendParams((int8_t)num);
  } else if (type == WStype_TEXT) {
    char msg[64];
    size_t n = length < sizeof(msg) - 1 ? length : sizeof(msg) - 1;
    memcpy(msg, payload, n);
    msg[n] = 0;
    handleText(num, msg);
  }
}

// ---------------- Taak op core 0 ----------------

static void webTask(void *) {
  uint32_t lastT = 0;
  for (;;) {
    server.handleClient();
    ws.loop();
    uint32_t now = millis();
    if (now - lastT >= TELEMETRY_MS) {
      lastT = now;
      if (ws.connectedClients() > 0) sendTelemetry();
    }
    vTaskDelay(1);
  }
}

void webBegin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  WiFi.setSleep(false);

  server.on("/", []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html", PAGE);
  });
  server.begin();

  ws.begin();
  ws.onEvent(wsEvent);

  xTaskCreatePinnedToCore(webTask, "web", 8192, NULL, 1, NULL, WEB_CORE);
}
