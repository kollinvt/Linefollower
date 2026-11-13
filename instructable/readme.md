# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.  

**benodigd gereedschap**: soldeerbout + tin, kniptang, striptang, multimeter, kleine schroevendraaier (potmeter MT3608), ijzerzaag of breekmes + lat, boormachine Ø3 mm.

### stap 1
bestel alle componenten uit de [bill of materials](../bill%20of%20materials/readme.md)  

### stap 2
druk de [mechanische tekening](../technische%20tekeningen/mechanisch/mechanisch_planB.pdf) en het [elektronisch schema](../technische%20tekeningen/elektronisch/schema_planB.pdf) af op A4

### stap 3
zaag / breek het protoboard op **80 x 140 mm** en schuur de randen glad

### stap 4
leg alle onderdelen op het protoboard volgens de mechanische tekening en markeer de posities met een stift

### stap 5
boor 2 gaten Ø3 mm voor de ball caster (x = 104 mm, midden) en 2 gaten Ø3 mm voor de sensor (x = 125 mm, volgens de bevestigingsgaten van de Yahboom-sensor)

### stap 6
soldeer 2 draden (± 6 cm) aan de aansluitingen van elke N20 motor

### stap 7
monteer beide N20 motoren met de motorbeugels onderaan de print, as op **25 mm** van de achterkant, assen naar buiten; druk de wielen op de assen

### stap 8
monteer de ball caster onderaan met M3 afstandhouders, zodat de print horizontaal staat

### stap 9
monteer de Yahboom-sensor onderaan vooraan met M3 afstandhouders, sensoren naar beneden, ± **3 mm** boven de grond

### stap 10
soldeer female pin headers voor de ESP32 DevKit en de DRV8833 op het protoboard (posities volgens de tekening)

### stap 11
sluit de MT3608 aan op een labovoeding of op de LiPo, meet de uitgang en draai aan de potmeter tot **5,0V** (dit **vóór** je de ESP32 aansluit!)

### stap 12
soldeer SW1, de TP4056, de MT3608, D1 + R1, C1 en C2 op het protoboard volgens het schema (let op: polariteit van C1 en D1)

### stap 13
soldeer een JST-PH connector met draden: rood → BAT+ (TP4056 B+ en SW1), zwart → GND (TP4056 B-)

### stap 14
leg de **vermogendraden** aan: SW1 → VBAT → DRV8833 VM en MT3608 VIN+, GND-rail naar alle GND's; zo kort mogelijk

### stap 15
leg de 5V-draden aan: MT3608 VOUT+ → ESP32 VIN, sensor 5V, R1 (power led)

### stap 16
verbind de motordraden: linker motor → AOUT1/AOUT2, rechter motor → BOUT1/BOUT2

### stap 17
leg de **signaaldraden** aan volgens het [aansluitschema](../technische%20tekeningen/elektronisch/readme.md#aansluitingen): GPIO26/27/25/33 → AIN1/AIN2/BIN1/BIN2, ESP32 3V3 → DRV8833 nSLEEP, sensor TX → GPIO16, GPIO17 → sensor RX

### stap 18
controleer met de multimeter (zonder batterij, zonder breakout boards): **geen kortsluiting tussen VBAT en GND, +5V en GND, +3V3 en GND en tussen VBAT en +5V**

### stap 19
plug de ESP32 DevKit en de DRV8833 in hun headers (let op de oriëntatie) en bevestig de LiPo plat voor de wielas (dubbelzijdige foam tape of klittenband, geen hotglue)

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
upload `code/proof of concepts/sensor/sensor/sensor.ino` en controleer met de Serial Plotter dat alle 8 sensoren reageren op de zwarte lijn

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
