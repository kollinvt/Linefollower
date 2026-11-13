# H-Bridge proof of concept

minimale hard- & software + stappenplan dat aantoont dat 2 motoren onafhankelijk van elkaar kunnen draaien, en (traploos) regelbaar zijn in snelheid en draairichting.

### deelschema

```
     LiPo 1S 3,7V
       +  ── SW1 ──┬── C1 100µF ──┬── GND
                   ├── C2 100nF ──┘
                   └── VM  ┌──────────┐
 ESP32 DevKit              │ DRV8833  │ AOUT1 ── M1 (links)
   3V3    ──────── nSLEEP  │          │ AOUT2 ──┘
   GPIO26 ──────── AIN1    │          │
   GPIO27 ──────── AIN2    │          │ BOUT1 ── M2 (rechts)
   GPIO25 ──────── BIN1    │          │ BOUT2 ──┘
   GPIO33 ──────── BIN2    │          │
   GND    ──────── GND     └──────────┘
```

Aansturing DRV8833 (per motor), pwm 20 kHz, 8 bit (0..255) via de LEDC-peripheral van de ESP32:

| IN1 | IN2 | motor |
|---|---|---|
| PWM | 0 | vooruit, snelheid ~ duty cycle |
| 0 | PWM | achteruit, snelheid ~ duty cycle |
| 0 | 0 | uitbollen (coast) |

### code
[hbridge/hbridge.ino](hbridge/hbridge.ino) (werkt met esp32 core 2.x en 3.x)

### stappenplan
1. Bouw het deelschema op. **Zet SW1 uit** voordat je iets verandert.
2. Upload de code, open de seriële monitor (115200 baud, *Newline*) en zet SW1 aan. Houd de robot omhoog.
3. `t` → automatische test: de linker motor gaat vooruit (0 → max → 0) en achteruit, daarna de rechter.
4. `l 120` / `l -120` / `r 200` / `r -60` → elke motor apart en traploos instellen. `s` = stop.
5. Controleer dat `l` met een positieve waarde de **linker** motor **vooruit** laat draaien. Zo niet: wissel de motordraden om (of zet `MOTOR_A_INVERT` / `MOTOR_B_INVERT` in `config.h` van de finale code).

### resultaat
* beide motoren draaien onafhankelijk van elkaar en in beide richtingen
* traploos regelbaar boven de dode zone: onder een bepaalde pwm staat de N20 stil (aanloopkoppel). Die waarde stel je in de finale code in als `runPwm` (*Min. PWM rijden*).
* 20 kHz pwm is onhoorbaar (geen piepende motoren)
