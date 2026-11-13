## bill of materials

Volledige lijst van alle onderdelen om de plan B linefollower na te bouwen. Prijzen zijn richtprijzen (november 2026, incl. btw). De links zijn zoeklinks bij de webshop: kies het product dat overeenkomt met de omschrijving. De LiPo-batterij heeft een directe productlink.

Ook beschikbaar als [bom.csv](bom.csv) (openen in Excel: scheidingsteken `;`).

<br />

|volgnummer|naam|omschrijving|nieuw/recup|kostprijs/stuk|aantal|subtotaal|aankoopbron|
|----------|----|------------|-----------|--------------|------|---------|-----------|
|1|U1|ESP32 DevKit V1 (ESP32-WROOM-32, 30 pins, USB CP2102/CH340)|nieuw|€ 8,00|1|€ 8,00|[link](https://www.tinytronics.nl/nl/zoeken?search=esp32+devkit)|
|2|U3|Yahboom 8-kanaals infrarood lijnsensor (UART / I2C / analoog)|nieuw|€ 15,00|1|€ 15,00|[link](https://category.yahboom.net/search?q=8-channel+line+tracking)|
|3|U2|DRV8833 dubbele h-brug breakout|nieuw|€ 2,50|1|€ 2,50|[link](https://www.tinytronics.nl/nl/zoeken?search=drv8833)|
|4|M1 M2|N20 micro metal gear motor 6V|nieuw|€ 4,00|2|€ 8,00|[link](https://www.tinytronics.nl/nl/zoeken?search=n20+motor)|
|5||wiel 34 mm voor N20 (3 mm D-as) - set van 2|nieuw|€ 2,00|1|€ 2,00|[link](https://www.tinytronics.nl/nl/zoeken?search=n20+wiel)|
|6||N20 motorbeugel - set van 2|nieuw|€ 1,00|1|€ 1,00|[link](https://www.aliexpress.com/w/wholesale-n20-motor-bracket.html)|
|7|BT1|PKCELL Li-Po batterij 3,7V 2500 mAh JST-PH (LP785060)|nieuw|€ 9,00|1|€ 9,00|[link](https://www.tinytronics.nl/nl/power/batterijen/li-po/pkcell-li-po-batterij-3.7v-2500mah-jst-ph-lp785060)|
|8||JST-PH 2.0 connector met draad (batterij aansluiten)|nieuw|€ 0,50|1|€ 0,50|[link](https://www.tinytronics.nl/nl/zoeken?search=jst+ph)|
|9|U4|MT3608 step-up converter (3,7V naar 5V)|nieuw|€ 1,50|1|€ 1,50|[link](https://www.tinytronics.nl/nl/zoeken?search=mt3608)|
|10|U5|TP4056 Li-ion lader module USB-C met beveiliging|nieuw|€ 1,50|1|€ 1,50|[link](https://www.tinytronics.nl/nl/zoeken?search=tp4056)|
|11||protoboard / gaatjesprint (op maat 80 x 140 mm)|nieuw|€ 3,00|1|€ 3,00|[link](https://www.tinytronics.nl/nl/zoeken?search=experimenteer+print)|
|12||ball caster (metalen kogel 12 mm)|nieuw|€ 1,00|1|€ 1,00|[link](https://www.aliexpress.com/w/wholesale-ball-caster-cy-12a.html)|
|13|SW1|schuifschakelaar SS12D00 (power on/off)|nieuw|€ 0,30|1|€ 0,30|[link](https://www.tinytronics.nl/nl/zoeken?search=schuifschakelaar)|
|14|S1|BOOT-knop op het ESP32 DevKit (start/stop) - geen extra onderdeel|-|€ 0,00|1|€ 0,00|-|
|15|D1|led 3 mm groen (power on)|nieuw|€ 0,05|1|€ 0,05|[link](https://www.tinytronics.nl/nl/zoeken?search=led+3mm+groen)|
|16|R1|weerstand 1 kΩ 1/4W|nieuw|€ 0,05|1|€ 0,05|[link](https://www.tinytronics.nl/nl/zoeken?search=weerstand+1k)|
|17|C1|elco 100 µF 10V of hoger|nieuw|€ 0,10|1|€ 0,10|[link](https://www.tinytronics.nl/nl/zoeken?search=elco+100uf)|
|18|C2|keramische condensator 100 nF|nieuw|€ 0,05|1|€ 0,05|[link](https://www.tinytronics.nl/nl/zoeken?search=100nf)|
|19||female pin headers 1x40 (breakout boards inpluggen)|nieuw|€ 0,40|2|€ 0,80|[link](https://www.tinytronics.nl/nl/zoeken?search=female+header)|
|20||M3 afstandhouders 10 mm + schroeven (sensor + caster)|nieuw|€ 1,50|1|€ 1,50|[link](https://www.tinytronics.nl/nl/zoeken?search=m3+afstandhouder)|
|21||montagedraad 0,25 mm² (diverse kleuren) 1 m|nieuw|€ 1,00|1|€ 1,00|[link](https://www.tinytronics.nl/nl/zoeken?search=montagedraad)|
|  |  |**totaal**|  |  |  |**€ 56,85**|  |

### aandachtspunten

* **richtprijs ± 50 €**: het totaal ligt iets hoger, vooral door de Yahboom-sensor. Een goedkopere sensor-array (bv. QTR-8A-kloon ± € 5) brengt het onder de € 50, maar vraagt dan analoge ingangen en andere code.
* **bestel op tijd** en neem reserve-exemplaren van kwetsbare onderdelen (N20-motor, DRV8833, MT3608).
* **LiPo**: laad nooit onbewaakt op en ontlaad niet onder 3,0V. De PKCELL heeft een ingebouwde beveiligingsprint.
* **MT3608**: regel de uitgang **vóór** het aansluiten van de ESP32 af op 5,0V (potmeter, meten met de multimeter).
* **N20 op 1S (3,7V)**: een 6V-motor draait op 3,7..4,2V ongeveer 60-70% van zijn nominaal toerental. Voor snelheid kies je dus best de versie met het hoogste toerental (bv. 1000 rpm).
