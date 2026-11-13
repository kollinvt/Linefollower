# basis programma

volgende zaken werken samen in dit template project ([basis/basis.ino](basis/basis.ino))

* instelbare parameters mbv. SerialCommands 
  * cyclus tijd (`set cycle <µs>`)
  * debug on/off (`set debug on|off`, `debug` toont de status)
  * start/stop (`start`, `stop`)
* parameters worden opgeslagen in eeprom geheugen (`EEPROMAnything.h`, struct `params` op adres 0)
* start/stop kan ook geactiveerd worden mbv. een externe interrupt (drukknop op D7)
* draadloze communicatie: dezelfde commando's werken via **USB (Serial)** én **bluetooth HC-05 (Serial1)**

Met behulp van de cyclus wordt een knipperende led geprogrammeerd: zolang de robot "rijdt", knippert D13 aan 1 Hz, onafhankelijk van de ingestelde cyclustijd.

### bibliotheken (in dezelfde map, worden automatisch mee gecompileerd)
* `SerialCommand.h/.cpp`: non-blocking parser `<commando> <arg1> <arg2><CR+LF>`. Meerdere poorten kunnen dezelfde eventhandlers delen. `SerialCommand::current` wijst naar de poort waarop het commando binnenkwam, zodat het antwoord naar de juiste poort teruggaat.
* `EEPROMAnything.h`: schrijft / leest een volledige struct in één keer (`EEPROM.update`, dus enkel gewijzigde bytes).

### test
1. Upload, open de seriële monitor (115200, *Both NL & CR*) of verbind via bluetooth (9600).
2. `set cycle 2000` → `debug` toont `cycle time: 2000` en de gemeten `calculation time`.
3. Haal de spanning eraf en zet ze terug → `debug` toont nog steeds 2000 (**EEPROM werkt**).
4. `start` of druk op de knop → de led knippert. `stop` of opnieuw drukken → de led gaat uit.
