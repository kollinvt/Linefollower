/*
 * Basis programma - LineFollower plan B
 * ---------------------------------------------------------------------------
 * Kapstok voor de finale code. Volgende zaken werken hier samen:
 *  - instelbare parameters via SerialCommand (USB én bluetooth HC-05)
 *      set cycle <µs>, set debug on|off, start, stop, debug
 *  - parameters worden opgeslagen in het EEPROM geheugen
 *  - vaste cyclus (zoals een plc) + meting van de rekentijd
 *  - start/stop ook via een drukknop op een externe interrupt (D7)
 *  - demo in de cyclus: led D13 knippert (1 Hz) zolang de robot "rijdt"
 */
#include "SerialCommand.h"
#include "EEPROMAnything.h"

#define USB_PORT Serial
#define BT_PORT Serial1

const byte PIN_BUTTON = 7;
const byte PIN_LED = 13;
const unsigned int PARAM_VERSION = 0xB001;

SerialCommand sCmdUsb(USB_PORT);
SerialCommand sCmdBt(BT_PORT);

struct param_t
{
  unsigned int version;
  unsigned long cycleTime; // µs
  /* andere parameters die in het eeprom geheugen moeten opgeslagen worden ... */
} params;

volatile bool run = false;
volatile unsigned long lastPress = 0;
bool debug;
unsigned long previous, calculationTime, cycleCount;

void setup()
{
  USB_PORT.begin(115200);
  BT_PORT.begin(9600);

  addCommands(sCmdUsb);
  addCommands(sCmdBt);

  EEPROM_readAnything(0, params);
  if (params.version != PARAM_VERSION)
  {
    params.version = PARAM_VERSION;
    params.cycleTime = 1000;
    EEPROM_writeAnything(0, params);
  }

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);

  USB_PORT.println("ready");
  BT_PORT.println("ready");
}

void addCommands(SerialCommand &c)
{
  c.addCommand("set", onSet);
  c.addCommand("debug", onDebug);
  c.addCommand("start", onStart);
  c.addCommand("stop", onStop);
  c.setDefaultHandler(onUnknownCommand);
}

void loop()
{
  sCmdUsb.readSerial();
  sCmdBt.readSerial();

  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    /* code die cyclisch moet uitgevoerd worden: led knippert op 1 Hz,
       onafhankelijk van de ingestelde cyclustijd */
    if (run)
    {
      cycleCount++;
      unsigned long half = 500000UL / params.cycleTime;
      if (cycleCount >= half)
      {
        cycleCount = 0;
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      }
    }
    else digitalWrite(PIN_LED, LOW);

    /* normaliseren en interpoleren sensor */
    /* pid regeling */
    /* aansturen motoren */
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

void onButton()
{
  unsigned long now = millis();
  if (now - lastPress < 250) return;
  lastPress = now;
  run = !run;
}

Stream &out() { return SerialCommand::current->port(); }

void onUnknownCommand(char *command)
{
  out().print("unknown command: \"");
  out().print(command);
  out().println("\"");
}

void onStart() { run = true;  out().println("started"); }
void onStop()  { run = false; out().println("stopped"); }

void onSet()
{
  char *param = SerialCommand::current->next();
  char *value = SerialCommand::current->next();
  if (param == NULL || value == NULL) return;

  if (strcmp(param, "debug") == 0)
  {
    if (strcmp(value, "on") == 0) debug = true;
    else if (strcmp(value, "off") == 0) debug = false;
  }
  else if (strcmp(param, "cycle") == 0) params.cycleTime = max(100L, atol(value));

  EEPROM_writeAnything(0, params);
  out().println("ok");
}

void onDebug()
{
  out().print("state: ");
  out().println(run ? "running" : "stopped");
  out().print("debug: ");
  out().println(debug ? "on" : "off");
  out().print("cycle time: ");
  out().println(params.cycleTime);
  out().print("calculation time: ");
  out().println(calculationTime);
  calculationTime = 0;
}
