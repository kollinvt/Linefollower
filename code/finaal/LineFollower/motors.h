#pragma once
#include <Arduino.h>

void motorsBegin();

// left/right: -255..255, continu. 0 = motor uit (uitrollen).
// Elk niet-nul commando wordt geschaald naar "Min. PWM rijden"..255. Als een wiel
// vanuit stilstand of van richting moet veranderen krijgt het eerst kort de startpuls.
void motorsDrive(float left, float right);
void motorsStop();

// Echte PWM die op dit moment naar de motoren gaat (met teken), voor de webpagina
extern volatile int motorPwmL, motorPwmR;
