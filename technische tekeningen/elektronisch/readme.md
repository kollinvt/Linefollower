# elektronisch schema - plan B

* printout: [schema_planB.pdf](schema_planB.pdf)
* origineel bestand: [schema_planB.svg](schema_planB.svg). Het wordt gegenereerd door [../tools/genereer_tekeningen.js](../tools/genereer_tekeningen.js) (`node genereer_tekeningen.js`).
* `easyeda_linefollower_planB.json` is het voorbeeldschema uit de template (ter referentie).

![schema](../../images/schema_planB.png)

## aansluitingen

| van | naar | net |
|---|---|---|
| LiPo + (vóór SW1) | TP4056 B+ | BAT+ (opladen, ook als de robot uit staat) |
| LiPo + via SW1 | DRV8833 VM, MT3608 VIN+, C1, C2 | VBAT (3,0..4,2V) |
| LiPo - | alle GND's | GND |
| MT3608 VOUT+ (5,0V) | ESP32 VIN, sensor 5V, R1 + D1 | +5V |
| ESP32 3V3 | DRV8833 nSLEEP | +3V3 |
| ESP32 GPIO26 / GPIO27 | DRV8833 AIN1 / AIN2 | linker motor M1 |
| ESP32 GPIO25 / GPIO33 | DRV8833 BIN1 / BIN2 | rechter motor M2 |
| DRV8833 AOUT1 / AOUT2 | M1 | |
| DRV8833 BOUT1 / BOUT2 | M2 | |
| sensor TX | ESP32 GPIO16 (RX2) | SENS_TX |
| ESP32 GPIO17 (TX2) | sensor RX | SENS_RX |
| ESP32 GPIO0 | BOOT-knop → GND (op het DevKit) | BTN |

## aandachtspunten

* Alle GND's zijn fysiek met elkaar verbonden: LiPo, DRV8833, MT3608, ESP32 en sensor.
* Er zijn **3 positieve spanningen** die nooit met elkaar verbonden mogen worden:
  * **VBAT**: vermogen, rechtstreeks van de LiPo naar de h-brug en de step-up
  * **+5V**: van de MT3608 naar de ESP32 (VIN) en de sensor
  * **+3V3**: van de spanningsregelaar op het ESP32 DevKit (logica)
* De motoren worden **rechtstreeks uit de LiPo** gevoed via de DRV8833 (VM 2,7..10,8V → 1S is in orde), nooit uit de 5V of 3V3.
* **MT3608**: regel de uitgang af op **5,0V voordat** je de ESP32 aansluit. Een step-up levert standaard vaak meer dan 5V.
* **ESP32-pinnen zijn 3,3V-logica.** Controleer in de datasheet van de Yahboom-sensor dat zijn TX op 3,3V werkt. Zendt hij op 5V, plaats dan een spanningsdeler (bv. 1k/2k) tussen sensor-TX en GPIO16.
* **GPIO0** is een strapping pin: de knop mag niet ingedrukt zijn tijdens het opstarten (anders gaat de ESP32 in downloadmodus).
* GPIO25, 26, 27 en 33 zijn vrije uitgangen zonder bootfunctie, dus geschikt voor pwm.
* C1 (100 µF) en C2 (100 nF) over VBAT dempen de stroompieken van de motoren, zodat de ESP32 niet reset bij het optrekken (brown-out).

> **Aannames**: de voedingsopbouw (MT3608, TP4056, SW1) en de power led D1 zijn afgeleid. Controleer ze tegen je eigen robot en pas het schema aan als het anders gebouwd is.
