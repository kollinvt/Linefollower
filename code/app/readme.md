# LineFollower Control (web app)

Gebruiksvriendelijk bedieningsprogramma voor de pc, zonder installatie: [linefollower.html](linefollower.html).

* Werkt in **Google Chrome** en **Microsoft Edge** (Web Serial API) op Windows, macOS, Linux en ChromeOS.
* Verbindt via **bluetooth** (COM-poort van de HC-05, 9600 baud) of via **USB** (115200 baud).

### functies
* grote **START** / **STOP** knoppen
* kalibratie zwart / wit met één klik
* alle parameters (cycle, power, boost, diff, Kp, Ki, Kd, D filter, lost) aanpassen. Ze worden in het EEPROM van de robot bewaard.
* **Lees parameters** haalt de huidige waardes uit de robot op en vult de velden in
* **live positie** van de lijn onder de sensor (balk, rood = lijn kwijt)
* console voor eigen commando's (bv. `help`)

### gebruik
1. Koppel de HC-05 met Windows (pincode 1234), zie [gebruiksaanwijzing](../../gebruiksaanwijzing/readme.md#verbinding-maken).
2. Open `linefollower.html` in Chrome of Edge (dubbelklik, of sleep het bestand in de browser).
3. Kies baud **9600** (bluetooth) of **115200** (USB) en klik **Verbinden**. Kies de COM-poort van de robot.
4. Klik **Lees parameters**, pas waardes aan en klik **set** (of **Alles versturen**).
