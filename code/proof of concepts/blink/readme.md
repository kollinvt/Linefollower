# blink proof of concept

Het eenvoudigste microcontrollerprogramma. Het toont aan dat:
* de Arduino IDE, de compiler en de USB-drivers correct geïnstalleerd zijn en dat uploaden lukt
* de microcontroller nog werkt
* de kloksnelheid klopt: de led knippert precies **60 keer per minuut** (controleer met een stopwatch)

### schema
Geen externe componenten nodig: de led op het Arduino Micro board hangt aan D13.

### stappenplan
1. Sluit de Arduino Micro met USB aan op de pc.
2. Open [blink/blink.ino](blink/blink.ino) en kies het board **Arduino Micro** en de juiste COM-poort.
3. Upload. De led L knippert 1x per seconde.

### opmerking
Lukt het uploaden niet bij een ATmega32U4? Druk dan 2x snel op reset vlak nadat "Uploading..." verschijnt. De bootloader blijft dan 8 s actief.
