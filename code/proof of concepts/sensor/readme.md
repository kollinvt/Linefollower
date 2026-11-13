# Sensoren proof of concept

minimale hard- en software die aantoont dat minimaal 6 sensoren onafhankelijk van elkaar kunnen uitgelezen worden (geen calibratie, normalisatie of interpolatie). Hierbij moet een zo groot mogelijk bereik van de AD converter benut worden (indien van toepassing)

### deelschema

```
 ESP32 DevKit                Yahboom 8-kanaals sensor
   5V (MT3608)  ──────────── 5V
   GND          ──────────── GND
   GPIO16 (RX2) ──────────── TX
   GPIO17 (TX2) ──────────── RX
```

TX en RX worden **gekruist**: TX van de sensor gaat naar RX van de ESP32 en omgekeerd.

De Yahboom-module heeft 8 IR-sensoren en een eigen microcontroller met ADC (12 bit, 0..4095). Hij stuurt de waardes via UART (115200 baud) door. Het commando `$0,1,0#` zet de module in de modus: *geen interne kalibratie, analoge uitvoer aan, digitale uitvoer uit*. Daarna stuurt hij frames als:

```
$A,x1:3012,x2:2950,x3:640,x4:410,x5:2890,x6:3050,x7:3001,x8:2987#
```

### code
[sensor/sensor.ino](sensor/sensor.ino): parseert de frames en print 10x per seconde de 8 ruwe waardes. Open **Tools → Serial Plotter** (115200 baud) om ze grafisch te zien. Komt er geen data binnen, dan stuurt de code het commando opnieuw en meldt dat op de monitor.

### stappenplan
1. Monteer de sensor op ± 3 mm boven het papier.
2. Upload en open de seriële monitor of plotter.
3. Leg de sensor op wit papier en noteer de waardes.
4. Schuif de zwarte lijn (15 mm) langzaam van links naar rechts onder de sensor door: S1 tot S8 reageren na elkaar.

### resultaat
* alle 8 sensoren zijn **individueel** uit te lezen en reageren elk apart op de zwarte lijn
* het verschil wit ↔ zwart per sensor is groot (het bereik van de 12-bit ADC wordt goed benut). De finale code eist bij het kalibreren minimaal 200 eenheden verschil per sensor (`CAL_MIN_RANGE`).
* geen enkele sensor blijft op 0 of 4095 hangen (geen saturatie)
* *noteer hier je gemeten waardes:* wit = ...., zwart = ....
