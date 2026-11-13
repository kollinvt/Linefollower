/*
 * Proof of concept: non-blocking start/stop via een hardware interrupt
 * Drukknop tussen D7 (INT6) en GND, interne pull-up.
 * Iedere druk wisselt de toestand run / stop, ONGEACHT wat de loop doet.
 * Om dat aan te tonen voert de loop expres een trage (blokkerende) taak uit:
 * de led D13 reageert toch onmiddellijk.
 */
const byte PIN_BUTTON = 7;
const byte PIN_LED = 13;

volatile bool run = false;
volatile unsigned long lastPress = 0;

void onButton()
{
  unsigned long now = millis();
  if (now - lastPress < 250) return; // software debounce
  lastPress = now;
  run = !run;
  digitalWrite(PIN_LED, run);
}

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);
}

void loop()
{
  Serial.println(run ? "run" : "stop");
  delay(2000); // "trage" taak: de knop werkt toch meteen dankzij de interrupt
}
