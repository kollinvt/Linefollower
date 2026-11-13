/*
 * Proof of concept: sensor array (QTR-8A, 6 middelste sensoren)
 * Leest de 6 sensoren individueel uit (geen kalibratie, normalisatie of
 * interpolatie) en stuurt de ruwe ADC waardes 10x per seconde door.
 * Open de Serial Plotter (115200 baud) om alle 6 de lijnen te zien.
 *
 * Controleer:
 *  - wit  => lage waarde (+- 30..150), zwart => hoge waarde (+- 800..980)
 *  - geen enkele sensor geeft 0 of 1023 (= buiten het bereik van de ADC)
 *  - sensor S1 (links) hoort bij A0, S6 (rechts) bij A5
 */
const int SENSOR[] = {A0, A1, A2, A3, A4, A5};
const byte PIN_LEDON = 4;

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LEDON, OUTPUT);
  digitalWrite(PIN_LEDON, HIGH); // IR leds aan
}

void loop()
{
  for (byte i = 0; i < 6; i++)
  {
    Serial.print("S"); Serial.print(i + 1); Serial.print(":");
    Serial.print(analogRead(SENSOR[i]));
    Serial.print(i < 5 ? "\t" : "\n");
  }
  delay(100);
}
