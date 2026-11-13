# Proof of Concepts

Een proof of concept is een combinatie van minimale hard- en software om eenvoudig aan te tonen dat iets werkt.  
Proof of concepts kunnen enerzijds als eerste poging dienen om een bepaald onderdeel van het project werkend te krijgen. Anderzijds kunnen ze tijdens het debuggen van het finale project dienen als test om eventuele hardware problemen te elimineren bij onduidelijke problemen.
<br />  
<br />
Een proof of concept bestaat uit
* een elektronisch schema (indien van toepassing)
* minimale code om de voorop gestelde zaken te demonstreren
* minimale uitleg / aandachtspunten
<br />
De elektronische schakelingen van de proof of concepts komen bij voorkeur overeen met deze van de uiteindelijke linefollower, zodat de code zonder aanpassingen op het finale project kan draaien.

## overzicht

| proof of concept | toont aan | code |
|---|---|---|
| [blink](blink/readme.md) | IDE, ESP32 board package, driver, upload en klok werken | [blink.ino](blink/blink/blink.ino) |
| [hbridge](hbridge/readme.md) | 2 motoren onafhankelijk, traploos, in beide richtingen | [hbridge.ino](hbridge/hbridge/hbridge.ino) |
| [sensor](sensor/readme.md) | 8 sensoren individueel uitlezen via UART, volledig bereik | [sensor.ino](sensor/sensor/sensor.ino) |
| [interrupt](interrupt/readme.md) | non-blocking start/stop via een hardware interrupt | [interrupt.ino](interrupt/interrupt/interrupt.ino) |
| [wireless communication](wireless%20communication/readme.md) | duplex communicatie met laptop / smartphone via WiFi | [wireless.ino](wireless%20communication/wireless/wireless.ino) |

Alle proof of concepts gebruiken **dezelfde pinnen als de finale robot** (zie [elektronisch schema](../../technische%20tekeningen/elektronisch/readme.md)). Ze kunnen dus zonder aanpassingen op de afgewerkte robot gebruikt worden om hardwarefouten op te sporen.

**Arduino IDE**: installeer het board package **esp32 by Espressif** (Bordenbeheer) en kies **Tools → Board → ESP32 Dev Module**. Voor `wireless` en de finale code heb je de bibliotheek **WebSockets** van Markus Sattler nodig (Bibliotheekbeheer). Alle sketches zijn getest op compileren met `arduino-cli compile -b esp32:esp32:esp32` (esp32 core 3.0.6).
