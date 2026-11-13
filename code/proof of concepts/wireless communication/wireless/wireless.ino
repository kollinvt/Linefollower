/*
 * Proof of concept: duplex draadloze communicatie via WiFi (ESP32)
 * De ESP32 maakt een eigen access point "LineFollower-POC" (wachtwoord 12345678).
 * Surf met laptop / smartphone naar http://192.168.4.1
 *  - de ESP32 stuurt elke seconde een teller naar de pagina   (ESP32 -> pc)
 *  - knoppen "led aan / led uit" schakelen GPIO2              (pc -> ESP32)
 *  - elk bericht wordt bevestigd met een echo                 (ESP32 -> pc)
 * Communicatie via een websocket (poort 81), volledig non-blocking.
 *
 * Bibliotheek: "WebSockets" van Markus Sattler (Bibliotheekbeheer).
 */
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#define LED_PIN 2

WebServer server(80);
WebSocketsServer ws(81);
uint32_t lastTick = 0, counter = 0;

static const char PAGE[] PROGMEM =
    "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>POC wireless</title></head><body style='font-family:sans-serif'>"
    "<h3>LineFollower - POC draadloze communicatie</h3>"
    "<button onclick=\"s('led on')\">led aan</button> <button onclick=\"s('led off')\">led uit</button>"
    "<p><input id='t'><button onclick='s(t.value)'>verstuur</button></p><pre id='log'></pre>"
    "<script>const w=new WebSocket('ws://'+location.hostname+':81/');"
    "w.onmessage=e=>{log.textContent=e.data+'\\n'+log.textContent.slice(0,2000)};"
    "function s(m){w.send(m)}</script></body></html>";

void wsEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type != WStype_TEXT) return;
  char msg[64];
  size_t n = length < sizeof(msg) - 1 ? length : sizeof(msg) - 1;
  memcpy(msg, payload, n);
  msg[n] = '\0';

  if (strcmp(msg, "led on") == 0) digitalWrite(LED_PIN, HIGH);
  else if (strcmp(msg, "led off") == 0) digitalWrite(LED_PIN, LOW);

  char reply[80];
  snprintf(reply, sizeof(reply), "echo: %s", msg);
  ws.sendTXT(num, reply);
  Serial.printf("ontvangen: %s\n", msg);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("LineFollower-POC", "12345678");
  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.begin();
  ws.begin();
  ws.onEvent(wsEvent);
  Serial.print("AP klaar, surf naar http://");
  Serial.println(WiFi.softAPIP());
}

void loop()
{
  server.handleClient();
  ws.loop();
  if (millis() - lastTick >= 1000)
  {
    lastTick = millis();
    char m[24];
    snprintf(m, sizeof(m), "alive %lu", (unsigned long)counter++);
    ws.broadcastTXT(m);
  }
}
