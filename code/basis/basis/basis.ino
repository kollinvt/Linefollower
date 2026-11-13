/*
 * Basis programma - LineFollower (ESP32)
 * ---------------------------------------------------------------------------
 * Kapstok waarop de finale code gebouwd is. Hier werken samen:
 *  - draadloze communicatie: WiFi access point "LineFollower-basis" (12345678),
 *    webpagina op http://192.168.4.1 + websocket (poort 81)
 *  - dezelfde commando's ook via USB (seriële monitor 115200, newline):
 *      start | stop | debug | set cycle <µs> | set debug on|off
 *  - parameters bewaard in flash (Preferences = EEPROM-emulatie van de ESP32)
 *    => na een reset / spanningsloos start de robot met de laatste waardes
 *  - vaste cyclus (zoals een plc) + meting van de rekentijd
 *  - start/stop via de BOOT-knop (GPIO0) met een hardware interrupt
 *  - demo in de cyclus: led GPIO2 knippert aan 1 Hz zolang de robot "rijdt"
 *
 * Bibliotheek: "WebSockets" van Markus Sattler.
 */
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Preferences.h>

#define BUTTON_PIN 0
#define LED_PIN 2

WebServer server(80);
WebSocketsServer ws(81);
Preferences prefs;

struct param_t
{
  uint32_t cycleTime; // µs
  bool debug;
} params;

volatile bool run = false;
volatile uint32_t lastPress = 0;
uint32_t previous, calculationTime, cycleCount, lastDebug;

static const char PAGE[] PROGMEM =
    "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>basis</title></head><body style='font-family:sans-serif'><h3>LineFollower basis</h3>"
    "<button onclick=\"s('start')\">start</button> <button onclick=\"s('stop')\">stop</button> "
    "<button onclick=\"s('debug')\">debug</button>"
    "<p><input id='t' placeholder='set cycle 2000'><button onclick='s(t.value)'>verstuur</button></p><pre id='log'></pre>"
    "<script>const w=new WebSocket('ws://'+location.hostname+':81/');"
    "w.onmessage=e=>{log.textContent=e.data+'\\n'+log.textContent.slice(0,3000)};"
    "function s(m){w.send(m)}</script></body></html>";

/* ---------------- parameters in flash ---------------- */
void paramsLoad()
{
  prefs.begin("basis", true);
  params.cycleTime = prefs.getUInt("cycle", 1000);
  params.debug = prefs.getBool("debug", false);
  prefs.end();
}

void paramsSave()
{
  prefs.begin("basis", false);
  prefs.putUInt("cycle", params.cycleTime);
  prefs.putBool("debug", params.debug);
  prefs.end();
}

/* ---------------- antwoorden naar USB + alle websocket clients ---------------- */
void reply(const char *fmt, ...)
{
  char b[160];
  va_list a;
  va_start(a, fmt);
  vsnprintf(b, sizeof(b), fmt, a);
  va_end(a);
  Serial.println(b);
  ws.broadcastTXT(b);
}

/* ---------------- commando's ---------------- */
void handleCommand(char *cmd)
{
  char *word = strtok(cmd, " ");
  if (!word) return;

  if (strcmp(word, "start") == 0) { run = true; reply("started"); }
  else if (strcmp(word, "stop") == 0) { run = false; reply("stopped"); }
  else if (strcmp(word, "debug") == 0)
  {
    reply("state: %s", run ? "running" : "stopped");
    reply("cycle time: %u", params.cycleTime);
    reply("debug: %s", params.debug ? "on" : "off");
    reply("calculation time: %u", calculationTime);
    calculationTime = 0;
  }
  else if (strcmp(word, "set") == 0)
  {
    char *param = strtok(NULL, " ");
    char *value = strtok(NULL, " ");
    if (!param || !value) { reply("gebruik: set <param> <waarde>"); return; }
    if (run) { reply("stop de robot eerst (flash schrijven tijdens het rijden = haperingen)"); return; }
    if (strcmp(param, "cycle") == 0) params.cycleTime = max(100L, atol(value));
    else if (strcmp(param, "debug") == 0) params.debug = strcmp(value, "on") == 0;
    else { reply("unknown parameter: %s", param); return; }
    paramsSave();
    reply("%s = %s", param, value);
  }
  else reply("unknown command: \"%s\"", word);
}

void wsEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type != WStype_TEXT) return;
  char msg[64];
  size_t n = length < sizeof(msg) - 1 ? length : sizeof(msg) - 1;
  memcpy(msg, payload, n);
  msg[n] = '\0';
  handleCommand(msg);
}

void readUsb() // non-blocking
{
  static char line[64];
  static uint8_t pos = 0;
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { line[pos] = '\0'; if (pos) handleCommand(line); pos = 0; }
    else if (pos < sizeof(line) - 1) line[pos++] = c;
  }
}

/* ---------------- start/stop interrupt ---------------- */
void IRAM_ATTR onButton()
{
  uint32_t now = millis();
  if (now - lastPress < 250) return;
  lastPress = now;
  run = !run;
}

void setup()
{
  Serial.begin(115200);
  paramsLoad();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("LineFollower-basis", "12345678");
  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.begin();
  ws.begin();
  ws.onEvent(wsEvent);

  Serial.println("ready");
}

void loop()
{
  server.handleClient();
  ws.loop();
  readUsb();

  uint32_t current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    /* cyclische code: led knippert aan 1 Hz, onafhankelijk van de cyclustijd */
    if (run)
    {
      if (++cycleCount >= 500000UL / params.cycleTime)
      {
        cycleCount = 0;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
    }
    else digitalWrite(LED_PIN, LOW);

    /* sensor uitlezen, positie, pid, motoren (zie finale code) */

    uint32_t difference = micros() - current;
    if (difference > calculationTime) calculationTime = difference;
  }

  /* debug info buiten het cyclische deel, 1x per seconde */
  if (params.debug && millis() - lastDebug >= 1000)
  {
    lastDebug = millis();
    reply("running: %d  calculation time: %u us", (int)run, calculationTime);
  }
}
