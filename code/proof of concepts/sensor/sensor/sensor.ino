/*
 * Proof of concept: Yahboom 8-kanaals lijnsensor via UART (ESP32)
 * Leest de 8 sensoren individueel uit (ruwe analoge waardes 0..4095, geen
 * kalibratie / normalisatie / interpolatie) en print ze 10x per seconde.
 * Open de Serial Plotter (115200) om alle 8 de lijnen te zien.
 *
 * Bedrading: sensor TX -> GPIO16 (RX2), sensor RX -> GPIO17 (TX2), 5V, GND
 * Commando naar de module: "$0,1,0#" = geen interne kalibratie, analoge
 * uitvoer aan, digitale uitvoer uit. Frames: $A,x1:1234,...,x8:1234#
 */
#define SENSOR_RX 16
#define SENSOR_TX 17

HardwareSerial &S = Serial2;
uint16_t raw[8];
char buf[128];
uint8_t len = 0;
bool inFrame = false;
uint32_t lastFrame = 0, lastCmd = 0, lastPrint = 0;

bool parseFrame()
{
  buf[len] = '\0';
  if (len < 2 || buf[0] != 'A') return false;
  uint8_t seen = 0;
  uint16_t v[8];
  for (uint8_t i = 0; i + 3 < len; i++)
  {
    if (buf[i] == 'x' && buf[i + 1] >= '1' && buf[i + 1] <= '8' && buf[i + 2] == ':')
    {
      uint8_t ch = buf[i + 1] - '1';
      v[ch] = constrain(atoi(&buf[i + 3]), 0, 4095);
      seen |= 1 << ch;
    }
  }
  if (seen != 0xFF) return false;
  memcpy(raw, v, sizeof(raw));
  return true;
}

void sendCmd()
{
  S.print("$0,1,0#"); // module zendt pas data na een commando
  lastCmd = millis();
}

void setup()
{
  Serial.begin(115200);
  S.begin(115200, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
  delay(50);
  sendCmd();
}

void loop()
{
  while (S.available())
  {
    char c = S.read();
    if (c == '$') { inFrame = true; len = 0; }
    else if (c == '#') { if (inFrame && parseFrame()) lastFrame = millis(); inFrame = false; }
    else if (inFrame && len < sizeof(buf) - 1) buf[len++] = c;
  }

  uint32_t now = millis();
  if (now - lastFrame > 200 && now - lastCmd > 500) sendCmd();

  if (now - lastPrint >= 100)
  {
    lastPrint = now;
    if (now - lastFrame > 200)
    {
      Serial.println("geen data van de sensor (TX/RX gekruist? 5V en GND aangesloten?)");
      return;
    }
    for (uint8_t i = 0; i < 8; i++) Serial.printf("S%u:%u%s", i + 1, raw[i], i < 7 ? "\t" : "\n");
  }
}
