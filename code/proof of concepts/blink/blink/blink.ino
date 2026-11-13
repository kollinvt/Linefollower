/*
 * Proof of concept: blink (ESP32 DevKit)
 * Toont aan dat de Arduino IDE, het ESP32 board package, de USB-driver
 * (CP210x / CH340) en het uploaden werken, en dat de klok klopt:
 * de blauwe led (GPIO2) knippert precies 60x per minuut.
 */
const int PIN_LED = 2;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.printf("CPU %u MHz\n", getCpuFrequencyMhz());
}

void loop()
{
  digitalWrite(PIN_LED, HIGH);
  delay(500);
  digitalWrite(PIN_LED, LOW);
  delay(500);
}
