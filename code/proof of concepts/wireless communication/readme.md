# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een laptop en smartphone, gebruik makend van WiFi (ingebouwd in de ESP32) en een webbrowser (websocket)
<br />

### configuratie
* De ESP32 werkt als **access point**: SSID `LineFollower-POC`, wachtwoord `12345678`, IP **192.168.4.1**
* webpagina op poort 80, websocket op poort 81
* bibliotheek **WebSockets** van Markus Sattler (via Bibliotheekbeheer)
* geen extra hardware: WiFi zit in de ESP32-WROOM-32 module

### opmerkingen
* Een access point werkt overal, ook in het labo zonder schoolnetwerk. Er is geen router nodig.
* Een websocket is een permanente verbinding in beide richtingen (**duplex**): de ESP32 kan zelf data pushen (telemetrie) zonder dat de browser telkens moet vragen.
* De code is non-blocking: `server.handleClient()` en `ws.loop()` worden in de loop aangeroepen en wachten nooit.
* De laptop of gsm verliest internet zolang hij met het access point van de robot verbonden is. Dat is normaal.

### gebruiksaanwijzing
1. Upload [wireless/wireless.ino](wireless/wireless.ino) en open de seriële monitor (115200): `AP klaar, surf naar http://192.168.4.1`.
2. Verbind laptop of smartphone met WiFi **LineFollower-POC** (wachtwoord `12345678`).
3. Surf naar **http://192.168.4.1**.
4. Elke seconde verschijnt `alive <n>` (ESP32 → pc).
5. Klik **led aan** / **led uit**: de blauwe led op de ESP32 gaat aan / uit (pc → ESP32) en je krijgt `echo: led on` terug (ESP32 → pc). Daarmee is de **duplex communicatie** aangetoond.
