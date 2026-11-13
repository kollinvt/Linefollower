# basis programma

volgende zaken werken samen in dit template project ([basis/basis.ino](basis/basis.ino), ESP32)

* instelbare parameters mbv. commando's (zelfde syntax als *SerialCommand*: `<commando> <arg1> <arg2>`), via **WiFi** (webpagina + websocket) én **USB** (seriële monitor 115200)
  * cyclus tijd: `set cycle <µs>`
  * debug on/off: `set debug on|off`, `debug` toont de status
  * start/stop: `start`, `stop`
* parameters worden opgeslagen in het flashgeheugen met **Preferences**, de EEPROM-emulatie van de ESP32
* start/stop kan ook geactiveerd worden mbv. een externe interrupt: de **BOOT-knop (GPIO0)**
* draadloze communicatie: access point `LineFollower-basis` (12345678), http://192.168.4.1

Met behulp van de cyclus wordt een knipperende led geprogrammeerd: zolang de robot "rijdt", knippert de blauwe led (GPIO2) aan 1 Hz, onafhankelijk van de ingestelde cyclustijd.

### opmerkingen
* **non-blocking**: webserver, websocket en USB worden in elke loop kort afgehandeld. Er wordt nooit gewacht.
* In het cyclische deel staat **geen communicatie**. Debug-uitvoer gebeurt erbuiten (1x per seconde).
* Naar flash schrijven pauzeert de ESP32 even. Daarom kan `set` alleen als de robot stilstaat. De finale code doet hetzelfde met de knop *Opslaan in robot*.

### test
1. Upload en open de seriële monitor (115200, *Newline*), of verbind met WiFi **LineFollower-basis** en surf naar http://192.168.4.1.
2. `set cycle 2000` → `debug` toont `cycle time: 2000` en de gemeten `calculation time`.
3. Haal de spanning eraf en zet ze terug → `debug` toont nog steeds 2000 (**flash-opslag werkt**).
4. `start` of druk op BOOT → de led knippert. `stop` of opnieuw drukken → de led gaat uit.

### van basis naar finaal
In de [finale code](../finaal/readme.md) is dezelfde opbouw verder uitgewerkt:
* de vaste cyclus wordt **elk sensorframe** (de Yahboom-sensor bepaalt het ritme), op een eigen FreeRTOS-taak op core 1
* de communicatie draait op een aparte taak op core 0 en kan de regeling dus nooit vertragen
* de parameters staan in een tabel ([params.cpp](../finaal/LineFollower/params.cpp)) waaruit de webpagina zichzelf opbouwt
