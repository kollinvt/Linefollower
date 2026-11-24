# mechanische tekening - LineFollower DUO (2 sensoren achter elkaar)

* **3D-model (Fusion 360-script)**: [fusion_bodemplaat_duo/fusion_bodemplaat_duo.py](fusion_bodemplaat_duo/fusion_bodemplaat_duo.py). Het script bouwt de volledige bodemplaat in Fusion.
* printout: [mechanisch_planB.pdf](mechanisch_planB.pdf)
* tekening als bronbestand: [mechanisch_planB.svg](mechanisch_planB.svg) (gegenereerd door [../tools/genereer_tekeningen.js](../tools/genereer_tekeningen.js), met dezelfde maten als het Fusion-script)
* vorige versie (1 sensor, 100 x 96 mm): [robot.f3d](robot.f3d)

![mechanische layout](../../images/mechanisch_planB.png)

## waarom 2 sensoren achter elkaar?

Met één sensor zag de robot een bocht pas als hij er al in zat. Bij hoge snelheid remde hij te laat en vloog hij uit de bocht, vooral in bochten van 180°.

| sensor | positie | taak |
|---|---|---|
| **VOORAAN** (U6) | **120 mm** voor de wielas | **vooruitkijken**: ziet de bocht ± 50 mm eerder, zodat de robot afremt **voor** hij in de bocht zit |
| **STUREN** (U3) | **70 mm** voor de wielas | volgt de lijn nauwkeurig, als input voor de **PID-regelaar** |

Is de stuursensor de lijn kwijt maar ziet de sensor vooraan ze nog (S-bochten), dan stuurt de robot op de sensor vooraan.

## Fusion-script uitvoeren

1. Open Fusion 360 met een nieuw, leeg ontwerp (Design-werkruimte).
2. **Utilities → Scripts and Add-Ins** (Shift+S) → tabblad **Scripts** → **+** → kies de map `fusion_bodemplaat_duo`.
3. Selecteer `fusion_bodemplaat_duo` → **Run**. Er verschijnt een component *Bodemplaat DUO* met:
   * de bodemplaat (4 mm) met alle M3-gaten
   * een gleuf in beide sensorbalken
   * een schets *onderdelen* met de posities van de ESP32, DRV8833, MT3608, beide sensoren, de LiPo en de wielas
4. **STL voor de 3D-printer**: rechtsklik op de body *bodemplaat* → **Save as Mesh** → STL.

Wil je de maten aanpassen (bv. de sensor nog verder vooraan)? Wijzig dan de lijsten `OUTLINE`, `HOLES` en `PARTS` bovenaan in het script.

## bodemplaat (3D print, 4 mm)

| zone (vanaf de voorkant, in mm) | breedte | functie |
|---|---|---|
| 0 .. 20 | 48 | **sensorbalk VOORAAN** met gleuf, sensor U6 eronder |
| 20 .. 50 | 30 | **smalle hals** (licht), **ball caster** eronder op X = 35 |
| 50 .. 70 | 48 | **sensorbalk STUREN** met gleuf, sensor U3 eronder |
| 70 .. 90 | 48 → 96 | schuine overgang |
| 90 .. 110 | **96** | breedste deel, ESP32 DevKit (29 x 52) |
| 110 .. 150 | 60 | achterstuk met **uitsparingen voor de wielen**, DRV8833 en MT3608 bovenop, N20-motoren eronder |

## layout

| onderdeel | positie (X vanaf de voorkant, mm) | waarom |
|---|---|---|
| sensor VOORAAN (onder) | 10 | 120 mm voor de wielas: bocht vroeg zien |
| ball caster (onder) | 35 | ondersteunt de lange neus |
| sensor STUREN (onder) | 60 | 70 mm voor de wielas: nauwkeurig sturen |
| LiPo 50 x 60 (onder) | 75 .. 125 | zwaartepunt tussen de sensoren en de wielen |
| ESP32 DevKit | 77 .. 106 | midden, USB en BOOT-knop (start/stop) bereikbaar |
| DRV8833 + MT3608 | 111 .. 133 | dicht bij de motoren: **korte vermogendraden** |
| wielas (2x N20 onder de plaat) | 130 | wielen in de uitsparingen, dus de robot is niet breder dan 96 mm |

## maten

* L x B x H: **150 x 96 x ± 41 mm**, dus **past onder de chronobrug** (12 x 12 cm)
* wielas → sensor VOORAAN: **120 mm** (richtlijn leerpad: 8-12 cm)
* wielas → sensor STUREN: **70 mm**
* sensorhoogte boven de baan: ± 3 mm (afstelbaar met afstandhouders)
* wieldiameter 34 mm
* massa: ± 170 g

## printinstellingen (richtwaarde)

* PETG (stijver dan PLA, beter voor de lange neus) of PLA, laaghoogte 0,2 mm, 3-4 wanden, 30 % infill
* plat printen, geen support nodig. De plaat is 150 mm lang en past op elke gangbare printer.
* gaten Ø3,2 mm (M3): na het printen eventueel ruimen met een boortje van 3 mm

## spoiler

De specificaties zeggen *"spoilers niet vergeten!!"* 😉. Achteraan kan je een kleine geprinte spoiler monteren. Hij mag niet hoger komen dan 12 cm.
