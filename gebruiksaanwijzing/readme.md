# Gebruiksaanwijzing

### opladen / vervangen batterijen
* De robot werkt op een **LiPo 1S 3,7V 2500 mAh** (PKCELL LP785060, JST-PH stekker).
* **Opladen**: steek een USB-C kabel in de TP4056-lader achteraan op de robot. Rode led = bezig met laden, blauwe/groene led = vol (4,2V). Dat kan met SW1 aan of uit. Laad nooit onbewaakt op.
* **Vervangen**: zet SW1 uit, trek de JST-PH stekker voorzichtig uit (aan de stekker, niet aan de draden) en steek een nieuwe cel in. Let op de **polariteit**: rood = +.
* Laad op wanneer de robot merkbaar trager wordt of als de cel onder **3,5V** komt. Onder 3,0V schakelt de beveiligingsprint de batterij af.
* Een volle batterij (4,2V) rijdt sneller dan een halfvolle (3,7V). Tune en meet daarom altijd met een **volle** batterij.

### draadloze communicatie
#### verbinding maken
1. Zet de robot aan met SW1: de groene power led brandt.
2. Verbind laptop of smartphone met de WiFi **LineFollower**, wachtwoord **12345678**.
3. Surf naar **http://192.168.4.1**. Rechtsboven verschijnt *verbonden* (groen).
4. Je laptop / gsm heeft zolang geen internet. Dat is normaal (de robot is zelf het access point).

#### de webpagina
* **START** / **STOP**: robot rijden of stoppen. De **spatiebalk** op de laptop = STOP (noodstop).
* **live data** (20x per seconde): staafjes met de lijnsterkte per sensor, de fout (blauwe stip), het motorvermogen links/rechts, *lijn kwijt* / *kruispunt*, sensorframes per seconde en de rekentijd per frame.
* **Kalibratie**: *Start kalibratie* / *Stop kalibratie* (zie hieronder).
* **parameters** per groep (PID, Snelheid, Motoren, Lijn): pas aan met de knoppen of door een waarde te typen. Ze worden **meteen** toegepast, ook tijdens het rijden.
* **Opslaan in robot**: schrijft alle parameters naar het flashgeheugen. Dat kan alleen als de robot stilstaat. Na een herstart begint de robot met deze waardes.
* **Standaard**: zet alle parameters terug op de standaardwaardes (nog niet opgeslagen).

#### commando's (websocket)
De webpagina stuurt deze tekstcommando's. Ze werken ook vanuit een eigen websocket-client (`ws://192.168.4.1:81`):

run  
stop  
cal1  → start kalibratie  
cal0  → stop kalibratie en sla op  
save  → parameters opslaan in flash  
def  → standaardwaardes laden  
set [parameter] [waarde]  → bv. `set kp 70`, `set vMax 150`

### kalibratie
Kalibreer bij elke nieuwe baan of andere lichtomstandigheden:
1. Robot stilzetten (STOP) en op de baan plaatsen.
2. Klik **Start kalibratie**.
3. Schuif de robot met de hand **langzaam heen en weer over de zwarte lijn**, zodat **elke** sensor minstens één keer boven zwart en boven wit papier komt.
4. Klik **Stop kalibratie**. De robot meldt *Kalibratie opgeslagen*. Krijg je *Kalibratie onvolledig*, dan zag een sensor te weinig verschil (< 200). Herhaal dan en beweeg verder.
5. Controle: zet de lijn onder het midden van de sensor. De middelste staafjes moeten hoog staan, de buitenste laag, en de fout moet rond 0 liggen.

De kalibratie wordt apart in flash bewaard en blijft behouden na een herstart.

### settings
Standaardwaardes (knop **Standaard**):

| groep | parameter | standaard | betekenis |
|---|---|---|---|
| PID | Kp | 67 | proportioneel (fout -7..+7) |
| PID | Ki | 0 | integrerend |
| PID | Kd | 80 | differentiërend (per frame) |
| PID | D-filter | 0.6 | 1 = geen filter, lager = meer filtering |
| PID | I-max | 200 | begrenzing integrator (anti windup) |
| Snelheid | Snelheid rechtdoor (vMax) | 120 | 0..255 |
| Snelheid | Min. snelheid in bocht (vMin) | 0 | |
| Snelheid | Bochtafremming (curve) | 70 | hoeveel trager per eenheid fout |
| Snelheid | Afremmen vanaf fout (curveSt) | 1.0 | onder deze fout geen afremming |
| Snelheid | Optrekken per frame (accel) | 6 | zacht vertrekken, geen slip |
| Motoren | Min. PWM rijden (runPwm) | 160 | dode zone N20 |
| Motoren | Startpuls PWM / ms | 220 / 15 | korte kick bij vertrek |
| Motoren | Max. achteruit | 255 | 0 = wielen nooit achteruit |
| Motoren | Trim links / rechts | 1.00 / 1.00 | verschil tussen motoren corrigeren |
| Lijn | Fout als lijn kwijt | 9 | stuurkracht bij lijn kwijt |
| Lijn | Kruispunt: min. sensoren / ms | 6 / 60 | kruispuntdetectie |
| Lijn | Ruisdrempel sensor | 0.15 | |
| Lijn | Lijn geeft hoge waarde | 0 | aanzetten als zwart een hoge ruwe waarde geeft |
| Lijn | Sensor links/rechts om | 0 | sensor omgekeerd gemonteerd |
| Lijn | Stuurrichting omkeren | 0 | robot stuurt van de lijn weg |

De robot rijdt stabiel met volgende parameters:  
*(in te vullen na het tunen)*

### tunen (snel en betrouwbaar)
Werk in deze volgorde. Prioriteit 1 = **3 toeren zonder fouten**, daarna pas sneller. Klik na een goede instelling telkens **Opslaan in robot**.
1. Kalibreer. Controleer op de webpagina dat de fout negatief is als de lijn links ligt en positief als hij rechts ligt. Is het omgekeerd? Zet dan **Sensor links/rechts om** aan.
2. Zet **Kp** laag (bv. 30), **Kd** 0, **vMax** 80 en druk START.
   * beide wielen moeten vooruit draaien. Een wiel draait achteruit? Zet `MOTOR_A_INVERT` / `MOTOR_B_INVERT` in `config.h` of wissel de motordraden om.
   * de robot stuurt van de lijn **weg** → zet **Stuurrichting omkeren** aan
   * vertrekt een wiel niet bij lage snelheid → verhoog **Min. PWM rijden**
3. Verhoog **Kp** tot de robot de lijn goed volgt maar licht begint te slingeren.
4. Verhoog **Kd** tot het slingeren verdwijnt. Wordt de robot schokkerig, verlaag dan **D-filter** (meer filtering).
5. Verhoog **vMax** stap voor stap (+10) en pas Kp en Kd telkens mee aan.
6. Vliegt de robot uit de bochten? Verhoog **Bochtafremming** of verlaag **Afremmen vanaf fout**. Is hij te traag in de bochten, doe dan het omgekeerde.
7. Trekt de robot scheef op een recht stuk? Gebruik **Trim links/rechts**.
8. **Ki** blijft meestal 0.
9. Noteer elke test (rondetijd, snelheid, parameters) in de tabel in de [readme](../readme.md#testresultaten).

### start/stop button
* **S1** is de **BOOT-knop** op het ESP32-bordje (naast de USB-poort, opschrift *BOOT* of *IO0*).
* Eén druk = start, nog een druk = stop. Hij werkt via een hardware interrupt en reageert dus meteen, ook als de robot via WiFi gestart werd.
* De **blauwe led** op de ESP32 brandt zolang de robot rijdt.
* **Niet indrukken tijdens het aanzetten**: dan start de ESP32 in downloadmodus. Zet de robot dan uit en weer aan.
* **SW1** (schuifschakelaar achteraan) = power on/off. De groene led D1 brandt als de robot aan staat.
