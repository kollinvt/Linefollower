datasheets of the components used in this project

| component | pdf in deze map | bron |
|---|---|---|
| Arduino Micro | [Arduino_Micro_schema.pdf](Arduino_Micro_schema.pdf) (schema rev3) | [docs.arduino.cc/hardware/micro](https://docs.arduino.cc/hardware/micro/) |
| ATmega32U4 | [ATmega32U4.pdf](ATmega32U4.pdf) | [Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf) |
| DRV8833 | [DRV8833.pdf](DRV8833.pdf) | [Texas Instruments](https://www.ti.com/lit/ds/symlink/drv8833.pdf) |
| DRV8833 breakout | - | [Pololu DRV8833 carrier](https://www.pololu.com/product/2130) |
| QTR-8A | [QTR-8A_QTR-8RC.pdf](QTR-8A_QTR-8RC.pdf) | [Pololu QTR-8A/QTR-8RC user's guide](https://www.pololu.com/docs/0J12) |
| QRE1113 (sensor) | [QRE1113.pdf](QRE1113.pdf) | [onsemi](https://www.onsemi.com/pdf/datasheet/qre1113-d.pdf) |
| HC-05 | [HC-05.pdf](HC-05.pdf) | [components101](https://components101.com/sites/default/files/component_datasheet/HC-05%20Datasheet.pdf) |
| N20 gear motor | - | [Pololu micro metal gearmotors (N20 formaat)](https://www.pololu.com/category/60/micro-metal-gearmotors) |
| 18650 Li-ion | [Samsung_INR18650-25R.pdf](Samsung_INR18650-25R.pdf) | [PowerStream](https://www.powerstream.com/p/INR18650-25R-datasheet.pdf) |
| SS12D00 schuifschakelaar | - | typisch 0,5A 50V DC (generiek) |

### belangrijkste waardes (gecontroleerd voor dit ontwerp)

| component | parameter | waarde | in dit ontwerp |
|---|---|---|---|
| Arduino Micro | VIN | 7..12V aanbevolen | 6..8,4V (2S Li-ion): werkt, onder ± 6,5V wordt 5V minder stabiel |
| DRV8833 | VM | 2,7..10,8V | 8,4V max ✔ |
| DRV8833 | stroom per kanaal | 1,5A RMS (2A piek) | N20 stall ± 0,6..1A ✔ |
| DRV8833 | RDSon (high + low side) | 360 mΩ | verlies < 0,4V ✔ |
| QTR-8A | voeding | 3,3..5V, ± 100 mA | 5V ✔ |
| QTR-8A | optimale afstand | 3 mm (max. 6 mm) | 3 mm ✔ |
| HC-05 | voeding module | 3,6..6V | 5V ✔ |
| HC-05 | RXD logica | 3,3V | spanningsdeler 1k/2k ✔ |
| ATmega32U4 | ADC | 10 bit, ± 100 µs per conversie | 6 sensoren ± 0,7 ms ✔ |
