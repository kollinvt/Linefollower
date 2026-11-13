/*
 * Proof of concept: H-brug DRV8833 + 2x N20 (ESP32)
 * Toont aan dat beide motoren ONAFHANKELIJK, traploos en in beide richtingen
 * geregeld kunnen worden. Zelfde pinnen en pwm-instellingen als de finale code.
 *
 * Seriële monitor 115200 baud, "Newline":
 *   l <-255..255>  linker motor     r <-255..255>  rechter motor
 *   t              automatische test                s  stop
 */
#define AIN1 26   // motor A = links
#define AIN2 27
#define BIN1 25   // motor B = rechts
#define BIN2 33
#define PWM_FREQ 20000  // 20 kHz: buiten het hoorbare bereik
#define PWM_BITS 8      // 0..255

const uint8_t PINS[4] = {AIN1, AIN2, BIN1, BIN2};

void pwm(uint8_t idx, uint32_t duty)
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PINS[idx], duty);
#else
  ledcWrite(idx, duty);
#endif
}

// fast decay: pwm op de ene ingang, andere ingang laag
void setMotor(uint8_t m, int power)
{
  power = constrain(power, -255, 255);
  uint8_t a = m * 2, b = m * 2 + 1;
  if (power >= 0) { pwm(b, 0); pwm(a, power); }
  else            { pwm(a, 0); pwm(b, -power); }
}

void ramp(uint8_t m, const char *name)
{
  Serial.printf("%s vooruit\n", name);
  for (int p = 0; p <= 255; p += 5) { setMotor(m, p); delay(20); }
  for (int p = 255; p >= 0; p -= 5) { setMotor(m, p); delay(20); }
  Serial.printf("%s achteruit\n", name);
  for (int p = 0; p >= -255; p -= 5) { setMotor(m, p); delay(20); }
  for (int p = -255; p <= 0; p += 5) { setMotor(m, p); delay(20); }
}

void setup()
{
  Serial.begin(115200);
  for (uint8_t i = 0; i < 4; i++)
  {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(PINS[i], PWM_FREQ, PWM_BITS);
#else
    ledcSetup(i, PWM_FREQ, PWM_BITS);
    ledcAttachPin(PINS[i], i);
#endif
  }
  setMotor(0, 0);
  setMotor(1, 0);
  Serial.println("hbridge ready: l <pwm>, r <pwm>, t, s");
}

void loop()
{
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'l')      { int p = Serial.parseInt(); setMotor(0, p); Serial.printf("links %d\n", p); }
  else if (c == 'r') { int p = Serial.parseInt(); setMotor(1, p); Serial.printf("rechts %d\n", p); }
  else if (c == 't') { ramp(0, "links"); ramp(1, "rechts"); Serial.println("test klaar"); }
  else if (c == 's') { setMotor(0, 0); setMotor(1, 0); Serial.println("stop"); }
}
