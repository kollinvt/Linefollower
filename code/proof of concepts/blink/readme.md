# blink proof of concept

Het eenvoudigste microcontrollerprogramma. Het toont aan dat:
* de Arduino IDE, het ESP32 board package en de USB-driver (CP210x of CH340) correct geïnstalleerd zijn en dat uploaden lukt
* de ESP32 nog werkt
* de klok klopt: de led knippert precies **60 keer per minuut**. De seriële monitor toont ook de CPU-frequentie (240 MHz).

### schema
Geen externe componenten nodig: de blauwe led op het ESP32 DevKit hangt aan GPIO2.

### stappenplan
1. Sluit de ESP32 met USB aan op de pc.
2. Open [blink/blink.ino](blink/blink.ino) en kies het board **ESP32 Dev Module** en de juiste COM-poort.
3. Upload. De blauwe led knippert 1x per seconde.

### opmerking
Blijft het uploaden hangen op *Connecting...*? Houd dan de **BOOT**-knop ingedrukt tot het uploaden start.
