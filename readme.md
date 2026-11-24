# LineFollower

Autonome linefollower robot - syntheseproject HOGENT - **plan B**: een volledig werkende robot opgebouwd met breakout boards. Een **ESP32** stuurt hem aan met een PID-regeling. Je bedient hem draadloos via een **WiFi-webpagina** en de instellingen worden in het flashgeheugen bewaard.

![mechanische layout](images/mechanisch_planB.png)

## specifications

| | |
|---|---|
| **microcontroller** | ESP32 DevKit V1 (ESP32-WROOM-32, dual core 240 MHz) |
| **motors** | 2x N20 micro metal gear motor 6V |
| **h-bridge** | DRV8833 breakout (dubbele mosfet h-brug), pwm 20 kHz |
| **sensors** | **2x** Yahboom 8-kanaals IR lijnsensor **achter elkaar**: sensor VOORAAN (vooruitkijken, afremmen voor de bocht) + sensor STUREN (PID) |
| **batteries** | PKCELL LiPo 1S 3,7V 2500 mAh (LP785060), MT3608 step-up naar 5V, TP4056 lader |
| **wireless communication** | WiFi access point "LineFollower" + webpagina met websocket (http://192.168.4.1) |
| **chassis** | 3D-geprinte bodemplaat DUO 4 mm (Fusion 360-script: [fusion_bodemplaat_duo](technische%20tekeningen/mechanisch/fusion_bodemplaat_duo/fusion_bodemplaat_duo.py)) |
| **distance sensor - motors** | sensor VOORAAN 120 mm, sensor STUREN 70 mm voor de wielas |
| **weight** | ± 170 g |
| **dimensions** | 150 x 96 x ± 41 mm (L x B x H), past onder de chronobrug (12 x 12 cm) |
| **speed** | *in te vullen na de test* (minimum 0,5 m/s) |
| **kostprijs** | ± € 71 (zie [bill of materials](bill%20of%20materials/readme.md)) |

## software in het kort

* **2 cores**: core 1 leest de sensor, berekent de positie en de PID en stuurt de motoren aan bij **elk sensorframe**. Core 0 doet WiFi, de webpagina en de websocket, zodat de regeling nooit door de communicatie geblokkeerd wordt.
* **lijnpositie**: gewogen gemiddelde van 8 gekalibreerde sensoren (fout -7 .. +7), met ruisdrempel.
* **PID** met gefilterde D-term en begrensde I-term (anti windup).
* **snelheidsprofiel**: volle snelheid rechtdoor, afremmen in bochten (`curve`, `curveSt`), zacht optrekken (`accel`).
* **motoren**: compensatie van de dode zone (`runPwm`), startpuls (`kickPwm`), trim per wiel.
* **lijn kwijt**: maximaal sturen naar de kant waar de lijn het laatst was. **Kruispunt**: rechtdoor.
* **veiligheid**: komt er 150 ms geen sensorframe binnen, dan gaan de motoren uit.
* **start/stop**: via de webpagina, de **spatiebalk** (stop) of de **BOOT-knop** op de robot (hardware interrupt).
* **instellingen**: meer dan 20 parameters, live aanpasbaar op de webpagina en bewaard in flash (Preferences, de EEPROM-emulatie van de ESP32).

## inhoud van deze repository

| map | inhoud |
|---|---|
| [bill of materials](bill%20of%20materials/readme.md) | alle onderdelen, aantallen, aankoopbron en prijs ([csv](bill%20of%20materials/bom.csv)) |
| [technische tekeningen/elektronisch](technische%20tekeningen/elektronisch/readme.md) | elektronisch schema ([pdf](technische%20tekeningen/elektronisch/schema_planB.pdf) + [svg bron](technische%20tekeningen/elektronisch/schema_planB.svg)) |
| [technische tekeningen/mechanisch](technische%20tekeningen/mechanisch/readme.md) | mechanische layout ([pdf](technische%20tekeningen/mechanisch/mechanisch_planB.pdf) + [svg bron](technische%20tekeningen/mechanisch/mechanisch_planB.svg)) |
| [code/proof of concepts](code/proof%20of%20concepts/readme.md) | blink, h-brug, sensor, start/stop interrupt, draadloze communicatie |
| [code/basis](code/basis/readme.md) | basisprogramma: commando's via WiFi + USB, flash-opslag, vaste cyclus, interrupt |
| [code/finaal](code/finaal/readme.md) | finale linefollower code (sensor, PID, motoren, webpagina) |
| [instructable](instructable/readme.md) | stappenplan om de robot na te bouwen |
| [gebruiksaanwijzing](gebruiksaanwijzing/readme.md) | gebruik, webpagina, kalibratie, parameters, tunen |
| [datasheets](datasheets/readme.md) | datasheets van de gebruikte componenten |
| [parcours](parcours) | testparcours (pdf) |

## testresultaten

| datum | parcours | vMax | Kp / Ki / Kd | rondetijd | gem. snelheid | opmerking |
|---|---|---|---|---|---|---|
| | | | | | | *in te vullen na het testen* |

> Prioriteit 1 uit plan B: eerst **3 toeren** foutloos rijden, pas daarna sneller maken (zie [gebruiksaanwijzing - tunen](gebruiksaanwijzing/readme.md#tunen-snel-en-betrouwbaar)).
