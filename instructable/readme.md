# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.  

**benodigd gereedschap**: soldeerbout + tin, kniptang, striptang, multimeter, ijzerzaag of breekmes + lat (protoboard op maat), boormachine Ø3 mm, schroevendraaier, lijmpistool niet nodig (geen hotglue!).

### stap 1
bestel alle componenten uit de [bill of materials](../bill%20of%20materials/readme.md)  

### stap 2
druk de [mechanische tekening](../technische%20tekeningen/mechanisch/mechanisch_planB.pdf) en het [elektronisch schema](../technische%20tekeningen/elektronisch/schema_planB.pdf) af op A4

### stap 3
zaag / breek het protoboard op **80 x 140 mm**, schuur de randen glad

### stap 4
leg alle onderdelen op het protoboard volgens de mechanische tekening en markeer de posities met een stift

### stap 5
boor 2 gaten Ø3 mm voor de ball caster (x = 104 mm, midden) en 2 gaten Ø3 mm voor de sensor (x = 125 mm, 35 mm uit elkaar, of de bevestigingsgaten van de QTR-8A)

### stap 6
soldeer 2 korte draden (± 6 cm) aan de aansluitingen van elke N20 motor

### stap 7
monteer beide N20 motoren met de motorbeugels onderaan de print, as op **25 mm** van de achterkant, assen naar buiten. Druk de wielen op de assen.

### stap 8
monteer de ball caster onderaan met M3 afstandhouders, zodat de print horizontaal staat

### stap 9
monteer de QTR-8A onderaan vooraan met M3 afstandhouders. De sensoren wijzen naar beneden, **3 mm** boven de grond. Stel bij met ringetjes.

### stap 10
soldeer de female pin headers voor de Arduino Micro, de DRV8833 en de HC-05 op het protoboard (posities volgens de tekening)

### stap 11
soldeer SW1, S1, D1 + R3, R1 + R2, C1 en C2 op het protoboard volgens het schema (let op: polariteit van C1 en D1)

### stap 12
bevestig de batterijhouder dwars, net voor de wielas (2 schroefjes M3 of dubbelzijdige foam tape)

### stap 13
leg de **vermogendraden** (VBAT en GND) aan: batterij + → SW1 → C1/C2 → DRV8833 VM en Arduino VIN, batterij - → GND-rail. Hou ze zo kort mogelijk.

### stap 14
verbind de motordraden: linker motor → AOUT1/AOUT2, rechter motor → BOUT1/BOUT2

### stap 15
leg alle **signaaldraden** aan volgens het [aansluitschema](../technische%20tekeningen/elektronisch/readme.md#aansluitingen): sensor OUT2..OUT7 → A0..A5, LEDON → D4, D5/D6/D9/D10 → AIN1/AIN2/BIN1/BIN2, D7 → S1, D0/D1 → HC-05, 5V → nSLEEP, sensor, HC-05

### stap 16
controleer met de multimeter (zonder batterijen, zonder breakout boards): **geen kortsluiting tussen VBAT en GND, tussen +5V en GND en tussen VBAT en +5V**

### stap 17
plug de Arduino Micro, de DRV8833 en de HC-05 in hun headers. Let op de oriëntatie.

### stap 18
installeer de **Arduino IDE 2** (arduino.cc/en/software). Er zijn geen extra bibliotheken nodig.

### stap 19
download deze repository: **Code → Download ZIP** en pak hem uit

### stap 20
sluit de Arduino Micro met USB aan (nog **zonder** batterijen), open `code/proof of concepts/blink/blink/blink.ino`, kies **Tools → Board → Arduino Micro** en de juiste poort en klik **Upload** → de led knippert

### stap 21
upload `code/proof of concepts/sensor/sensor/sensor.ino` en controleer met de Serial Plotter dat alle 6 sensoren reageren op de zwarte lijn

### stap 22
plaats de opgeladen batterijen, zet SW1 aan, upload `code/proof of concepts/hbridge/hbridge/hbridge.ino` en test met `t` in de seriële monitor (robot omhoog houden!)

### stap 23
upload `code/proof of concepts/wireless communication/wireless/wireless.ino`, koppel de HC-05 met de pc (pincode 1234) en test de verbinding

### stap 24
upload de finale code `code/finaal/linefollower/linefollower.ino`

### stap 25
open `code/app/linefollower.html` in Chrome / Edge, klik **Verbinden** en kies de bluetooth COM-poort (9600)

### stap 26
kalibreer: alle sensoren boven zwart → **Kalibreer zwart**, alle sensoren boven wit → **Kalibreer wit**

### stap 27
plaats de robot op het parcours en druk op **START** (of op de drukknop S1)

### stap 28
tune de parameters volgens de [gebruiksaanwijzing](../gebruiksaanwijzing/readme.md#tunen-snel-en-betrouwbaar)

### stap 29
monteer de spoiler 🏁
