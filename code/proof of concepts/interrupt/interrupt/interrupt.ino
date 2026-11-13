/*
 * Proof of concept: non-blocking start/stop via een hardware interrupt (ESP32)
 * Knop: BOOT-knop van het DevKit bord = GPIO0 naar GND (of een externe
 * drukknop tussen een vrije GPIO en GND). Interne pull-up.
 * De led op GPIO2 wisselt meteen bij een druk, ook al blokkeert de loop
 * expres 2 s met delay().
 */
#define BUTTON_PIN 0
#define LED_PIN 2

volatile bool run = false;
volatile bool changed = false;
volatile uint32_t lastPress = 0;

void IRAM_ATTR onButton()
{
  uint32_t now = millis();
  if (now - lastPress < 250) return; // software debounce
  lastPress = now;
  run = !run;
  changed = true;
  digitalWrite(LED_PIN, run);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButton, FALLING);
}

void loop()
{
  Serial.println(run ? "run" : "stop");
  delay(2000); // "trage" taak: de knop werkt toch meteen dankzij de interrupt
}
