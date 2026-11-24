# finale code

[LineFollower/LineFollower.ino](LineFollower/LineFollower.ino): ESP32 + DRV8833 + 2x N20 + Yahboom 8-kanaals sensor (analoog, UART).

Benodigd: board package **esp32 by Espressif**, board **ESP32 Dev Module**, bibliotheek **WebSockets** van Markus Sattler.

> **DUO-ontwerp (2 sensoren):** deze code gebruikt de sensor **STUREN** (GPIO16/17). De sensor **VOORAAN** (GPIO18/19) is in de hardware voorzien voor vooruitkijken en afremmen voor de bocht.

## opbouw

| bestand | functie |
|---|---|
| [config.h](LineFollower/config.h) | vaste hardware-instellingen: pinnen, pwm, sensor, WiFi, start/stop knop |
| [params.cpp](LineFollower/params.cpp) / [.h](LineFollower/params.h) | tabel met alle instelbare parameters + opslaan in flash (Preferences) |
| [sensor.cpp](LineFollower/sensor.cpp) / [.h](LineFollower/sensor.h) | UART-frames van de Yahboom-sensor lezen, kalibratie (min/max per sensor), lijnsterkte 0..1 |
| [control.cpp](LineFollower/control.cpp) / [.h](LineFollower/control.h) | regeltaak op **core 1**: positie, PID, snelheidsprofiel, motoren |
| [motors.cpp](LineFollower/motors.cpp) / [.h](LineFollower/motors.h) | DRV8833 aansturing (pwm 20 kHz), dode-zone compensatie, startpuls |
| [web.cpp](LineFollower/web.cpp) / [.h](LineFollower/web.h) | WiFi access point, webserver en websocket op **core 0** |
| [page.h](LineFollower/page.h) | de webpagina (HTML/JS) die op de robot staat |

## werking (elk sensorframe, op core 1)

1. **sensor**: een frame `$A,x1:..,x8:..#` komt binnen → per sensor `n = (raw - min) / (max - min)` met de kalibratiewaardes uit flash → lijnsterkte 0..1 (1 = recht boven de lijn).
2. **positie (interpolatie)**: gewogen gemiddelde. Elke sensor heeft een positie -7, -5, .., +5, +7. Sensoren onder de ruisdrempel (`noise`) tellen niet mee. Het resultaat is de fout `e` (-7 .. +7, positief = lijn rechts).
   * **lijn kwijt** (totale sterkte < 0,25): `e = ±lostErr` naar de kant waar de lijn het laatst was
   * **kruispunt** (≥ `crossN` sensoren actief én beide buitenste): `crossMs` ms rechtdoor (`e = 0`)
3. **snelheidsprofiel**: `target = vMax - curve · (|e| - curveSt)`, begrensd tot `vMin`. Afremmen gebeurt meteen, optrekken met hoogstens `accel` per frame.
4. **PID**
   * P: `kp · e`
   * I: `ki · Σe`, begrensd tot ±`iMax` (anti windup). Er wordt niet geïntegreerd als de lijn kwijt is.
   * D: `kd · dF`, waarbij `dF` de gefilterde verandering per frame is (`dFilter`, 1 = geen filter)
5. **naar de wielen**: `L = base + out`, `R = base - out`. Boven 255 zakken beide wielen samen, zodat het stuurverschil behouden blijft. `maxRev` begrenst het achteruit draaien en `trimL`/`trimR` corrigeren verschillen tussen de motoren.
6. **motoren**: 0..255 wordt geschaald naar `runPwm`..255 (dode zone). Bij vertrek of omkeren krijgt de motor eerst `kickMs` ms een startpuls `kickPwm`.

**Veiligheid**: komt er 150 ms geen sensorframe binnen (`SENSOR_TIMEOUT_MS`), dan gaan de motoren meteen uit. Tijdens het kalibreren rijdt de robot niet.

**Start/stop**: webpagina (START / STOP, spatiebalk = STOP) of de **BOOT-knop** op de robot. De knop werkt via een hardware interrupt: de ISR zet een vlag en `loop()` schakelt. De blauwe led (GPIO2) brandt zolang de robot rijdt.

**Telemetrie**: 20x per seconde naar de webpagina: fout, basissnelheid, PID-uitvoer, motor-pwm, sterkte per sensor, frames per seconde en de rekentijd per frame (µs).

Gebruik en tunen: zie [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md).

## pinout

| functie | GPIO |
|---|---|
| motor links AIN1 / AIN2 | 26 / 27 |
| motor rechts BIN1 / BIN2 | 25 / 33 |
| sensor STUREN RX2 / TX2 | 16 / 17 (115200 baud) |
| sensor VOORAAN | 18 / 19 |
| start/stop knop | 0 (BOOT) |
| run led | 2 |
