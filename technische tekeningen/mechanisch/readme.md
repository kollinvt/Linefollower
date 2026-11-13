# mechanische tekening - plan B

* printout: [mechanisch_planB.pdf](mechanisch_planB.pdf)
* origineel bestand: [mechanisch_planB.svg](mechanisch_planB.svg) (gegenereerd door [../tools/genereer_tekeningen.js](../tools/genereer_tekeningen.js))

![mechanische layout](../../images/mechanisch_planB.png)

## layout

| onderdeel | positie (vanaf achterkant print, in mm) | waarom |
|---|---|---|
| wielas (2x N20 onder de print) | x = 25 | aandrijving achteraan, sensor zo ver mogelijk vooraan |
| LiPo 1S 50 x 60 mm (plat) | x = 30 .. 80 | zwaarste onderdeel **net voor de wielbasis**: stabiel en genoeg grip op de wielen |
| DRV8833, SW1, TP4056 | x = 2 .. 22 | **korte vermogendraden** tussen batterij, h-brug en motoren; USB-C laadpoort achteraan |
| ESP32 DevKit | x = 83 .. 135 | bovenop; USB en BOOT-knop (start/stop) bereikbaar |
| MT3608 | x = 83 .. 113 | naast de ESP32, korte 5V-draad |
| ball caster (onder) | x = 104 | draagt de voorkant |
| Yahboom 8-kanaals sensor (onder) | x = 125 (= **100 mm voor de wielas**) | richtlijn 8-12 cm: vroeg bijsturen bij hoge snelheid |

## maten

* print (chassis): 80 x 140 mm protoboard, 1,6 mm dik
* totale breedte over de wielen: ± 100 mm, dus **past onder de chronobrug** (12 cm breed)
* totale hoogte: ± 40 mm (hoogste punt = ESP32 op headers), dus **< 12 cm**
* sensorhoogte boven de baan: ± 3 mm (afstelbaar met M3 afstandhouders en ringetjes; zie datasheet sensor)
* wieldiameter 34 mm
* massa: ± 150 g (LiPo ± 45 g)

> De maten van de Yahboom-sensor en de exacte posities zijn een richtlijn. Meet je eigen robot na en pas de tekening aan in `genereer_tekeningen.js`.

## spoiler

De specificaties zeggen *"spoilers niet vergeten!!"* 😉. Achteraan kan je een kleine spoiler (3D print of stevig karton) op een staafje monteren. Hij mag niet hoger komen dan 12 cm.
