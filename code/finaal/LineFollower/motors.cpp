#include "motors.h"
#include "config.h"
#include "params.h"
#include <math.h>

volatile int motorPwmL = 0, motorPwmR = 0;

static const uint8_t PIN_A[2] = {AIN1, BIN1};
static const uint8_t PIN_B[2] = {AIN2, BIN2};
static const bool INV[2] = {MOTOR_A_INVERT, MOTOR_B_INVERT};

static int lastSign[2] = {0, 0};
static uint32_t kickUntil[2] = {0, 0};

#if ESP_ARDUINO_VERSION_MAJOR >= 3
static inline void pwm(uint8_t pin, uint8_t ch, uint32_t d) { (void)ch; ledcWrite(pin, d); }
#else
static inline void pwm(uint8_t pin, uint8_t ch, uint32_t d) { (void)pin; ledcWrite(ch, d); }
#endif

void motorsBegin() {
  const uint8_t pins[4] = {AIN1, AIN2, BIN1, BIN2};
  for (uint8_t i = 0; i < 4; i++) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(pins[i], PWM_FREQ, PWM_BITS);
#else
    ledcSetup(i, PWM_FREQ, PWM_BITS);
    ledcAttachPin(pins[i], i);
#endif
  }
  motorsStop();
}

// m = 0 (links) of 1 (rechts). Kanalen (core 2.x): A=0/1, B=2/3
static int writeMotor(uint8_t m, float cmd, uint32_t now) {
  uint8_t pa = PIN_A[m], pb = PIN_B[m];
  uint8_t ca = m * 2, cb = m * 2 + 1;
  if (INV[m]) cmd = -cmd;

  float mag = fabsf(cmd);
  if (mag < 0.5f) {                 // uit = uitrollen (beide pinnen laag)
    pwm(pa, ca, 0);
    pwm(pb, cb, 0);
    lastSign[m] = 0;
    return 0;
  }
  if (mag > 255.0f) mag = 255.0f;
  int sgn = cmd > 0 ? 1 : -1;

  // Dode zone compenseren: 0..255 -> runPwm..255 (continu, geen sprongen buiten 0)
  float run = P.runPwm;
  int d = (int)lroundf(run + mag * (255.0f - run) / 255.0f);

  // Startpuls bij vertrek uit stilstand of bij omkeren van draairichting
  if (sgn != lastSign[m]) kickUntil[m] = now + (uint32_t)P.kickMs;
  lastSign[m] = sgn;
  if ((int32_t)(kickUntil[m] - now) > 0 && d < (int)P.kickPwm) d = (int)P.kickPwm;
  if (d > 255) d = 255;
  if (d < 0) d = 0;

#if DECAY_SLOW
  if (sgn > 0) { pwm(pa, ca, 255);     pwm(pb, cb, 255 - d); }
  else         { pwm(pa, ca, 255 - d); pwm(pb, cb, 255); }
#else
  if (sgn > 0) { pwm(pb, cb, 0); pwm(pa, ca, d); }
  else         { pwm(pa, ca, 0); pwm(pb, cb, d); }
#endif
  return INV[m] ? -sgn * d : sgn * d;
}

void motorsDrive(float left, float right) {
  uint32_t now = millis();
  motorPwmL = writeMotor(0, left, now);
  motorPwmR = writeMotor(1, right, now);
}

void motorsStop() {
  for (uint8_t m = 0; m < 2; m++) {
    pwm(PIN_A[m], m * 2, 0);
    pwm(PIN_B[m], m * 2 + 1, 0);
    lastSign[m] = 0;
  }
  motorPwmL = motorPwmR = 0;
}
