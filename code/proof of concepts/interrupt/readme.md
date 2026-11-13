# start/stop interrupt proof of concept
minimale hard- en software die de correcte werking van een start/stop drukknop aantoont, gebruik makend van een hardware interrupt

### deelschema

```
 ESP32 DevKit
   GPIO0 ──── BOOT-knop (op het bord) ──── GND
   (interne pull-up actief: INPUT_PULLUP)
   GPIO2 ──── blauwe led op het bord (toestand run/stop)
```

Er is geen extra hardware nodig: de **BOOT-knop** van het DevKit is een monostabiele drukknop tussen GPIO0 en GND. Wil je een externe knop, sluit hem dan aan tussen een vrije GPIO (bv. GPIO4) en GND en pas `BUTTON_PIN` aan.

### code
[interrupt/interrupt.ino](interrupt/interrupt.ino)

* `attachInterrupt(digitalPinToInterrupt(0), onButton, FALLING)`: de ISR wordt opgeroepen op het moment dat de knop ingedrukt wordt (flank hoog → laag).
* De ISR staat in IRAM (`IRAM_ATTR`), zodat ze ook werkt als de flash-cache bezet is.
* Software debounce: drukken binnen 250 ms na de vorige worden genegeerd (contactdender).
* De variabelen die de ISR wijzigt zijn `volatile`.

### stappenplan
1. Upload de code en open de seriële monitor (115200 baud). De loop print om de 2 s `run` of `stop` en blokkeert expres met `delay(2000)`.
2. Druk op BOOT: de blauwe led schakelt **onmiddellijk**, ook al zit de loop op dat moment in de `delay`.

### resultaat
De robot reageert altijd meteen op de start/stop-knop, wat de rest van de code ook aan het doen is. In de finale code zet de ISR enkel een vlag en voert `loop()` de start/stop uit.

### opmerking
GPIO0 is een strapping pin: **niet indrukken tijdens het opstarten / resetten**, anders start de ESP32 in downloadmodus.
