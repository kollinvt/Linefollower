## bill of materials

Volledige lijst van alle onderdelen om de plan B linefollower na te bouwen. Prijzen zijn richtprijzen (november 2026, incl. btw, AliExpress / webshops). De links zijn zoeklinks: kies een aanbieder met goede reviews. Bij de belangrijkste onderdelen staat ook een link naar het originele product.

Ook beschikbaar als [bom.csv](bom.csv) (openen in Excel: scheidingsteken `;`).

<br />

|volgnummer|naam|omschrijving|nieuw/recup|kostprijs/stuk|aantal|subtotaal|aankoopbron|
|----------|----|------------|-----------|--------------|------|---------|-----------|
|1|U1|Arduino Micro (ATmega32U4) of compatibele kloon|nieuw|€ 9,00|1|€ 9,00|[link](https://www.aliexpress.com/w/wholesale-arduino-micro.html) (origineel: [link](https://store.arduino.cc/products/arduino-micro))|
|2|U3|QTR-8A analoge sensor array (of kloon HY-S301)|nieuw|€ 4,50|1|€ 4,50|[link](https://www.aliexpress.com/w/wholesale-qtr-8a.html) (origineel: [link](https://www.pololu.com/product/960))|
|3|U2|DRV8833 dubbele h-brug breakout|nieuw|€ 2,50|1|€ 2,50|[link](https://www.aliexpress.com/w/wholesale-drv8833-module.html) (origineel: [link](https://www.pololu.com/product/2130))|
|4|M1 M2|N20 micro metal gear motor 6V 1000 rpm|nieuw|€ 3,50|2|€ 7,00|[link](https://www.aliexpress.com/w/wholesale-n20-gear-motor-1000rpm.html)|
|5||wiel 34 mm voor N20 (3 mm D-as) - set van 2|nieuw|€ 2,00|1|€ 2,00|[link](https://www.aliexpress.com/w/wholesale-n20-wheel-34mm.html)|
|6||N20 motorbeugel - set van 2|nieuw|€ 1,00|1|€ 1,00|[link](https://www.aliexpress.com/w/wholesale-n20-motor-bracket.html)|
|7|U4|HC-05 bluetooth module (met breakout)|nieuw|€ 4,50|1|€ 4,50|[link](https://www.aliexpress.com/w/wholesale-hc-05.html)|
|8|BT1|18650 Li-ion cel 3,7V (ongeveer 2500 mAh) - flat top|recup/nieuw|€ 4,00|2|€ 8,00|[link](https://www.aliexpress.com/w/wholesale-18650-battery.html)|
|9|BT1|batterijhouder 2x 18650 in serie met draden|nieuw|€ 1,50|1|€ 1,50|[link](https://www.aliexpress.com/w/wholesale-2x18650-battery-holder.html)|
|10||protoboard / gaatjesprint 10 x 16 cm (op maat 80 x 140 mm)|nieuw|€ 3,00|1|€ 3,00|[link](https://www.aliexpress.com/w/wholesale-prototype-pcb-10x16.html)|
|11||ball caster (metalen kogel 12 mm, CY-12A of gelijkaardig)|nieuw|€ 1,00|1|€ 1,00|[link](https://www.aliexpress.com/w/wholesale-ball-caster-cy-12a.html)|
|12|SW1|schuifschakelaar SS12D00 (power on/off)|nieuw|€ 0,30|1|€ 0,30|[link](https://www.aliexpress.com/w/wholesale-ss12d00.html)|
|13|S1|drukknop 6x6 mm (start/stop, monostabiel)|nieuw|€ 0,10|1|€ 0,10|[link](https://www.aliexpress.com/w/wholesale-tact-switch-6x6.html)|
|14|D1|led 3 mm groen (power on)|nieuw|€ 0,05|1|€ 0,05|[link](https://www.aliexpress.com/w/wholesale-3mm-led.html)|
|15|R1 R3|weerstand 1 kΩ 1/4W|nieuw|€ 0,05|2|€ 0,10|[link](https://www.aliexpress.com/w/wholesale-resistor-kit.html)|
|16|R2|weerstand 2 kΩ 1/4W (of 2x 1 kΩ in serie)|nieuw|€ 0,05|1|€ 0,05|[link](https://www.aliexpress.com/w/wholesale-resistor-kit.html)|
|17|C1|elco 100 µF 16V|nieuw|€ 0,10|1|€ 0,10|[link](https://www.aliexpress.com/w/wholesale-electrolytic-capacitor-kit.html)|
|18|C2|keramische condensator 100 nF|nieuw|€ 0,05|1|€ 0,05|[link](https://www.aliexpress.com/w/wholesale-ceramic-capacitor-kit.html)|
|19||female pin headers 1x40 (breakout boards inpluggen)|nieuw|€ 0,40|2|€ 0,80|[link](https://www.aliexpress.com/w/wholesale-female-header-2.54.html)|
|20||M3 afstandhouders 10 mm + schroeven (sensor + caster)|nieuw|€ 1,50|1|€ 1,50|[link](https://www.aliexpress.com/w/wholesale-m3-spacer-kit.html)|
|21||montagedraad 0,25 mm² (diverse kleuren) 1 m|nieuw|€ 1,00|1|€ 1,00|[link](https://www.aliexpress.com/w/wholesale-silicone-wire-26awg.html)|
|  |  |**totaal**|  |  |  |**€ 48,05**|  |

### aandachtspunten

* **bestel op tijd**: AliExpress levert in 1-3 weken. Bestel kwetsbare onderdelen dubbel (N20 motor, DRV8833, HC-05). Samen kost dat ± € 10 extra en spaart het veel tijd bij een defect.
* **18650 cellen**: neem 2 identieke cellen (zelfde merk en capaciteit) zonder beschermingsprint (flat top past beter in de houder). Recup uit een oude laptopbatterij kan ook. Meet dan wel eerst de spanning (> 3,0V).
* **N20 motoren**: kies de versie **6V 1000 rpm**. Lagere toerentallen (300-600 rpm) rijden betrouwbaarder maar trager. Met de 1000 rpm versie kunnen we de 1 m/s halen.
* **QTR-8A kloon (HY-S301)**: werkt identiek. Controleer de pinout op de print (VCC, GND, LEDON, OUT1..8).
* **Arduino Micro kloon**: controleer dat het een ATmega32U4 is (geen Pro Micro 3,3V versie).
* richtprijs volgens de specificaties: ± 50 € → **in orde**.
