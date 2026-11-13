/*
 * Genereert de technische tekeningen (SVG) van de LineFollower plan B.
 *   node genereer_tekeningen.js
 * => ../elektronisch/schema_planB.svg
 * => ../mechanisch/mechanisch_planB.svg
 * PDF: open de SVG in Chrome/Edge en print naar PDF (of zie readme).
 */
const fs = require("fs");
const path = require("path");

/* ======================================================================= */
/* elektronisch schema                                                      */
/* ======================================================================= */
const W = 1400, H = 990;
let s = [];
const C = { line: "#7a0000", body: "#fffbe6", text: "#000080", net: "#006400", gnd: "#333" };

function text(x, y, t, o = {}) {
  s.push(`<text x="${x}" y="${y}" font-size="${o.size || 13}" fill="${o.fill || C.text}" font-family="Arial" text-anchor="${o.anchor || "start"}" font-weight="${o.bold ? "bold" : "normal"}">${t}</text>`);
}
function line(x1, y1, x2, y2, col = C.line, w = 1.6) {
  s.push(`<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${col}" stroke-width="${w}"/>`);
}
function dot(x, y) { s.push(`<circle cx="${x}" cy="${y}" r="3.5" fill="${C.line}"/>`); }
function gnd(x, y) {
  line(x, y, x, y + 12);
  line(x - 12, y + 12, x + 12, y + 12, C.gnd, 2);
  line(x - 7, y + 17, x + 7, y + 17, C.gnd, 2);
  line(x - 3, y + 22, x + 3, y + 22, C.gnd, 2);
}
function power(x, y, name) { // pijl omhoog met netnaam
  line(x, y, x, y - 14);
  line(x - 10, y - 14, x + 10, y - 14, C.line, 2);
  text(x, y - 20, name, { anchor: "middle", bold: true, fill: C.net });
}
function netLabel(x, y, name, dir) { // dir: 'l' of 'r' = kant waar het label staat
  const w = name.length * 8 + 12;
  const x0 = dir === "r" ? x : x - w;
  s.push(`<rect x="${x0}" y="${y - 10}" width="${w}" height="20" rx="3" fill="#eaf7ea" stroke="${C.net}"/>`);
  text(x0 + w / 2, y + 5, name, { anchor: "middle", fill: C.net, bold: true, size: 12 });
}
/* blok (breakout board) met pinnen links en rechts.
   pins: [naam, net] ; net = 'GND' => massasymbool, '+5V'/'VBAT' => label */
function block(x, y, w, title, sub, left, right) {
  const pitch = 26, h = Math.max(left.length, right.length) * pitch + 30;
  s.push(`<rect x="${x}" y="${y}" width="${w}" height="${h}" fill="${C.body}" stroke="${C.line}" stroke-width="2"/>`);
  text(x + w / 2, y - 22, title, { anchor: "middle", bold: true, size: 15 });
  text(x + w / 2, y - 6, sub, { anchor: "middle", size: 12, fill: "#555" });
  const pins = {};
  left.forEach(([n, net], i) => {
    const py = y + 28 + i * pitch;
    line(x - 40, py, x, py);
    text(x + 6, py + 4, n, { size: 12 });
    pins[n] = [x - 40, py];
    if (net) netLabel(x - 40, py, net, "l");
  });
  right.forEach(([n, net], i) => {
    const py = y + 28 + i * pitch;
    line(x + w, py, x + w + 40, py);
    text(x + w - 6, py + 4, n, { size: 12, anchor: "end" });
    pins[n] = [x + w + 40, py];
    if (net) netLabel(x + w + 40, py, net, "r");
  });
  return pins;
}
function resistor(x, y, vertical, name, val) {
  if (vertical) {
    s.push(`<rect x="${x - 7}" y="${y}" width="14" height="36" fill="#fff" stroke="${C.line}" stroke-width="1.6"/>`);
    text(x + 12, y + 16, name, { size: 12 }); text(x + 12, y + 30, val, { size: 12 });
  } else {
    s.push(`<rect x="${x}" y="${y - 7}" width="36" height="14" fill="#fff" stroke="${C.line}" stroke-width="1.6"/>`);
    text(x + 18, y - 12, `${name} ${val}`, { size: 12, anchor: "middle" });
  }
}
function capacitor(x, y, name, val, polar) { // verticaal, y = boven
  line(x, y, x, y + 16);
  line(x - 12, y + 16, x + 12, y + 16, C.line, 2.5);
  if (polar) s.push(`<path d="M ${x - 12} ${y + 26} Q ${x} ${y + 18} ${x + 12} ${y + 26}" fill="none" stroke="${C.line}" stroke-width="2.5"/>`);
  else line(x - 12, y + 22, x + 12, y + 22, C.line, 2.5);
  line(x, y + 22, x, y + 38);
  if (polar) text(x - 18, y + 14, "+", { size: 13 });
  text(x + 16, y + 16, name, { size: 12 }); text(x + 16, y + 30, val, { size: 12 });
}
function motor(x, y, name) { // cirkel, aansluitingen links (boven / onder)
  s.push(`<circle cx="${x}" cy="${y}" r="22" fill="#fff" stroke="${C.line}" stroke-width="2"/>`);
  text(x, y + 5, "M", { anchor: "middle", bold: true, size: 16 });
  text(x + 28, y + 5, name, { size: 12 });
}

s.push(`<rect x="0" y="0" width="${W}" height="${H}" fill="#fff"/>`);
s.push(`<rect x="10" y="10" width="${W - 20}" height="${H - 20}" fill="none" stroke="#880000" stroke-width="2"/>`);

/* --- voeding --------------------------------------------------------- */
text(40, 50, "VOEDING", { bold: true, size: 16, fill: "#880000" });
// batterij
const bx = 90, by = 110;
line(bx, by, bx, by + 20);
line(bx - 18, by + 20, bx + 18, by + 20, C.line, 3);
line(bx - 9, by + 28, bx + 9, by + 28, C.line, 3);
line(bx - 18, by + 36, bx + 18, by + 36, C.line, 3);
line(bx - 9, by + 44, bx + 9, by + 44, C.line, 3);
line(bx, by + 44, bx, by + 70);
text(bx + 26, by + 26, "BT1", { size: 12 });
text(bx + 26, by + 40, "2x 18650 Li-ion", { size: 12 });
text(bx + 26, by + 54, "7,4V (6..8,4V)", { size: 12 });
text(bx - 30, by + 18, "+", { size: 14 });
gnd(bx, by + 70);
// schakelaar
line(bx, by, bx, by - 20); line(bx, by - 20, 160, by - 20);
dot(160, by - 20);
s.push(`<line x1="160" y1="${by - 20}" x2="196" y2="${by - 34}" stroke="${C.line}" stroke-width="2"/>`);
dot(200, by - 20);
text(160, by - 44, "SW1 power on/off", { size: 12 });
line(200, by - 20, 330, by - 20);
// condensatoren
dot(240, by - 20); capacitor(240, by - 20, "C1", "100µF/16V", true); gnd(240, by + 18);
dot(330, by - 20); capacitor(330, by - 20, "C2", "100nF", false); gnd(330, by + 18);
line(330, by - 20, 380, by - 20);
netLabel(380, by - 20, "VBAT", "r");

// power led
text(470, 50, "POWER LED", { bold: true, size: 16, fill: "#880000" });
power(520, 100, "+5V");
line(520, 100, 520, 110);
resistor(520, 110, true, "R3", "1kΩ");
line(520, 146, 520, 160);
s.push(`<polygon points="508,160 532,160 520,180" fill="#fff" stroke="${C.line}" stroke-width="2"/>`);
line(508, 180, 532, 180, C.line, 2);
s.push(`<line x1="536" y1="166" x2="546" y2="158" stroke="${C.line}" stroke-width="1.5"/><line x1="538" y1="174" x2="548" y2="166" stroke="${C.line}" stroke-width="1.5"/>`);
text(550, 176, "D1 groen", { size: 12 });
gnd(520, 180);

// start/stop knop
text(700, 50, "START / STOP", { bold: true, size: 16, fill: "#880000" });
netLabel(720, 120, "D7_BTN", "l");
line(720, 120, 760, 120); dot(760, 120);
s.push(`<line x1="760" y1="106" x2="800" y2="106" stroke="${C.line}" stroke-width="2"/><line x1="780" y1="106" x2="780" y2="96" stroke="${C.line}" stroke-width="2"/>`);
dot(800, 120); line(800, 120, 830, 120); gnd(830, 120);
text(760, 150, "S1 drukknop (monostabiel)", { size: 12 });
text(760, 165, "interne pull-up µC", { size: 12, fill: "#555" });

// bluetooth spanningsdeler
text(990, 50, "LEVEL SHIFT BT RX", { bold: true, size: 16, fill: "#880000" });
netLabel(1000, 110, "TX1", "l");
line(1000, 110, 1030, 110);
resistor(1030, 110, false, "R1", "1kΩ");
line(1066, 110, 1130, 110); dot(1100, 110);
netLabel(1130, 110, "BT_RXD", "r");
line(1100, 110, 1100, 120);
resistor(1100, 120, true, "R2", "2kΩ");
line(1100, 156, 1100, 160); gnd(1100, 160);
text(990, 210, "5V → 3,3V (HC-05 RXD is 3,3V logica)", { size: 12, fill: "#555" });

line(30, 250, W - 30, 250, "#bbb", 1);

/* --- breakout boards ------------------------------------------------- */
const mcu = block(620, 330, 200, "U1  Arduino Micro", "ATmega32U4 - 5V - 16MHz", [
  ["VIN", "VBAT"], ["GND", "GND"], ["5V", "+5V"], ["D0/RX1", "BT_TXD"], ["D1/TX1", "TX1"],
  ["D4", "LEDON"], ["D7/INT6", "D7_BTN"], ["D13 (led)", ""],
], [
  ["A0", "S2"], ["A1", "S3"], ["A2", "S4"], ["A3", "S5"], ["A4", "S6"], ["A5", "S7"],
  ["D5 PWM", "AIN1"], ["D6 PWM", "AIN2"], ["D9 PWM", "BIN1"], ["D10 PWM", "BIN2"],
]);
text(720, 640, "led D13 on board = run indicator", { size: 12, fill: "#555", anchor: "middle" });

block(140, 330, 180, "U2  DRV8833", "dubbele h-brug (mosfet)", [
  ["VM", "VBAT"], ["GND", "GND"], ["nSLEEP", "+5V"], ["AIN1", "AIN1"], ["AIN2", "AIN2"],
  ["BIN1", "BIN1"], ["BIN2", "BIN2"],
], [
  ["AOUT1", ""], ["AOUT2", ""], ["BOUT1", ""], ["BOUT2", ""],
]);
// motoren aan U2 (AOUT1/2 -> M1, BOUT1/2 -> M2)
const m1y = 371, m2y = 470, mxp = 430;
line(360, 358, mxp, 358); line(mxp, 358, mxp, m1y - 22);
line(360, 384, 395, 384); line(395, 384, 395, 395); line(395, 395, mxp, 395); line(mxp, 395, mxp, m1y + 22);
line(360, 410, 380, 410); line(380, 410, 380, 425); line(380, 425, mxp, 425); line(mxp, 425, mxp, m2y - 22);
line(360, 436, 370, 436); line(370, 436, 370, 505); line(370, 505, mxp, 505); line(mxp, 505, mxp, m2y + 22);
motor(mxp, m1y, "M1 links");
motor(mxp, m2y, "M2 rechts");
text(140, 560, "M1, M2: N20 6V 1000rpm micro gear motor", { size: 12, fill: "#555" });

block(1040, 330, 200, "U3  QTR-8A", "analoge IR sensor array (8x QRE1113)", [
  ["VCC", "+5V"], ["GND", "GND"], ["LEDON", "LEDON"], ["OUT1", ""], ["OUT2", "S2"], ["OUT3", "S3"],
  ["OUT4", "S4"], ["OUT5", "S5"], ["OUT6", "S6"], ["OUT7", "S7"], ["OUT8", ""],
], []);
text(1140, 665, "OUT1 en OUT8 niet gebruikt (n.c.)", { size: 12, fill: "#555", anchor: "middle" });
text(1140, 681, "6 sensoren, afstand 9,525 mm", { size: 12, fill: "#555", anchor: "middle" });

block(140, 700, 180, "U4  HC-05", "bluetooth classic SPP", [
  ["VCC", "+5V"], ["GND", "GND"], ["TXD", "BT_TXD"], ["RXD", "BT_RXD"],
], [["STATE", ""], ["EN", ""]]);
text(140, 855, "standaard: 9600 baud, pin 1234", { size: 12, fill: "#555" });

/* --- titelblok -------------------------------------------------------- */
const tx = W - 430, ty = H - 150;
s.push(`<rect x="${tx}" y="${ty}" width="410" height="130" fill="#fff" stroke="#880000" stroke-width="2"/>`);
line(tx, ty + 40, tx + 410, ty + 40, "#880000", 1);
line(tx, ty + 75, tx + 410, ty + 75, "#880000", 1);
text(tx + 10, ty + 27, "LineFollower - plan B - elektronisch schema", { bold: true, size: 16 });
text(tx + 10, ty + 62, "Syntheseproject HOGENT - aansluitschema breakout boards", { size: 13 });
text(tx + 10, ty + 98, "Alle GND's zijn met elkaar verbonden (massasymbool).", { size: 12 });
text(tx + 10, ty + 116, "Nets met dezelfde naam zijn met elkaar verbonden.  rev 1.0", { size: 12 });

/* legende netten */
text(560, 720, "NETTEN", { bold: true, size: 14, fill: "#880000" });
const legend = [
  "VBAT  : batterij na SW1 → VIN (U1), VM (U2)",
  "+5V   : 5V regulator Arduino Micro → U2 nSLEEP, U3, U4, D1",
  "S2..S7: sensoren → analoge ingangen A0..A5",
  "AIN1/2, BIN1/2: PWM D5, D6, D9, D10 → h-brug",
  "BT_TXD: HC-05 TXD → D0 (RX1)",
  "TX1 → R1/R2 → BT_RXD (HC-05 RXD)",
  "D7_BTN: start/stop knop (INT6)",
  "LEDON: D4 → IR leds sensor array aan/uit",
];
legend.forEach((l, i) => text(560, 745 + i * 18, l, { size: 12, fill: "#333" }));

const schema = `<svg xmlns="http://www.w3.org/2000/svg" width="${W}" height="${H}" viewBox="0 0 ${W} ${H}">\n${s.join("\n")}\n</svg>\n`;
fs.writeFileSync(path.join(__dirname, "..", "elektronisch", "schema_planB.svg"), schema);

/* ======================================================================= */
/* mechanische tekening (schaal 1:1 in mm, x 4 px)                          */
/* ======================================================================= */
s = [];
const K = 4; // px per mm
const MW = 1400, MH = 990;
s.push(`<rect x="0" y="0" width="${MW}" height="${MH}" fill="#fff"/>`);
s.push(`<rect x="10" y="10" width="${MW - 20}" height="${MH - 20}" fill="none" stroke="#000" stroke-width="2"/>`);
const T = (x, y, t, o = {}) => s.push(`<text x="${x}" y="${y}" font-size="${o.size || 13}" font-family="Arial" fill="${o.fill || "#000"}" text-anchor="${o.anchor || "start"}" font-weight="${o.bold ? "bold" : "normal"}"${o.rot ? ` transform="rotate(${o.rot} ${x} ${y})"` : ""}>${t}</text>`);
const R = (x, y, w, h, fill, o = {}) => s.push(`<rect x="${x}" y="${y}" width="${w}" height="${h}" fill="${fill}" stroke="${o.stroke || "#000"}" stroke-width="${o.sw || 1.5}"${o.dash ? ' stroke-dasharray="6 4"' : ""}${o.rx ? ` rx="${o.rx}"` : ""}/>`);
const L = (x1, y1, x2, y2, o = {}) => s.push(`<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${o.col || "#000"}" stroke-width="${o.w || 1}"${o.dash ? ' stroke-dasharray="8 4 2 4"' : ""}/>`);
s.push(`<defs><marker id="a" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="#0040a0"/></marker></defs>`);
function dimH(x1, x2, y, label) {
  s.push(`<line x1="${x1}" y1="${y}" x2="${x2}" y2="${y}" stroke="#0040a0" stroke-width="1" marker-start="url(#a)" marker-end="url(#a)"/>`);
  T((x1 + x2) / 2, y - 5, label, { anchor: "middle", fill: "#0040a0", size: 12 });
}
function dimV(x, y1, y2, label) {
  s.push(`<line x1="${x}" y1="${y1}" x2="${x}" y2="${y2}" stroke="#0040a0" stroke-width="1" marker-start="url(#a)" marker-end="url(#a)"/>`);
  T(x - 6, (y1 + y2) / 2, label, { anchor: "middle", fill: "#0040a0", size: 12, rot: -90 });
}

/* bovenaanzicht: rijrichting = naar rechts.  x = lengte, y = breedte */
T(40, 50, "BOVENAANZICHT (schaal 1:2,5 op A4 landscape - maten in mm)", { bold: true, size: 15 });
const ox = 80, oy = 120; // linksboven van de printplaat
const LEN = 140, WID = 80;
const axleX = 25; // as op 25 mm van de achterkant
// printplaat
R(ox, oy, LEN * K, WID * K, "#e8f3e0", { rx: 6 });
T(ox + 330, oy + WID * K - 8, "protoboard 80 x 140 mm (chassis)", { size: 12, fill: "#2a6a2a" });
// wielen
const wheelD = 34, wheelW = 7;
R(ox + (axleX - wheelD / 2) * K, oy - (wheelW + 3) * K, wheelD * K, wheelW * K, "#333", { rx: 6 });
R(ox + (axleX - wheelD / 2) * K, oy + (WID + 3) * K, wheelD * K, wheelW * K, "#333", { rx: 6 });
// motoren (onder de print, gestippeld)
R(ox + (axleX - 6) * K, oy + 3 * K, 12 * K, 34 * K, "none", { dash: true, stroke: "#555" });
R(ox + (axleX - 6) * K, oy + (WID - 37) * K, 12 * K, 34 * K, "none", { dash: true, stroke: "#555" });
T(ox + (axleX + 8) * K, oy + 22 * K, "M1 (N20)", { size: 11, fill: "#555" });
T(ox + (axleX + 8) * K, oy + (WID - 18) * K, "M2 (N20)", { size: 11, fill: "#555" });
// as
L(ox + axleX * K, oy - 14 * K, ox + axleX * K, oy + (WID + 14) * K, { dash: true, col: "#a00" });
// batterijhouder 2x18650 (77 x 41) dwars, net voor de wielbasis
const batX = 32;
R(ox + batX * K, oy + (WID / 2 - 38.5) * K, 41 * K, 77 * K, "#cfe0ff", { rx: 4 });
T(ox + (batX + 20.5) * K, oy + (WID / 2) * K, "2x 18650", { anchor: "middle", size: 12 });
T(ox + (batX + 20.5) * K, oy + (WID / 2 + 5) * K, "batterijhouder", { anchor: "middle", size: 12 });
// DRV8833 bij de motoren
R(ox + 4 * K, oy + (WID / 2 - 9) * K, 18 * K, 18 * K, "#ffe0e0");
T(ox + 13 * K, oy + (WID / 2 + 1.5) * K, "U2", { anchor: "middle", size: 11 });
// Arduino Micro bovenop de batterij-zone? -> naast batterij achteraan op de print
R(ox + 4 * K, oy + 6 * K, 18 * K, 22 * K, "#fff2c0");
T(ox + 13 * K, oy + 18 * K, "SW1", { anchor: "middle", size: 11 });
// Arduino Micro (48 x 18) in de lengte, voor de batterij
R(ox + 78 * K, oy + 8 * K, 48 * K, 18 * K, "#fff2c0");
T(ox + 102 * K, oy + 19 * K, "U1 Arduino Micro", { anchor: "middle", size: 12 });
// HC-05 (27 x 13 + header)
R(ox + 78 * K, oy + 56 * K, 30 * K, 16 * K, "#e0f0ff");
T(ox + 93 * K, oy + 66 * K, "U4 HC-05", { anchor: "middle", size: 11 });
// knop + led
s.push(`<circle cx="${ox + 118 * K}" cy="${oy + 60 * K}" r="${3 * K}" fill="#d33" stroke="#000"/>`);
T(ox + 112 * K, oy + 61 * K, "S1", { anchor: "end", size: 11 });
s.push(`<circle cx="${ox + 118 * K}" cy="${oy + 71 * K}" r="${1.5 * K}" fill="#2c2" stroke="#000"/>`);
T(ox + 112 * K, oy + 72.5 * K, "D1", { anchor: "end", size: 11 });
// ball caster (onder de print)
s.push(`<circle cx="${ox + 104 * K}" cy="${oy + 40 * K}" r="${6 * K}" fill="none" stroke="#555" stroke-dasharray="5 3" stroke-width="1.5"/>`);
T(ox + 104 * K, oy + 51 * K, "ball caster (onder)", { anchor: "middle", size: 11, fill: "#555" });
// sensor array (75 x 13) onderaan vooraan, gestippeld
const sensX = axleX + 100;
R(ox + (sensX - 6.5) * K, oy + (WID / 2 - 37.5) * K, 13 * K, 75 * K, "none", { dash: true, stroke: "#0060c0", sw: 2 });
for (let i = 0; i < 6; i++) {
  const sy = WID / 2 + (i - 2.5) * 9.525;
  s.push(`<rect x="${ox + (sensX - 2) * K}" y="${oy + (sy - 1.5) * K}" width="${4 * K}" height="${3 * K}" fill="#0060c0"/>`);
}
T(ox + (sensX + 9) * K, oy + (WID / 2) * K, "U3 QTR-8A (onder)", { size: 11, fill: "#0060c0", rot: -90, anchor: "middle" });
// rijrichting
s.push(`<line x1="${ox + LEN * K + 30}" y1="${oy + WID / 2 * K}" x2="${ox + LEN * K + 110}" y2="${oy + WID / 2 * K}" stroke="#000" stroke-width="2" marker-end="url(#a)"/>`);
T(ox + LEN * K + 70, oy + WID / 2 * K - 10, "rijrichting", { anchor: "middle", size: 12 });
// maten
dimH(ox, ox + LEN * K, oy + (WID + 22) * K, "140");
dimH(ox + axleX * K, ox + sensX * K, oy + (WID + 30) * K, "100 (as → sensor)");
dimH(ox, ox + axleX * K, oy + (WID + 30) * K, "25");
dimV(ox - 20, oy, oy + WID * K, "80");
dimV(ox + LEN * K + 20, oy - (wheelW + 3) * K, oy + (WID + wheelW + 3) * K, "100 (totale breedte)");

/* zijaanzicht */
const sx = 80, groundY = 900;
T(40, 640, "ZIJAANZICHT", { bold: true, size: 15 });
L(sx - 30, groundY, sx + LEN * K + 60, groundY, { w: 2 });
// wiel
s.push(`<circle cx="${sx + axleX * K}" cy="${groundY - 17 * K}" r="${17 * K}" fill="#333"/>`);
// motor
R(sx + (axleX - 5) * K, groundY - 23 * K, 10 * K, 12 * K, "#999");
// print op 24 mm
const pcbY = groundY - 25.6 * K;
R(sx, pcbY, LEN * K, 1.6 * K, "#2a6a2a", { stroke: "#2a6a2a" });
// batterij (dwars => in zijaanzicht 41 lang, 21 hoog)
R(sx + batX * K, pcbY - 21 * K, 41 * K, 21 * K, "#cfe0ff", { rx: 6 });
// µC
R(sx + 78 * K, pcbY - 8 * K, 48 * K, 8 * K, "#fff2c0");
// caster
s.push(`<circle cx="${sx + 104 * K}" cy="${groundY - 6 * K}" r="${6 * K}" fill="#bbb" stroke="#000"/>`);
L(sx + 104 * K, groundY - 12 * K, sx + 104 * K, pcbY + 1.6 * K, { w: 3, col: "#777" });
// sensor op 3 mm van de grond via M3 afstandhouders
R(sx + (sensX - 6.5) * K, groundY - 4.6 * K, 13 * K, 1.6 * K, "#0060c0", { stroke: "#0060c0" });
L(sx + (sensX - 4) * K, groundY - 4.6 * K, sx + (sensX - 4) * K, pcbY + 1.6 * K, { w: 3, col: "#777" });
L(sx + (sensX + 4) * K, groundY - 4.6 * K, sx + (sensX + 4) * K, pcbY + 1.6 * K, { w: 3, col: "#777" });
// spoiler
s.push(`<polygon points="${sx - 2 * K},${pcbY - 34 * K} ${sx + 18 * K},${pcbY - 34 * K} ${sx + 18 * K},${pcbY - 31 * K} ${sx - 2 * K},${pcbY - 30 * K}" fill="#d33" stroke="#000"/>`);
L(sx + 6 * K, pcbY - 31 * K, sx + 6 * K, pcbY, { w: 2 });
T(sx + 22 * K, pcbY - 31 * K, "spoiler (3D print / karton, optioneel)", { size: 11, fill: "#a00" });
// maten zijaanzicht
dimV(sx + LEN * K + 30, groundY, groundY - 50 * K, "50 (&lt; 120 chronobrug)");
dimV(sx + (sensX + 12) * K, groundY, groundY - 3 * K, "");
T(sx + (sensX + 14) * K, groundY - 1 * K, "3 mm", { size: 11, fill: "#0040a0" });
dimV(sx - 20, groundY, groundY - 34 * K, "Ø34 wiel");

/* titelblok */
const mx = MW - 430, my = MH - 150;
s.push(`<rect x="${mx}" y="${my}" width="410" height="130" fill="#fff" stroke="#000" stroke-width="2"/>`);
L(mx, my + 40, mx + 410, my + 40); L(mx, my + 75, mx + 410, my + 75);
T(mx + 10, my + 27, "LineFollower - plan B - mechanische layout", { bold: true, size: 16 });
T(mx + 10, my + 62, "Syntheseproject HOGENT - maten in mm", { size: 13 });
T(mx + 10, my + 98, "massa ± 200 g - zwaartepunt net voor de wielas", { size: 12 });
T(mx + 10, my + 116, "afmetingen L x B x H: 140 x 100 x 50 mm   rev 1.0", { size: 12 });

const mech = `<svg xmlns="http://www.w3.org/2000/svg" width="${MW}" height="${MH}" viewBox="0 0 ${MW} ${MH}">\n${s.join("\n")}\n</svg>\n`;
fs.writeFileSync(path.join(__dirname, "..", "mechanisch", "mechanisch_planB.svg"), mech);
console.log("ok");
