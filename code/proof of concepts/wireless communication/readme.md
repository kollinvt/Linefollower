# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een laptop (en smartphone), gebruik makend van een HC-05 bluetooth module en een serieel terminalprogramma (Arduino IDE seriële monitor, PuTTY, de [web app](../../app/readme.md) of "Serial Bluetooth Terminal" op Android)
<br />

### deelschema

```
 Arduino Micro                 HC-05
   5V  ─────────────────────── VCC
   GND ─────────────────────── GND
   D0 (RX1) ────────────────── TXD
   D1 (TX1) ── R1 1k ──┬────── RXD
                       R2 2k
                       │
                      GND
```

De spanningsdeler R1/R2 brengt het 5V TX-signaal naar ± 3,3V voor de HC-05.

### configuratie
De HC-05 werkt meteen met de standaardinstellingen: **9600 baud, 8N1, naam `HC-05`, pincode `1234`** (soms `0000`).

Optioneel een eigen naam geven (AT-modus):
1. Houd de knop op de HC-05 ingedrukt terwijl je de voeding aansluit: de led knippert traag (1x per 2 s).
2. Upload een doorgeefsketch met `Serial1.begin(38400)` (AT-modus = 38400 baud) en stuur, met *Both NL & CR*:
   * `AT` → `OK`
   * `AT+NAME=LineFollower` → `OK`
   * `AT+PSWD="1234"` → `OK`
3. Herstart de module (spanning eraf en terug).

### opmerkingen
* De ATmega32U4 heeft een **aparte hardware UART (Serial1)** naast de USB-poort (Serial). Je kan dus tegelijk via USB debuggen en via bluetooth communiceren.
* Bluetooth Classic (HC-05) is **niet** compatibel met BLE. iPhones ondersteunen geen SPP: gebruik een laptop of een Android-toestel.
* De code is non-blocking: `Serial1.available()` wordt in de loop gecontroleerd en er wordt nooit gewacht op data.

### gebruiksaanwijzing
1. Upload [wireless/wireless.ino](wireless/wireless.ino) en sluit de HC-05 aan. De led op de HC-05 knippert snel (= niet gekoppeld).
2. Windows: **Instellingen → Bluetooth → Apparaat toevoegen → HC-05**, pincode `1234`.
3. Zoek de COM-poort op: **Meer Bluetooth-opties → COM-poorten**. Neem de poort met richting **Uitgaand**.
4. Open die COM-poort in de seriële monitor op 9600 baud: elke seconde verschijnt `alive <n>`. De led op de HC-05 knippert nu 2x kort.
5. Typ `led on` / `led off`: de led D13 op de Arduino gaat aan / uit en je krijgt `echo: led on` terug → **duplex communicatie werkt**.
