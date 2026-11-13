datasheets of the components used in this project

| component | pdf in deze map | bron |
|---|---|---|
| ESP32 (chip) | [ESP32.pdf](ESP32.pdf) | [Espressif](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf) |
| ESP32-WROOM-32 (module) | [ESP32-WROOM-32.pdf](ESP32-WROOM-32.pdf) | [Espressif](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf) |
| ESP32 DevKit (referentieschema) | [ESP32-DevKitC_schema.pdf](ESP32-DevKitC_schema.pdf) | [Espressif](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf) |
| DRV8833 | [DRV8833.pdf](DRV8833.pdf) | [Texas Instruments](https://www.ti.com/lit/ds/symlink/drv8833.pdf) |
| MT3608 (step-up) | [MT3608.pdf](MT3608.pdf) | [Olimex](https://www.olimex.com/Products/Breadboarding/BB-PWR-3608/resources/MT3608.pdf) |
| TP4056 (lader) | [TP4056.pdf](TP4056.pdf) | [SparkFun](https://dlnmh9ip6v2uc.cloudfront.net/datasheets/Prototyping/TP4056.pdf) |
| Yahboom 8-kanaals lijnsensor | - | [Yahboom](https://category.yahboom.net/search?q=8-channel+line+tracking) (handleiding + UART-protocol op de productpagina) |
| PKCELL LiPo LP785060 | - | [TinyTronics productpagina](https://www.tinytronics.nl/nl/power/batterijen/li-po/pkcell-li-po-batterij-3.7v-2500mah-jst-ph-lp785060) |
| N20 gear motor | - | [Pololu micro metal gearmotors (N20 formaat)](https://www.pololu.com/category/60/micro-metal-gearmotors) |

### belangrijkste waardes (gecontroleerd voor dit ontwerp)

| component | parameter | waarde | in dit ontwerp |
|---|---|---|---|
| ESP32 | logica / GPIO | 3,3V, **niet 5V-tolerant** | sensor-TX moet 3,3V zijn (anders spanningsdeler) |
| ESP32 | strapping pins | GPIO0, 2, 5, 12, 15 | GPIO0 = BOOT-knop (niet indrukken bij opstarten), motoren op 25/26/27/33 ✔ |
| ESP32 | LEDC pwm | tot 40 MHz, 1..20 bit | 20 kHz, 8 bit ✔ |
| ESP32 DevKit | VIN / 5V | 5V via USB of pin | 5,0V van de MT3608 ✔ |
| ESP32 | stroom | ± 80 mA, pieken ± 250 mA (WiFi zenden) | MT3608 levert ruim genoeg ✔ |
| DRV8833 | VM | 2,7..10,8V | LiPo 3,0..4,2V ✔ |
| DRV8833 | stroom per kanaal | 1,5A RMS (2A piek) | N20 ✔ |
| DRV8833 | RDSon (high + low side) | 360 mΩ | weinig verlies, belangrijk bij 1S ✔ |
| MT3608 | VIN | 2..24V | 3,0..4,2V ✔ |
| MT3608 | max. schakelstroom | 2A (piek) | 5V-belasting ± 0,4A ✔ |
| TP4056 | laadstroom / eindspanning | 1A (instelbaar met Rprog) / 4,2V | 2500 mAh cel → 0,4C ✔ |
| LiPo LP785060 | spanning | 3,0..4,2V, 2500 mAh, met beveiliging | |
