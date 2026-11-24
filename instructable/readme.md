# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.  

**benodigd gereedschap**: 3D-printer (of FabLab), soldeerbout + tin, kniptang, striptang, multimeter, kleine schroevendraaier (potmeter MT3608), boortje Ø3 mm.

### stap 1
bestel alle componenten uit de [bill of materials](../bill%20of%20materials/readme.md)  

### stap 2
druk de [mechanische tekening](../technische%20tekeningen/mechanisch/mechanisch_planB.pdf) en het [elektronisch schema](../technische%20tekeningen/elektronisch/schema_planB.pdf) af op A4

### stap 3
voer in Fusion 360 het script [fusion_bodemplaat_duo](../technische%20tekeningen/mechanisch/fusion_bodemplaat_duo/fusion_bodemplaat_duo.py) uit (**Utilities → Scripts and Add-Ins → +** → map kiezen → **Run**), exporteer de bodemplaat als **STL** (rechtsklik op de body → *Save as Mesh*) en print hem in PETG of PLA (0,2 mm, 3-4 wanden, 30 % infill, plat, zonder support)

### stap 4
verwijder de printresten en ruim de gaten Ø3 mm uit met een boortje

### stap 5
leg alle onderdelen op de plaat volgens de mechanische tekening (ESP32 midden, DRV8833 + MT3608 achteraan, sensor VOORAAN onder de voorste balk, sensor STUREN onder de tweede balk) en controleer of alles past

### stap 6
soldeer 2 draden (± 6 cm) aan de aansluitingen van elke N20 motor

### stap 7
monteer beide N20 motoren met de motorbeugels onderaan de plaat, as op **20 mm** van de achterkant, assen naar buiten. Druk de wielen op de assen: ze vallen in de uitsparingen van de plaat.

### stap 8
monteer de ball caster onder de **smalle hals** (35 mm van de voorkant) met M3 afstandhouders, zodat de plaat horizontaal staat

### stap 9
monteer de 2 Yahboom-sensoren met M3 afstandhouders, sensoren naar beneden, ± **3 mm** boven de grond: **U6 (VOORAAN)** onder de voorste sensorbalk, **U3 (STUREN)** onder de tweede sensorbalk. Beide even hoog!

### stap 10
soldeer female pin headers voor de ESP32 DevKit en de DRV8833 op een stukje gaatjesprint en schroef / lijm die op de plaat op de geschetste posities (M3, of dubbelzijdige foam tape)

### stap 11
sluit de MT3608 aan op een labovoeding of op de LiPo, meet de uitgang en draai aan de potmeter tot **5,0V** (dit **vóór** je de ESP32 aansluit!)

### stap 12
soldeer SW1, D1 + R1, C1 en C2 op de kleine gaatjesprint (voedingsverdeling) en bevestig de TP4056 en de MT3608 achteraan op de plaat (let op: polariteit van C1 en D1)

### stap 13
soldeer een JST-PH connector met draden: rood → BAT+ (TP4056 B+ en SW1), zwart → GND (TP4056 B-)

### stap 14
leg de **vermogendraden** aan: SW1 → VBAT → DRV8833 VM en MT3608 VIN+, GND-rail naar alle GND's; zo kort mogelijk

### stap 15
leg de 5V-draden aan: MT3608 VOUT+ → ESP32 VIN, sensor 5V, R1 (power led)

### stap 16
verbind de motordraden: linker motor → AOUT1/AOUT2, rechter motor → BOUT1/BOUT2

### stap 17
leg de **signaaldraden** aan volgens het [aansluitschema](../technische%20tekeningen/elektronisch/readme.md#aansluitingen): GPIO26/27/25/33 → AIN1/AIN2/BIN1/BIN2, ESP32 3V3 → DRV8833 nSLEEP, sensor STUREN TX → GPIO16, GPIO17 → sensor STUREN RX, sensor VOORAAN TX → GPIO18, GPIO19 → sensor VOORAAN RX

### stap 18
controleer met de multimeter (zonder batterij, zonder breakout boards): **geen kortsluiting tussen VBAT en GND, +5V en GND, +3V3 en GND en tussen VBAT en +5V**

### stap 19
plug de ESP32 DevKit en de DRV8833 in hun headers (let op de oriëntatie) en bevestig de LiPo net voor de wielas (klittenband of een geprinte houder, geen hotglue)

### stap 20
installeer de **Arduino IDE 2** (arduino.cc/en/software)

### stap 21
Arduino IDE → **Bestand → Voorkeuren → Additional boards manager URLs**: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`

### stap 22
**Bordenbeheer** → installeer **esp32 by Espressif Systems**

### stap 23
**Bibliotheekbeheer** → installeer **WebSockets** van **Markus Sattler**

### stap 24
download deze repository: **Code → Download ZIP** en pak hem uit

### stap 25
sluit de ESP32 met USB aan (nog **zonder** batterij), kies **Tools → Board → ESP32 Dev Module** en de juiste poort, open `code/proof of concepts/blink/blink/blink.ino` en klik **Upload** → de blauwe led knippert

### stap 26
upload `code/proof of concepts/sensor/sensor/sensor.ino` en controleer met de Serial Plotter dat alle 8 sensoren reageren op de zwarte lijn (eerst sensor STUREN; voor sensor VOORAAN zet je in de sketch `SENSOR_RX 18` en `SENSOR_TX 19`)

### stap 27
steek de LiPo in, zet SW1 aan, upload `code/proof of concepts/hbridge/hbridge/hbridge.ino` en test met `t` in de seriële monitor (robot omhoog houden!)

### stap 28
upload de finale code `code/finaal/LineFollower/LineFollower.ino`

### stap 29
verbind met WiFi **LineFollower** (wachtwoord 12345678) en surf naar **http://192.168.4.1**

### stap 30
kalibreer: **Start kalibratie**, schuif de robot over de lijn tot elke sensor zwart en wit gezien heeft, **Stop kalibratie**

### stap 31
plaats de robot op het parcours en druk op **START** (of op de BOOT-knop)

### stap 32
tune de parameters volgens de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md#tunen-snel-en-betrouwbaar) en klik **Opslaan in robot**

### stap 33
monteer de spoiler 🏁
