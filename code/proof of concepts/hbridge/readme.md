# H-Bridge proof of concept

minimale hard- & software + stappenplan dat aantoont dat 2 motoren onafhankelijk van elkaar kunnen draaien, en (traploos) regelbaar zijn in snelheid en draairichting.

### deelschema

```
            batterij 2x18650 (7,4V)
              +  ── SW1 ──┬── C1 100µF ──┬── GND
                          ├── C2 100nF ──┘
                          └── VM  ┌──────────┐
 Arduino Micro                    │ DRV8833  │ AOUT1 ── M1 (links)
   5V  ─────────────────── nSLEEP │          │ AOUT2 ──┘
   D5  ─────────────────── AIN1   │          │
   D6  ─────────────────── AIN2   │          │ BOUT1 ── M2 (rechts)
   D9  ─────────────────── BIN1   │          │ BOUT2 ──┘
   D10 ─────────────────── BIN2   │          │
   GND ─────────────────── GND    └──────────┘
   VIN ── VBAT (of USB voor de logica tijdens het testen)
```

Aansturing DRV8833 (per motor):

| IN1 | IN2 | motor |
|---|---|---|
| PWM | 0 | vooruit, snelheid ~ duty cycle |
| 0 | PWM | achteruit, snelheid ~ duty cycle |
| 0 | 0 | uitbollen (coast) |

### code
[hbridge/hbridge.ino](hbridge/hbridge.ino)

### stappenplan
1. Bouw het deelschema op. **Zet SW1 uit** voordat je iets verandert.
2. Upload de code, open de seriële monitor (115200 baud, *newline*), zet SW1 aan.
3. `t` → automatische test: eerst de linker motor vooruit (0 → max → 0) en achteruit, daarna de rechter motor.
4. `l 120` / `l -120` / `r 200` / `r -60` → elke motor apart en traploos instellen. `s` = stop.
5. Controleer dat `l` met een positieve waarde de **linker** motor **vooruit** laat draaien. Zo niet, wissel dan de 2 motordraden van die motor om.

### resultaat
* beide motoren draaien onafhankelijk van elkaar en in beide richtingen
* traploos regelbaar: onder een pwm van ± 40 staat de N20 stil (aanloopkoppel), daarboven neemt de snelheid geleidelijk toe
* de DRV8833 wordt niet warm (mosfets, RDSon 360 mΩ)
