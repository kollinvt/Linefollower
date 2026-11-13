/*
 * Proof of concept: H-brug (DRV8833) + 2 motoren
 * Toont aan dat beide motoren ONAFHANKELIJK van elkaar traploos in snelheid
 * en in beide richtingen geregeld kunnen worden.
 *
 * Bediening via de seriële monitor (115200 baud, newline):
 *   l <-255..255>   snelheid linker motor
 *   r <-255..255>   snelheid rechter motor
 *   t               automatische test (ramp vooruit/achteruit per motor)
 *   s               stop
 */
const byte PIN_AIN1 = 5;   // linker motor
const byte PIN_AIN2 = 6;
const byte PIN_BIN1 = 9;   // rechter motor
const byte PIN_BIN2 = 10;

void setMotor(byte in1, byte in2, int power)
{
  power = constrain(power, -255, 255);
  if (power >= 0) { digitalWrite(in2, LOW); analogWrite(in1, power); }
  else            { digitalWrite(in1, LOW); analogWrite(in2, -power); }
}

void ramp(byte in1, byte in2, const char *name)
{
  Serial.print(name); Serial.println(" vooruit 0 -> 255 -> 0");
  for (int p = 0; p <= 255; p += 5) { setMotor(in1, in2, p); delay(20); }
  for (int p = 255; p >= 0; p -= 5) { setMotor(in1, in2, p); delay(20); }
  Serial.print(name); Serial.println(" achteruit 0 -> -255 -> 0");
  for (int p = 0; p >= -255; p -= 5) { setMotor(in1, in2, p); delay(20); }
  for (int p = -255; p <= 0; p += 5) { setMotor(in1, in2, p); delay(20); }
}

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT);
  setMotor(PIN_AIN1, PIN_AIN2, 0);
  setMotor(PIN_BIN1, PIN_BIN2, 0);
}

void loop()
{
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'l')      { int p = Serial.parseInt(); setMotor(PIN_AIN1, PIN_AIN2, p); Serial.print("links "); Serial.println(p); }
  else if (c == 'r') { int p = Serial.parseInt(); setMotor(PIN_BIN1, PIN_BIN2, p); Serial.print("rechts "); Serial.println(p); }
  else if (c == 't') { ramp(PIN_AIN1, PIN_AIN2, "links"); ramp(PIN_BIN1, PIN_BIN2, "rechts"); Serial.println("test klaar"); }
  else if (c == 's') { setMotor(PIN_AIN1, PIN_AIN2, 0); setMotor(PIN_BIN1, PIN_BIN2, 0); Serial.println("stop"); }
}
