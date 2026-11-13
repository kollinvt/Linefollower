/*
 * Proof of concept: duplex draadloze communicatie via HC-05 (bluetooth classic)
 * HC-05 TXD -> D0 (RX1), HC-05 RXD <- D1 (TX1) via spanningsdeler 1k/2k.
 *
 * - alles wat via bluetooth binnenkomt wordt teruggestuurd (echo) en naar
 *   de USB seriële monitor doorgestuurd
 * - alles wat je in de USB seriële monitor typt gaat naar bluetooth
 * - de microcontroller stuurt iedere seconde een teller (non-blocking)
 * - commando "led on" / "led off" via bluetooth schakelt D13
 */
#define BT Serial1

const byte PIN_LED = 13;
unsigned long previous, counter;
char line[32];
byte pos = 0;

void setup()
{
  Serial.begin(115200);
  BT.begin(9600); // standaard baudrate HC-05
  pinMode(PIN_LED, OUTPUT);
  BT.println("hello from linefollower");
}

void loop()
{
  while (BT.available())
  {
    char c = BT.read();
    Serial.write(c);
    if (c == '\r') continue;
    if (c == '\n')
    {
      line[pos] = '\0';
      if (strcmp(line, "led on") == 0) digitalWrite(PIN_LED, HIGH);
      else if (strcmp(line, "led off") == 0) digitalWrite(PIN_LED, LOW);
      BT.print("echo: ");
      BT.println(line);
      pos = 0;
    }
    else if (pos < sizeof(line) - 1) line[pos++] = c;
  }
  while (Serial.available()) BT.write(Serial.read());

  if (millis() - previous >= 1000)
  {
    previous = millis();
    BT.print("alive ");
    BT.println(counter++);
  }
}
