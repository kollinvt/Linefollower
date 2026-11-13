#pragma once
#include <Arduino.h>

// Yahboom 8-kanaals sensor in analoge modus ($A-frames, 0..4095 per kanaal) + kalibratie.

void sensorBegin();               // UART starten + analoge uitvoer aanzetten ($0,1,0#)
bool sensorPoll();                // niet-blokkerend; true als er een nieuw frame is
uint32_t sensorLastFrameMs();

// Lijnsterkte per sensor 0..1 (1 = recht boven de lijn), links -> rechts,
// na kalibratie, "lijn geeft hoge waarde" en links/rechts omdraaien.
void sensorStrength(float s[8]);

extern volatile uint16_t sensorRaw[8];

// Kalibratie (alleen als de robot stilstaat)
void calibLoad();
void calibStart();
void calibStop();                 // controleert en slaat op in flash
bool calibRunning();
bool calibOk();
