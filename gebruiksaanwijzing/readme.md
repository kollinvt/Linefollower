# Gebruiksaanwijzing

### opladen / vervangen batterijen
* De robot werkt op **2x 18650 Li-ion** (in serie, 7,4V nominaal, 8,4V vol).
* **Zet SW1 uit**, neem de cellen uit de houder en laad ze op in een Li-ion lader (4,2V per cel). Laad nooit onbewaakt op.
* Plaats ze terug met de **juiste polariteit** (+ zoals aangeduid op de houder).
* Laad op wanneer een cel onder **3,3V** komt (± 6,6V totaal). Diepontladen (< 3,0V) beschadigt de cellen. De robot wordt merkbaar trager als de spanning onder ± 7V zakt, omdat `MAX_PWM` dan niet meer de volle 7V haalt.
* Gebruik altijd 2 identieke cellen met dezelfde laadtoestand.

### draadloze communicatie
#### verbinding maken
1. Zet de robot aan met SW1: de groene power led brandt en de led op de HC-05 knippert snel.
2. Windows: **Instellingen → Bluetooth en apparaten → Apparaat toevoegen → Bluetooth → HC-05** (of *LineFollower*), pincode **1234**.
3. Zoek de COM-poort op: **Meer Bluetooth-opties → COM-poorten**, de poort met richting **Uitgaand**.
4. Open de [web app](../code/app/readme.md) (`code/app/linefollower.html`) in Chrome / Edge, kies **9600** en klik **Verbinden**.
   Alternatief: Arduino seriële monitor of PuTTY op die COM-poort, 9600 baud, *Both NL & CR*. Op Android werkt de app *Serial Bluetooth Terminal*.
5. Bij een geslaagde verbinding knippert de led op de HC-05 2x kort om de 2 s.

Via USB werkt alles ook (115200 baud), handig tijdens het ontwikkelen.

#### commando's
debug  → toont alle parameters, de rekentijd en de rondetijd  
debug [on/off]  → live positie / output / motorvermogen (4x per s)  
start  
stop  
set cycle [µs]  → cyclustijd, standaard 2500  
set power [0..255]  → basissnelheid  
set boost [0..255]  → extra snelheid op rechte stukken  
set diff [0..1]  → verdeling van de stuuractie over de motoren  
set kp [0..]  
set ki [0..]  → in 1/s, wordt intern herrekend naar de cyclustijd  
set kd [0..]  → in s, wordt intern herrekend naar de cyclustijd  
set dfilter [0..0.95]  → laagdoorlaatfilter op de D-term  
set lost [0..1000]  → drempel "lijn kwijt" (genormaliseerd)  
calibrate black  
calibrate white  
sensors  → ruwe en genormaliseerde sensorwaardes  
reset  → standaard parameters terugzetten  
help  

Alle instellingen worden meteen in het **EEPROM** bewaard. De robot start altijd op met de laatst ingestelde waardes.

### kalibratie
Kalibreer bij elke nieuwe baan of andere lichtomstandigheden:
1. Robot staat stil (`stop`).
2. Plaats de robot zo dat **alle 6 sensoren boven zwart** zijn (bv. dwars op de lijn op een stuk zwart tape, of op een zwart vlak van de testbaan) → `calibrate black`.
3. Plaats de robot met **alle sensoren boven wit papier** → `calibrate white`.
4. Controle: zet de robot met de lijn onder het midden en stuur `sensors`. De middelste genormaliseerde waardes moeten bijna 0 zijn, de buitenste bijna 1000.
5. Met `debug on` zie je de positie. Schuif de robot dwars over de lijn: de positie loopt vloeiend van ± -24 tot +24 mm.

### settings
Startwaardes (na `reset`):

| parameter | waarde |
|---|---|
| cycle | 2500 µs |
| power | 120 |
| boost | 0 |
| diff | 0.5 |
| Kp | 8 |
| Ki | 0 |
| Kd | 0.25 |
| dfilter | 0.5 |
| lost | 700 |

De robot rijdt stabiel met volgende parameters:  
*(in te vullen na het tunen, bv. power 150, boost 60, diff 0.4, Kp 12, Ki 0, Kd 0.35)*

### tunen (snel en betrouwbaar)
Werk in deze volgorde. Prioriteit 1 = **3 toeren zonder fouten**, daarna pas sneller.
1. `set kp 0`, `set ki 0`, `set kd 0`, `set power 80`, `start` → beide wielen moeten vooruit draaien. Draait een wiel achteruit? Wissel dan de draden van die motor om.
2. `set kp 5`, zet de robot op de baan, `start`.
   * de robot rijdt van de lijn **weg** → wissel de draden van **beide** motoren om én verwissel de motoren links/rechts (of `set kp -5`)
   * te traag of valt stil → power omhoog
   * stuurt te weinig bij → Kp omhoog
   * waggelt over de lijn → Kp omlaag
3. Verhoog **Kd** (0.1 → 0.5) tot het waggelen verdwijnt. Wordt de robot schokkerig? Zet `dfilter` dan hoger (0.6 - 0.8).
4. Verhoog nu stap voor stap **power** (+10) en telkens **Kp** en **Kd** mee, tot de robot fouten maakt. Zet hem daarna 1 stap terug.
5. **diff**: versnelt in de bochten → diff omlaag. Valt stil in de bochten → diff omhoog.
6. Voeg **boost** toe (bv. 40 - 80) voor extra snelheid op de rechte stukken.
7. **Ki** blijft meestal 0. Gebruik hem alleen als de robot in lange bochten systematisch naar buiten drijft (Ki 0.5 - 2).
8. Meet objectief met de rondetijd (`debug` → `lap time`) en noteer elke test in de tabel in de [readme](../readme.md#testresultaten).

### start/stop button
* **S1** is de rode drukknop **vooraan rechts** op de robot.
* Eén druk = start, nog een druk = stop. Hij werkt via een hardware interrupt en reageert dus meteen, ook als de robot draadloos gestart werd.
* De led D13 op de Arduino brandt zolang de robot rijdt.
* **SW1** (schuifschakelaar achteraan links) = power on/off. De groene led D1 brandt als de robot aan staat.
