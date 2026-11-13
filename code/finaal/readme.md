# finale code

[linefollower/linefollower.ino](linefollower/linefollower.ino) bouwt verder op het [basis programma](../basis/readme.md). In het cyclische deel (standaard elke 2,5 ms) gebeurt:

1. **sensoren uitlezen + normaliseren**: `map(raw, black[i], white[i], 0, 1000)` per sensor, met de kalibratiewaardes uit EEPROM (zwart = 0, wit = 1000).
2. **interpoleren**: kwadratische interpolatie door de zwartste sensor en zijn 2 buren. Het dal van de parabool geeft de positie in mm (0 = midden, + = lijn rechts).
   * **lijn kwijt** (zwartste sensor > `lost`): de robot stuurt maximaal naar de kant waar hij de lijn het laatst zag.
   * **kruispunt** (≥ 5 sensoren zwart): positie = 0, dus rechtdoor. Twee keer hetzelfde kruispunt kruisen geeft de **rondetijd** (`debug` → `lap time`).
3. **PID**
   * P: `output = Kp · error`
   * I: `iTerm += Ki* · error`, begrensd tot ±510 (anti reset windup). `Ki* = Ki · cycleTime` wordt herrekend bij het aanpassen van de cyclustijd.
   * D: `Kd* · (error - lastErr)` met `Kd* = Kd / cycleTime`, gefilterd door een 1ste orde laagdoorlaatfilter (`dfilter`).
   * output begrensd tot ±510
4. **snelheid**: `power + boost` op een recht stuk. De boost valt lineair weg naarmate de fout groter wordt (bochten).
5. **verdelen over de motoren** met parameter `diff` (zie leerpad *proportioneel regelen*) en de motoren aansturen (pwm + draairichting, DRV8833). `MAX_PWM` begrenst de motorspanning tot ± 7V.

De I-term wordt bij elke start en stop op 0 gezet. Er staat **geen seriële communicatie in het cyclische deel**: debug-uitvoer gebeurt erbuiten, maximaal 4x per seconde, en enkel als `debug on` aan staat.

Rekentijd gemeten via `debug` → `calculation time`: richtwaarde ± 1,2 ms (6x analogRead ± 0,7 ms + float PID). Een cyclustijd van 2500 µs geeft dus ruim 2x marge.

Compileren: Arduino IDE, board **Arduino Micro**, open `linefollower.ino` (de bibliotheken staan in dezelfde map).  
Gebruik: zie [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md).

### pinout
| functie | pin |
|---|---|
| sensoren S2..S7 | A0..A5 |
| sensor IR leds aan | D4 |
| motor links AIN1 / AIN2 | D5 / D6 |
| motor rechts BIN1 / BIN2 | D9 / D10 |
| start/stop knop | D7 (INT6) |
| bluetooth HC-05 | D0 / D1 (Serial1, 9600) |
| run led | D13 |
