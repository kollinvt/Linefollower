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
// batterij 1S LiPo
const bx = 90, by = 110;
line(bx, by, bx, by + 20);
line(bx - 18, by + 20, bx + 18, by + 20, C.line, 3);
line(bx - 9, by + 28, bx + 9, by + 28, C.line, 3);
line(bx, by + 28, bx, by + 70);
text(bx + 30, by + 80, "BT1  LiPo 1S 3,7V 2500mAh", { size: 12 });
text(bx + 30, by + 94, "PKCELL LP785060 (JST-PH)", { size: 12 });
text(bx + 30, by + 108, "3,0..4,2V, met beveiligingsprint", { size: 12, fill: "#555" });
text(bx - 30, by + 18, "+", { size: 14 });
gnd(bx, by + 70);
// BAT+ (voor de schakelaar) naar de lader
line(bx, by, bx, by - 20); line(bx, by - 20, 160, by - 20);
dot(130, by - 20); line(130, by - 20, 130, by - 50); netLabel(130, by - 50, "BAT+", "l");
dot(160, by - 20);
s.push(`<line x1="160" y1="${by - 20}" x2="196" y2="${by - 34}" stroke="${C.line}" stroke-width="2"/>`);
dot(200, by - 20);
text(150, by - 44, "SW1 power on/off", { size: 12 });
line(200, by - 20, 330, by - 20);
dot(250, by - 20); capacitor(250, by - 20, "C1", "100µF/10V", true); gnd(250, by + 18);
dot(330, by - 20); capacitor(330, by - 20, "C2", "100nF", false); gnd(330, by + 18);
line(330, by - 20, 380, by - 20);
netLabel(380, by - 20, "VBAT", "r");

// power led
text(560, 50, "POWER LED", { bold: true, size: 16, fill: "#880000" });
power(600, 100, "+5V");
line(600, 100, 600, 110);
resistor(600, 110, true, "R1", "1kΩ");
line(600, 146, 600, 160);
s.push(`<polygon points="588,160 612,160 600,180" fill="#fff" stroke="${C.line}" stroke-width="2"/>`);
line(588, 180, 612, 180, C.line, 2);
s.push(`<line x1="616" y1="166" x2="626" y2="158" stroke="${C.line}" stroke-width="1.5"/><line x1="618" y1="174" x2="628" y2="166" stroke="${C.line}" stroke-width="1.5"/>`);
text(630, 176, "D1 groen", { size: 12 });
gnd(600, 180);

// start/stop knop
text(800, 50, "START / STOP", { bold: true, size: 16, fill: "#880000" });
netLabel(820, 120, "BTN", "l");
line(820, 120, 860, 120); dot(860, 120);
s.push(`<line x1="860" y1="106" x2="900" y2="106" stroke="${C.line}" stroke-width="2"/><line x1="880" y1="106" x2="880" y2="96" stroke="${C.line}" stroke-width="2"/>`);
dot(900, 120); line(900, 120, 930, 120); gnd(930, 120);
text(860, 150, "S1 = BOOT-knop op het ESP32 DevKit", { size: 12 });
text(860, 165, "(monostabiel, GPIO0, interne pull-up)", { size: 12, fill: "#555" });

text(1100, 50, "AANNAMES", { bold: true, size: 16, fill: "#880000" });
text(1100, 75, "voeding (MT3608, TP4056), SW1, D1:", { size: 12, fill: "#555" });
text(1100, 91, "controleer tegen je eigen opbouw", { size: 12, fill: "#555" });

line(30, 250, W - 30, 250, "#bbb", 1);

/* --- breakout boards ------------------------------------------------- */
block(620, 330, 200, "U1  ESP32 DevKit V1", "ESP32-WROOM-32 - 240 MHz - WiFi", [
  ["VIN (5V)", "+5V"], ["GND", "GND"], ["3V3", "+3V3"], ["GPIO0 (BOOT)", "BTN"], ["GPIO2 (led)", ""],
], [
  ["GPIO26", "AIN1"], ["GPIO27", "AIN2"], ["GPIO25", "BIN1"], ["GPIO33", "BIN2"],
  ["GPIO16 RX2", "SENS_TX"], ["GPIO17 TX2", "SENS_RX"],
  ["GPIO18 RX1", "SENS2_TX"], ["GPIO19 TX1", "SENS2_RX"],
]);
text(720, 590, "blauwe led GPIO2 on board = run indicator", { size: 12, fill: "#555", anchor: "middle" });

block(140, 330, 180, "U2  DRV8833", "dubbele h-brug (mosfet)", [
  ["VM", "VBAT"], ["GND", "GND"], ["nSLEEP", "+3V3"], ["AIN1", "AIN1"], ["AIN2", "AIN2"],
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
text(140, 560, "M1, M2: N20 6V micro gear motor", { size: 12, fill: "#555" });
text(140, 576, "pwm 20 kHz, fast decay", { size: 12, fill: "#555" });

block(1060, 330, 200, "U3  Yahboom 8-kanaals", "sensor STUREN (70 mm voor wielas)", [
  ["5V", "+5V"], ["GND", "GND"], ["TX", "SENS_TX"], ["RX", "SENS_RX"],
], []);
text(1160, 480, "UART 115200 - 8 kanalen", { size: 12, fill: "#555", anchor: "middle" });

block(1060, 560, 200, "U6  Yahboom 8-kanaals", "sensor VOORAAN (120 mm voor wielas)", [
  ["5V", "+5V"], ["GND", "GND"], ["TX", "SENS2_TX"], ["RX", "SENS2_RX"],
], []);
text(1160, 710, "vooruitkijken: bocht vroeg zien", { size: 12, fill: "#555", anchor: "middle" });

block(140, 700, 180, "U4  MT3608", "step-up 3,7V → 5V", [
  ["VIN+", "VBAT"], ["VIN-", "GND"],
], [["VOUT+", "+5V"], ["VOUT-", "GND"]]);
text(140, 800, "uitgang afregelen op 5,0V vóór aansluiten!", { size: 12, fill: "#a00" });

block(140, 860, 180, "U5  TP4056 (USB-C)", "Li-ion lader 1A + beveiliging", [
  ["IN USB-C", ""],
], [["B+", "BAT+"], ["B-", "GND"]]);

/* --- titelblok -------------------------------------------------------- */
const tx = W - 430, ty = H - 150;
s.push(`<rect x="${tx}" y="${ty}" width="410" height="130" fill="#fff" stroke="#880000" stroke-width="2"/>`);
line(tx, ty + 40, tx + 410, ty + 40, "#880000", 1);
line(tx, ty + 75, tx + 410, ty + 75, "#880000", 1);
text(tx + 10, ty + 27, "LineFollower - plan B - elektronisch schema", { bold: true, size: 16 });
text(tx + 10, ty + 62, "Syntheseproject HOGENT - DUO: 2 sensoren achter elkaar", { size: 13 });
text(tx + 10, ty + 98, "Alle GND's zijn met elkaar verbonden (massasymbool).", { size: 12 });
text(tx + 10, ty + 116, "Nets met dezelfde naam zijn met elkaar verbonden.  rev 3.0", { size: 12 });

/* legende netten */
text(560, 640, "NETTEN", { bold: true, size: 14, fill: "#880000" });
const legend = [
  "BAT+  : LiPo + (vóór SW1) → TP4056 B+ (opladen)",
  "VBAT  : LiPo na SW1 (3,0..4,2V) → DRV8833 VM, MT3608 VIN+",
  "+5V   : MT3608 → ESP32 VIN, beide sensoren 5V, power led",
  "+3V3  : 3,3V regulator ESP32 → DRV8833 nSLEEP",
  "AIN1/2, BIN1/2: GPIO26, 27, 25, 33 → h-brug (pwm 20 kHz)",
  "SENS_TX / SENS_RX: sensor STUREN ↔ GPIO16 / GPIO17",
  "SENS2_TX / SENS2_RX: sensor VOORAAN ↔ GPIO18 / GPIO19",
  "BTN   : GPIO0 (BOOT-knop) start/stop interrupt",
];
legend.forEach((l, i) => text(560, 665 + i * 18, l, { size: 12, fill: "#333" }));

const schema = `<svg xmlns="http://www.w3.org/2000/svg" width="${W}" height="${H}" viewBox="0 0 ${W} ${H}">\n${s.join("\n")}\n</svg>\n`;
fs.writeFileSync(path.join(__dirname, "..", "elektronisch", "schema_planB.svg"), schema);

/* ======================================================================= */
/* mechanische tekening - DUO: 3D-geprinte bodemplaat met 2 sensoren        */
/* robotcoördinaten in mm: X = afstand vanaf de voorkant (0 = voorkant      */
/* sensorbalk VOORAAN, 150 = achterkant), Y = zijwaarts (0 = midden, + = links) */
/* zelfde maten als fusion_bodemplaat_duo.py                                 */
/* ======================================================================= */
s = [];
const K = 4.0; // px per mm
const MW = 1400, MH = 990;
s.push(`<rect x="0" y="0" width="${MW}" height="${MH}" fill="#fff"/>`);
s.push(`<rect x="10" y="10" width="${MW - 20}" height="${MH - 20}" fill="none" stroke="#000" stroke-width="2"/>`);
const T = (x, y, t, o = {}) => s.push(`<text x="${x}" y="${y}" font-size="${o.size || 13}" font-family="Arial" fill="${o.fill || "#000"}" text-anchor="${o.anchor || "start"}" font-weight="${o.bold ? "bold" : "normal"}"${o.rot ? ` transform="rotate(${o.rot} ${x} ${y})"` : ""}>${t}</text>`);
const L = (x1, y1, x2, y2, o = {}) => s.push(`<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${o.col || "#000"}" stroke-width="${o.w || 1}"${o.dash ? ' stroke-dasharray="8 4 2 4"' : ""}/>`);
s.push(`<defs><marker id="a" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="#0040a0"/></marker></defs>`);
function dimH(x1, x2, y, label) {
  s.push(`<line x1="${x1}" y1="${y}" x2="${x2}" y2="${y}" stroke="#0040a0" stroke-width="1" marker-start="url(#a)" marker-end="url(#a)"/>`);
  T((x1 + x2) / 2, y - 5, label, { anchor: "middle", fill: "#0040a0", size: 12 });
}
function dimV(x, y1, y2, label, side = -6) {
  s.push(`<line x1="${x}" y1="${y1}" x2="${x}" y2="${y2}" stroke="#0040a0" stroke-width="1" marker-start="url(#a)" marker-end="url(#a)"/>`);
  if (label) T(x + side, (y1 + y2) / 2, label, { anchor: "middle", fill: "#0040a0", size: 12, rot: -90 });
}

/* geometrie (identiek aan fusion_bodemplaat_duo.py) */
const LEN = 150, AXLE = 130, AHEAD_X = 10, STEER_X = 60, CASTER_X = 35;
const OUTLINE = [[0, 24], [0, -24], [20, -24], [20, -15], [50, -15], [50, -24], [70, -24], [90, -48],
  [110, -48], [110, -30], [150, -30], [150, 30], [110, 30], [110, 48], [90, 48], [70, 24], [50, 24],
  [50, 15], [20, 15], [20, 24]];
const HOLES = [[10, 15], [10, -15], [60, 15], [60, -15], [35, 8], [35, -8], [100, 38], [100, -38], [140, 0]];

/* bovenaanzicht: voorkant rechts (rijrichting →), achterkant links */
const ox = 90, oy = 330;
const px = X => ox + (LEN - X) * K;
const py = Y => oy - Y * K;
const poly = (pts, fill, o = {}) => s.push(`<polygon points="${pts.map(([X, Y]) => `${px(X).toFixed(1)},${py(Y).toFixed(1)}`).join(" ")}" fill="${fill}" stroke="${o.stroke || "#000"}" stroke-width="${o.sw || 1.6}"${o.dash ? ' stroke-dasharray="6 4"' : ""}/>`);
const rect = (X1, Y1, X2, Y2, fill, o = {}) => poly([[X1, Y1], [X2, Y1], [X2, Y2], [X1, Y2]], fill, o);

T(40, 50, "BOVENAANZICHT - LineFollower DUO - 3D-geprinte bodemplaat 4 mm - maten in mm", { bold: true, size: 15 });

poly(OUTLINE, "#dde3ea");
// gleuven
rect(8.5, -22, 10, 22, "#b9c3cf", { sw: 1 });
rect(58.5, -22, 60, 22, "#b9c3cf", { sw: 1 });
// LiPo onder de plaat (gestippeld)
rect(75, -30, 125, 30, "none", { dash: true, stroke: "#4a78c2", sw: 1.4 });
T(px(100), py(-25), "LiPo 50x60 (onder)", { anchor: "middle", size: 11, fill: "#4a78c2" });
// wielen in de uitsparingen + motoren onder de plaat
for (const side of [1, -1]) {
  rect(AXLE - 17, side * 39, AXLE + 17, side * 46, "#333", { sw: 1 });
  rect(AXLE - 6, side * 4, AXLE + 6, side * 36, "none", { dash: true, stroke: "#555", sw: 1.2 });
}
L(px(AXLE), py(58), px(AXLE), py(-58), { dash: true, col: "#a00" });
T(px(AXLE), py(60), "wielas", { anchor: "middle", size: 11, fill: "#a00" });
// onderdelen bovenop
rect(77.3, -26, 106.3, 26, "#fff2c0");
T(px(91.8), py(0), "U1 ESP32 DevKit", { anchor: "middle", size: 12, rot: -90 });
rect(115.3, 0.4, 133.3, 16.4, "#ffe0e0");
T(px(124.3), py(7), "U2 DRV8833", { anchor: "middle", size: 10 });
rect(111.2, -19.3, 133.2, -2.3, "#e0f0ff");
T(px(122.2), py(-12), "U4 MT3608", { anchor: "middle", size: 10 });
// sensoren onder de sensorbalken
for (const [X, name, col] of [[AHEAD_X, "U6 sensor VOORAAN", "#c06000"], [STEER_X, "U3 sensor STUREN", "#0060c0"]]) {
  rect(X - 6, -38, X + 6, 38, "none", { dash: true, stroke: col, sw: 2 });
  for (let i = 0; i < 8; i++) {
    const sy = (i - 3.5) * 9;
    rect(X - 1.5, sy - 1.5, X + 1.5, sy + 1.5, col, { stroke: col, sw: 0.5 });
  }
  T(px(X), py(42), name, { anchor: "middle", size: 11, fill: col });
}
// ball caster onder de hals
s.push(`<circle cx="${px(CASTER_X)}" cy="${py(0)}" r="${6 * K}" fill="none" stroke="#555" stroke-dasharray="5 3" stroke-width="1.5"/>`);
T(px(CASTER_X), py(-19), "ball caster", { anchor: "middle", size: 10, fill: "#555" });
// gaten
for (const [X, Y] of HOLES) s.push(`<circle cx="${px(X)}" cy="${py(Y)}" r="${1.6 * K}" fill="#fff" stroke="#000"/>`);
// rijrichting
s.push(`<line x1="${px(0) + 70}" y1="${oy}" x2="${px(0) + 150}" y2="${oy}" stroke="#000" stroke-width="2" marker-end="url(#a)"/>`);
T(px(0) + 110, oy - 10, "rijrichting", { anchor: "middle", size: 12 });
// maten
const yb = py(-48);
dimH(px(150), px(0), yb + 40, "150");
dimH(px(AXLE), px(AHEAD_X), yb + 70, "120 (wielas → sensor VOORAAN)");
dimH(px(AXLE), px(STEER_X), yb + 100, "70 (wielas → sensor STUREN)");
dimH(px(150), px(AXLE), yb + 70, "20");
dimH(px(50), px(20), py(48) - 20, "hals 30");
dimV(px(150) - 30, py(30), py(-30), "60");
dimV(px(100), py(48), py(-48), "96", 12);
dimV(px(0) + 30, py(24), py(-24), "48", 12);

/* zijaanzicht */
const sx = 90, groundY = 905;
const sxX = X => sx + (LEN - X) * K;
T(40, 690, "ZIJAANZICHT", { bold: true, size: 15 });
L(sx - 30, groundY, sxX(0) + 60, groundY, { w: 2 });
s.push(`<circle cx="${sxX(AXLE)}" cy="${groundY - 17 * K}" r="${17 * K}" fill="#333"/>`);
s.push(`<rect x="${sxX(AXLE + 6)}" y="${groundY - 23 * K}" width="${12 * K}" height="${12 * K}" fill="#999" stroke="#000"/>`);
const pcbY = groundY - 27 * K;
s.push(`<rect x="${sxX(150)}" y="${pcbY}" width="${150 * K}" height="${4 * K}" fill="#9aa7b5" stroke="#000"/>`);
s.push(`<rect x="${sxX(106.3)}" y="${pcbY - 14 * K}" width="${29 * K}" height="${14 * K}" fill="#fff2c0" stroke="#000"/>`);
s.push(`<rect x="${sxX(133.3)}" y="${pcbY - 6 * K}" width="${22 * K}" height="${6 * K}" fill="#e0f0ff" stroke="#000"/>`);
s.push(`<rect x="${sxX(125)}" y="${pcbY + 4 * K}" width="${50 * K}" height="${8 * K}" fill="#cfe0ff" stroke="#000" stroke-dasharray="5 3"/>`);
for (const [X, col] of [[AHEAD_X, "#c06000"], [STEER_X, "#0060c0"]]) {
  s.push(`<rect x="${sxX(X + 6)}" y="${groundY - 4.6 * K}" width="${12 * K}" height="${1.6 * K}" fill="${col}"/>`);
  L(sxX(X), groundY - 4.6 * K, sxX(X), pcbY + 4 * K, { w: 3, col: "#777" });
}
s.push(`<circle cx="${sxX(CASTER_X)}" cy="${groundY - 6 * K}" r="${6 * K}" fill="#bbb" stroke="#000"/>`);
L(sxX(CASTER_X), groundY - 12 * K, sxX(CASTER_X), pcbY + 4 * K, { w: 3, col: "#777" });
T(sxX(CASTER_X), groundY + 16, "ball caster", { anchor: "middle", size: 11, fill: "#555" });
T(sxX(AHEAD_X), groundY + 16, "VOORAAN", { anchor: "middle", size: 11, fill: "#c06000" });
T(sxX(STEER_X), groundY + 16, "STUREN", { anchor: "middle", size: 11, fill: "#0060c0" });
dimV(sxX(0) + 30, groundY, pcbY - 14 * K, "± 41 (&lt; 120 chronobrug)");
dimV(sx - 20, groundY, groundY - 34 * K, "Ø34 wiel");

/* uitleg */
const ex = 860;
T(ex, 110, "WAAROM 2 SENSOREN ACHTER ELKAAR?", { bold: true, size: 14, fill: "#880000" });
[
  "sensor VOORAAN (120 mm voor de wielas):",
  "  ziet een bocht ± 50 mm eerder -> de robot",
  "  remt AF voor hij in de bocht zit",
  "sensor STUREN (70 mm voor de wielas):",
  "  volgt de lijn nauwkeurig -> input PID",
  "lijn kwijt op STUREN maar zichtbaar VOORAAN:",
  "  sturen op de sensor vooraan (S-bochten)",
  "smalle hals + ball caster onder de hals:",
  "  lange neus blijft licht en gesteund",
].forEach((l, i) => T(ex, 138 + i * 19, l, { size: 12, fill: "#333" }));

/* titelblok */
const mx = MW - 430, my = MH - 150;
s.push(`<rect x="${mx}" y="${my}" width="410" height="130" fill="#fff" stroke="#000" stroke-width="2"/>`);
L(mx, my + 40, mx + 410, my + 40); L(mx, my + 75, mx + 410, my + 75);
T(mx + 10, my + 27, "LineFollower DUO - mechanische layout", { bold: true, size: 16 });
T(mx + 10, my + 62, "3D-geprinte bodemplaat 4 mm - maten in mm", { size: 13 });
T(mx + 10, my + 98, "bron: fusion_bodemplaat_duo.py (Fusion 360)", { size: 12 });
T(mx + 10, my + 116, "L x B x H: 150 x 96 x ± 41 mm - ± 170 g   rev 4.0", { size: 12 });

const mech = `<svg xmlns="http://www.w3.org/2000/svg" width="${MW}" height="${MH}" viewBox="0 0 ${MW} ${MH}">\n${s.join("\n")}\n</svg>\n`;
fs.writeFileSync(path.join(__dirname, "..", "mechanisch", "mechanisch_planB.svg"), mech);
console.log("ok");
