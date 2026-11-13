/*
 * Proof of concept: blink
 * Toont aan dat de programmeeromgeving, drivers en upload werken en dat de
 * kloksnelheid klopt: de led op D13 moet exact 1x per seconde knipperen
 * (60x in 1 minuut - controleer met een stopwatch).
 */
const byte PIN_LED = 13;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
}

void loop()
{
  digitalWrite(PIN_LED, HIGH);
  delay(500);
  digitalWrite(PIN_LED, LOW);
  delay(500);
}
