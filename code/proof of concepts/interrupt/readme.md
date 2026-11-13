# start/stop interrupt proof of concept
minimale hard- en software die de correcte werking van een start/stop drukknop aantoont, gebruik makend van een hardware interrupt

### deelschema

```
 Arduino Micro
   D7 (INT6) ──── S1 drukknop ──── GND
   (interne pull-up actief: INPUT_PULLUP)
   D13 ── on board led (toestand run/stop)
```

### code
[interrupt/interrupt.ino](interrupt/interrupt.ino)

* `attachInterrupt(digitalPinToInterrupt(7), onButton, FALLING)`: de ISR wordt opgeroepen op het moment dat de knop ingedrukt wordt (flank hoog → laag).
* Software debounce: drukken binnen 250 ms na de vorige druk worden genegeerd (contactdender).
* De variabelen die in de ISR gewijzigd worden zijn `volatile`.

### stappenplan
1. Bouw het deelschema op en upload de code.
2. Open de seriële monitor (115200 baud). De loop print om de 2 s `run` of `stop` en blokkeert expres met `delay(2000)`.
3. Druk op de knop: de led D13 schakelt **onmiddellijk**, ook al zit de loop op dat moment in de `delay`.

### resultaat
De robot reageert dus altijd meteen op de start/stop knop, wat de rest van de code ook aan het doen is.
