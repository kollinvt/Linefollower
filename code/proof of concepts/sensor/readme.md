# Sensoren proof of concept

minimale hard- en software die aantoont dat minimaal 6 sensoren onafhankelijk van elkaar kunnen uitgelezen worden (geen calibratie, normalisatie of interpolatie). Hierbij moet een zo groot mogelijk bereik van de AD converter benut worden (indien van toepassing)

### deelschema

```
 Arduino Micro            QTR-8A
   5V  ────────────────── VCC
   GND ────────────────── GND
   D4  ────────────────── LEDON
   A0  ────────────────── OUT2   (meest linkse gebruikte sensor)
   A1  ────────────────── OUT3
   A2  ────────────────── OUT4
   A3  ────────────────── OUT5
   A4  ────────────────── OUT6
   A5  ────────────────── OUT7   (meest rechtse gebruikte sensor)
                          OUT1, OUT8: niet aangesloten
```

De QTR-8A heeft per sensor al een voorschakelweerstand voor de IR led en een 47 kΩ pull-up op de fototransistor (spanningsdeler). Je hebt dus geen externe componenten nodig.

### code
[sensor/sensor.ino](sensor/sensor.ino): stuurt 10x per seconde de 6 ruwe ADC-waardes door. Open **Tools → Serial Plotter** (115200 baud) om ze grafisch te zien.

### stappenplan
1. Monteer de sensor op **3 mm** boven het papier.
2. Upload en open de seriële monitor of plotter.
3. Leg de sensor op wit papier: alle waardes zijn laag.
4. Schuif de zwarte lijn (15 mm) langzaam van links naar rechts onder de sensor door: S1 tot S6 worden na elkaar hoog.

### resultaat
| | typische waarde |
|---|---|
| wit papier | 40 .. 150 |
| zwarte lijn | 800 .. 980 |

* Het bereik van de 10-bit ADC (0..1023) wordt bijna volledig benut, **zonder 0 of 1023** (geen saturatie).
* Is zwart te laag (< 600)? Dan staat de sensor te hoog: verlaag hem. Is wit te hoog? Dan valt er waarschijnlijk zonlicht in: scherm de sensor af.
