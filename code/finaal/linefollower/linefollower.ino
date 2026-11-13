/*
 * LineFollower - plan B - finale code
 * ---------------------------------------------------------------------------
 * hardware : Arduino Micro (ATmega32U4) - DRV8833 h-brug - QTR-8A sensor array
 *            (6 middelste sensoren) - HC-05 bluetooth - 2x 18650 Li-ion
 *
 * werking  : 1. sensoren uitlezen en normaliseren (kalibratie zwart/wit)
 *            2. positie van de lijn bepalen met kwadratische interpolatie
 *            3. PID regeling (Ki en Kd herrekend naar de cyclustijd)
 *            4. output verdelen over beide motoren (parameter diff)
 *            alles in een vaste cyclus (params.cycleTime in µs), non-blocking.
 *
 * bediening: drukknop (interrupt) of commando's via USB of bluetooth.
 *            Typ "help" voor de lijst met commando's.
 */
#include "SerialCommand.h"
#include "EEPROMAnything.h"

/* ------------------------------------------------------------------------ */
/* pinout (zie technische tekeningen/elektronisch)                          */
/* ------------------------------------------------------------------------ */
#define BT_PORT Serial1      // HC-05 op D0 (RX1) / D1 (TX1)
#define USB_PORT Serial      // USB (seriële monitor / GUI)
#define BAUDRATE_USB 115200
#define BAUDRATE_BT 9600     // standaard baudrate HC-05

const int SENSOR[] = {A0, A1, A2, A3, A4, A5}; // links -> rechts
const byte SENSOR_COUNT = 6;
const float SENSOR_DISTANCE = 9.525;           // mm tussen 2 sensoren (QTR-8A)

const byte PIN_LEDON = 4;     // QTR-8A LEDON (IR leds aan/uit)
const byte PIN_BUTTON = 7;    // start/stop drukknop (INT6), naar GND
const byte PIN_RUN_LED = 13;  // led brandt wanneer de robot rijdt

const byte PIN_AIN1 = 5;      // DRV8833 motor A = linker motor
const byte PIN_AIN2 = 6;
const byte PIN_BIN1 = 9;      // DRV8833 motor B = rechter motor
const byte PIN_BIN2 = 10;

/* 2x 18650 = max 8,4V. De N20 motoren zijn 6V motoren: we begrenzen de
   pwm duty cycle zodat de gemiddelde motorspanning nooit hoger is dan 6V.
   power 255 in de software komt dus overeen met MAX_PWM op de pin.
   (lichte overvolt van 6V naar 7V geeft +- 15% extra toerental)           */
const int MAX_PWM = 215;      // 215/255 * 8,2V = +- 7V (korte overvolt is ok voor N20)

/* ------------------------------------------------------------------------ */
/* parameters in EEPROM                                                     */
/* ------------------------------------------------------------------------ */
const unsigned int PARAM_VERSION = 0x4C46 + 2; // wijzig bij aanpassing struct

struct param_t
{
  unsigned int version;
  unsigned long cycleTime;  // µs
  int power;                // basis vermogen 0..255
  float diff;               // 0..1 verdeling van de output over de motoren
  float kp;
  float ki;                 // opgeslagen als Ki* = Ki * cycleTime
  float kd;                 // opgeslagen als Kd* = Kd / cycleTime
  int black[SENSOR_COUNT];
  int white[SENSOR_COUNT];
  int lostThreshold;        // genormaliseerd: zwartste sensor > threshold = lijn kwijt
  int boost;                // extra vermogen op rechte stukken 0..255
  float dFilter;            // laagdoorlaatfilter D-term 0 (geen) .. 0.95 (sterk)
} params;

/* ------------------------------------------------------------------------ */
/* globale variabelen                                                       */
/* ------------------------------------------------------------------------ */
SerialCommand sCmdUsb(USB_PORT);
SerialCommand sCmdBt(BT_PORT);

volatile bool run = false;          // robot rijdt
volatile bool runChanged = false;   // gezet door ISR of start/stop commando
volatile unsigned long lastButton = 0;

bool debug = false;
unsigned long previous, calculationTime, lastDebug;

long normalised[SENSOR_COUNT];
float position, lastPosition;
float error, lastErr, iTerm, output, dTerm;
int powerLeft, powerRight;
bool lineLost;

/* lap teller (kruispunten) */
unsigned long crossingTime, lapTime;
bool onCrossing;

/* ------------------------------------------------------------------------ */
void setup()
{
  USB_PORT.begin(BAUDRATE_USB);
  BT_PORT.begin(BAUDRATE_BT);

  pinMode(PIN_LEDON, OUTPUT);
  digitalWrite(PIN_LEDON, HIGH);
  pinMode(PIN_RUN_LED, OUTPUT);
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  motorsOff();

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);

  addCommands(sCmdUsb);
  addCommands(sCmdBt);

  EEPROM_readAnything(0, params);
  if (params.version != PARAM_VERSION) loadDefaults(); // leeg / oud EEPROM

  BT_PORT.println("ready");
  USB_PORT.println("ready");
}

void addCommands(SerialCommand &c)
{
  c.addCommand("set", onSet);
  c.addCommand("debug", onDebug);
  c.addCommand("start", onStart);
  c.addCommand("stop", onStop);
  c.addCommand("calibrate", onCalibrate);
  c.addCommand("sensors", onSensors);
  c.addCommand("reset", onReset);
  c.addCommand("help", onHelp);
  c.setDefaultHandler(onUnknownCommand);
}

void loadDefaults()
{
  params.version = PARAM_VERSION;
  params.cycleTime = 2500;
  params.power = 120;
  params.diff = 0.5;
  params.kp = 8.0;
  params.ki = 0.0;
  params.kd = 0.25 / (params.cycleTime / 1000000.0);
  for (byte i = 0; i < SENSOR_COUNT; i++)
  {
    params.black[i] = 900;
    params.white[i] = 80;
  }
  params.lostThreshold = 700;
  params.boost = 0;
  params.dFilter = 0.5;
  EEPROM_writeAnything(0, params);
}

/* ------------------------------------------------------------------------ */
void loop()
{
  sCmdUsb.readSerial();
  sCmdBt.readSerial();

  if (runChanged)
  {
    runChanged = false;
    iTerm = 0;          // integrator resetten bij iedere start / stop
    lastErr = 0;
    dTerm = 0;
    lapTime = 0;
    crossingTime = 0;
    digitalWrite(PIN_RUN_LED, run);
    if (!run) motorsOff();
  }

  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    readSensors();
    interpolate();

    if (run)
    {
      pid();
      drive();
    }

    unsigned long difference = micros() - current;
    if (difference > calculationTime) calculationTime = difference;
  }

  /* debug info: buiten het cyclische deel, maximaal 4x per seconde */
  if (debug && millis() - lastDebug >= 250)
  {
    lastDebug = millis();
    Stream &out = SerialCommand::current ? SerialCommand::current->port() : (Stream &)USB_PORT;
    out.print("pos ");
    out.print(position, 1);
    out.print(" out ");
    out.print(output, 0);
    out.print(" L ");
    out.print(powerLeft);
    out.print(" R ");
    out.print(powerRight);
    if (lineLost) out.print(" LOST");
    out.println();
  }
}

/* ------------------------------------------------------------------------ */
/* sensor                                                                   */
/* ------------------------------------------------------------------------ */
void readSensors()
{
  for (byte i = 0; i < SENSOR_COUNT; i++)
  {
    long value = analogRead(SENSOR[i]);
    // QTR-8A: zwart = hoge spanning, wit = lage spanning
    // na normalisatie: zwart = 0, wit = 1000
    normalised[i] = map(value, params.black[i], params.white[i], 0, 1000);
    normalised[i] = constrain(normalised[i], 0, 1000);
  }
}

void interpolate()
{
  /* zwartste sensor zoeken + kruispunt detecteren */
  byte index = 0;
  byte blackCount = 0;
  for (byte i = 0; i < SENSOR_COUNT; i++)
  {
    if (normalised[i] < normalised[index]) index = i;
    if (normalised[i] < 300) blackCount++;
  }

  /* lijn kwijt: zwartste sensor is niet zwart genoeg.
     => blijf sturen naar de kant waar we de lijn het laatst zagen */
  lineLost = normalised[index] > params.lostThreshold;
  if (lineLost)
  {
    float edge = (SENSOR_COUNT / 2.0) * SENSOR_DISTANCE;
    position = (lastPosition >= 0) ? edge : -edge;
    return;
  }

  /* kruispunt: (bijna) alle sensoren zwart => rechtdoor rijden */
  if (blackCount >= SENSOR_COUNT - 1)
  {
    if (!onCrossing)
    {
      onCrossing = true;
      unsigned long now = millis();
      if (crossingTime != 0) lapTime = now - crossingTime;
      crossingTime = now;
    }
    position = 0;
    return;
  }
  onCrossing = false;

  /* kwadratische interpolatie door de zwartste sensor en zijn buren */
  if (index == 0) index = 1;
  else if (index == SENSOR_COUNT - 1) index = SENSOR_COUNT - 2;

  long sZero = normalised[index];
  long sMinusOne = normalised[index - 1];
  long sPlusOne = normalised[index + 1];

  float b = ((float)(sPlusOne - sMinusOne)) / 2;
  float a = sPlusOne - b - sZero;

  float x = 0;
  if (a > 0) x = -b / (2 * a);           // dal van de parabool
  x = constrain(x, -1.0, 1.0);

  position = x + index - (SENSOR_COUNT - 1) / 2.0; // 0 = midden van de array
  position *= SENSOR_DISTANCE;                      // in mm
  lastPosition = position;
}

/* ------------------------------------------------------------------------ */
/* PID regelaar                                                             */
/* ------------------------------------------------------------------------ */
void pid()
{
  /* error: positie > 0 = lijn ligt rechts => naar rechts sturen
     => output > 0 => linker motor sneller (zie verdeling hieronder).
     Rijdt jouw robot van de lijn weg? Wissel dan de motordraden van
     beide motoren om, of maak Kp negatief.                          */
  error = position;

  /* proportioneel regelen */
  output = error * params.kp;

  /* integrerend regelen, met anti reset windup */
  iTerm += params.ki * error;
  iTerm = constrain(iTerm, -510, 510);
  output += iTerm;

  /* differentiërend regelen, met 1ste orde laagdoorlaatfilter op de D-term
     (filtert hoogfrequente sensorruis die de D-term anders versterkt)  */
  float dRaw = params.kd * (error - lastErr);
  dTerm = params.dFilter * dTerm + (1 - params.dFilter) * dRaw;
  output += dTerm;
  lastErr = error;

  /* output begrenzen tot wat fysiek mogelijk is */
  output = constrain(output, -510, 510);

  /* snelheid: op een recht stuk (kleine error) extra boost, in een bocht
     valt de boost lineair weg => sneller rondje zonder de bochten te missen */
  float maxPos = (SENSOR_COUNT - 1) / 2.0 * SENSOR_DISTANCE;
  float straight = 1.0 - min(fabs(error) / (maxPos / 2), 1.0);
  int power = constrain(params.power + (int)(params.boost * straight), 0, 255);

  /* output verdelen over de 2 motoren */
  if (output >= 0)
  {
    powerLeft = constrain(power + params.diff * output, -255, 255);
    powerRight = constrain(powerLeft - output, -255, 255);
    powerLeft = powerRight + output;
  }
  else
  {
    powerRight = constrain(power - params.diff * output, -255, 255);
    powerLeft = constrain(powerRight + output, -255, 255);
    powerRight = powerLeft - output;
  }
}

/* ------------------------------------------------------------------------ */
/* motoren (DRV8833: pwm op IN1 of IN2, andere ingang laag = fast decay)    */
/* ------------------------------------------------------------------------ */
void setMotor(byte in1, byte in2, int power)
{
  power = constrain(power, -255, 255);
  int pwm = ((long)abs(power) * MAX_PWM) / 255;
  if (power >= 0)
  {
    digitalWrite(in2, LOW);
    analogWrite(in1, pwm);
  }
  else
  {
    digitalWrite(in1, LOW);
    analogWrite(in2, pwm);
  }
}

void drive()
{
  setMotor(PIN_AIN1, PIN_AIN2, powerLeft);
  setMotor(PIN_BIN1, PIN_BIN2, powerRight);
}

void motorsOff()
{
  powerLeft = powerRight = 0;
  output = 0;
  digitalWrite(PIN_AIN1, LOW);
  digitalWrite(PIN_AIN2, LOW);
  digitalWrite(PIN_BIN1, LOW);
  digitalWrite(PIN_BIN2, LOW);
}

/* ------------------------------------------------------------------------ */
/* start / stop                                                             */
/* ------------------------------------------------------------------------ */
void onButton() // ISR
{
  unsigned long now = millis();
  if (now - lastButton < 250) return; // software debounce
  lastButton = now;
  run = !run;
  runChanged = true;
}

void setRun(bool value)
{
  noInterrupts();
  run = value;
  runChanged = true;
  interrupts();
}

/* ------------------------------------------------------------------------ */
/* SerialCommand eventhandlers                                              */
/* ------------------------------------------------------------------------ */
Stream &out() { return SerialCommand::current->port(); }
char *arg() { return SerialCommand::current->next(); }

void onUnknownCommand(char *command)
{
  out().print("unknown command: \"");
  out().print(command);
  out().println("\" (typ help)");
}

void onStart()
{
  setRun(true);
  out().println("started");
}

void onStop()
{
  setRun(false);
  out().println("stopped");
}

void onSet()
{
  char *param = arg();
  char *value = arg();
  if (param == NULL || value == NULL)
  {
    out().println("gebruik: set <param> <waarde>");
    return;
  }

  float cycleTimeInSec = ((float)params.cycleTime) / 1000000;

  if (strcmp(param, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    if (newCycleTime < 500) newCycleTime = 500;
    float ratio = ((float)newCycleTime) / ((float)params.cycleTime);
    params.ki *= ratio;
    params.kd /= ratio;
    params.cycleTime = newCycleTime;
  }
  else if (strcmp(param, "power") == 0) params.power = constrain(atoi(value), 0, 255);
  else if (strcmp(param, "diff") == 0) params.diff = constrain(atof(value), 0.0, 1.0);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);
  else if (strcmp(param, "ki") == 0) params.ki = atof(value) * cycleTimeInSec;
  else if (strcmp(param, "kd") == 0) params.kd = atof(value) / cycleTimeInSec;
  else if (strcmp(param, "lost") == 0) params.lostThreshold = constrain(atoi(value), 0, 1000);
  else if (strcmp(param, "boost") == 0) params.boost = constrain(atoi(value), 0, 255);
  else if (strcmp(param, "dfilter") == 0) params.dFilter = constrain(atof(value), 0.0, 0.95);
  else
  {
    out().print("unknown parameter: ");
    out().println(param);
    return;
  }

  EEPROM_writeAnything(0, params);
  out().print(param);
  out().print(" = ");
  out().println(value);
}

void onDebug()
{
  char *value = arg();
  if (value != NULL)
  {
    if (strcmp(value, "on") == 0) debug = true;
    else if (strcmp(value, "off") == 0) debug = false;
    out().print("debug ");
    out().println(debug ? "on" : "off");
    return;
  }

  float cycleTimeInSec = ((float)params.cycleTime) / 1000000;
  Stream &o = out();
  o.print("state: ");            o.println(run ? "running" : "stopped");
  o.print("cycle time [us]: ");  o.println(params.cycleTime);
  o.print("power: ");            o.println(params.power);
  o.print("diff: ");             o.println(params.diff, 2);
  o.print("Kp: ");               o.println(params.kp, 3);
  o.print("Ki: ");               o.println(params.ki / cycleTimeInSec, 3);
  o.print("Kd: ");               o.println(params.kd * cycleTimeInSec, 4);
  o.print("lost threshold: ");   o.println(params.lostThreshold);
  o.print("boost: ");            o.println(params.boost);
  o.print("D filter: ");         o.println(params.dFilter, 2);
  o.print("black: ");
  for (byte i = 0; i < SENSOR_COUNT; i++) { o.print(params.black[i]); o.print(" "); }
  o.println();
  o.print("white: ");
  for (byte i = 0; i < SENSOR_COUNT; i++) { o.print(params.white[i]); o.print(" "); }
  o.println();
  o.print("position [mm]: ");    o.println(position, 2);
  o.print("lap time [ms]: ");    o.println(lapTime);
  o.print("calculation time [us]: "); o.println(calculationTime);
  calculationTime = 0;
}

void onCalibrate()
{
  char *param = arg();
  if (param == NULL)
  {
    out().println("gebruik: calibrate black|white");
    return;
  }

  int *target;
  if (strcmp(param, "black") == 0) target = params.black;
  else if (strcmp(param, "white") == 0) target = params.white;
  else
  {
    out().println("gebruik: calibrate black|white");
    return;
  }

  out().print("calibrating ");
  out().print(param);
  out().print("... ");

  // gemiddelde van 16 metingen per sensor (robot staat stil => mag even duren)
  for (byte i = 0; i < SENSOR_COUNT; i++)
  {
    long sum = 0;
    for (byte n = 0; n < 16; n++) sum += analogRead(SENSOR[i]);
    target[i] = sum / 16;
  }

  EEPROM_writeAnything(0, params);
  out().println("done");
}

void onSensors()
{
  // ruwe en genormaliseerde waardes, handig bij het kalibreren
  Stream &o = out();
  o.print("raw: ");
  for (byte i = 0; i < SENSOR_COUNT; i++) { o.print(analogRead(SENSOR[i])); o.print(" "); }
  o.println();
  o.print("normalised: ");
  for (byte i = 0; i < SENSOR_COUNT; i++) { o.print(normalised[i]); o.print(" "); }
  o.println();
}

void onReset()
{
  setRun(false);
  loadDefaults();
  out().println("parameters reset to defaults");
}

void onHelp()
{
  Stream &o = out();
  o.println("start | stop");
  o.println("debug            -> toon alle parameters");
  o.println("debug on|off     -> live positie / output");
  o.println("set cycle <us>   set power <0..255>  set diff <0..1>");
  o.println("set kp <x>  set ki <x>  set kd <x>  set lost <0..1000>");
  o.println("set boost <0..255>  set dfilter <0..0.95>");
  o.println("calibrate black | calibrate white");
  o.println("sensors          -> ruwe + genormaliseerde sensorwaardes");
  o.println("reset            -> standaard parameters");
}
