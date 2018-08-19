// Consulted PumpkinCon(https://github.com/kkkyyy03/PumpkinCon) by RedPumpkin,
// Everyone is permitted to copy and distribute this code

#include "UnoJoy.h"
#include <Encoder.h>

// Setup Buttons' Pin
const int btA = 30;
const int btB = 32;
const int btC = 34;
const int btD = 36;
const int fxL = 38;
const int fxR = 40;
const int str = 50; // Start Button

// Setup Knobs' Pin
const int knobLA = 42;
const int knobLB = 43;
const int knobRA = 44;
const int knobRB = 45;

Encoder knobL(knobLA, knobLB);
Encoder knobR(knobRA, knobRB);

const int knobMin = 0;
const int knobMid = 128;
const int knobMax = 255;
const int knobPad = 1; // Knob Sensitivity

const int easingLength = 200;

long easingStartTimeL = 0;
long easingStartTimeR = 0;

float knobLData = knobMid;
float knobRData = knobMid;
float lastKnobL = knobLData;
float lastKnobR = knobRData;

const int test = 52; // Only use it for SDVX Arcade HDD
const int service = 28;

// LED PIN
const int ledBtA = 31;
const int ledBtB = 33;
const int ledBtC = 35;
const int ledBtD = 37;
const int ledFxL = 39;
const int ledFxR = 41;
const int ledStr = 51;

// void setup part
void setup() {
  setupUnoJoy();
  
  // Setup Pin
  pinMode(btA, INPUT);
  pinMode(btB, INPUT);
  pinMode(btC, INPUT);
  pinMode(btD, INPUT);
  pinMode(fxL, INPUT);
  pinMode(fxR, INPUT);
  pinMode(str, INPUT);
  digitalWrite(btA, HIGH);
  digitalWrite(btB, HIGH);
  digitalWrite(btC, HIGH);
  digitalWrite(btD, HIGH);
  digitalWrite(fxL, HIGH);
  digitalWrite(fxR, HIGH);
  digitalWrite(str, HIGH);

  // Only use it for SDVX Arcade HDD
  pinMode(test, INPUT);
  pinMode(service, INPUT);
  digitalWrite(test, HIGH);
  digitalWrite(service, HIGH);

  // Setup LED
  pinMode(ledBtA, OUTPUT);
  pinMode(ledBtB, OUTPUT);
  pinMode(ledBtC, OUTPUT);
  pinMode(ledBtD, OUTPUT);
  pinMode(ledFxL, OUTPUT);
  pinMode(ledFxR, OUTPUT);
  pinMode(ledStr, OUTPUT);

  // Setup Knob
  knobL.write(knobMid);
  knobR.write(knobMid);
}

int autoLed(int btn, int led) {
  int btnStatus = digitalRead(btn);

  if (btnStatus == HIGH) {
    digitalWrite(led, LOW);
  } else {
    digitalWrite(led, HIGH);
  }

  return btnStatus;
}

float easeOutQuad(long t, float b, float c, long d) {
  float tt = ((float) t) / d;
  return -c * tt * (tt - 2) + b;
};


float easingKnob(float knob, long easingStartTime) {
  if (knob > knobMid + knobPad) {
    knob = knobMax - easeOutQuad(millis() - easingStartTime, knobMin, knobMid, easingLength);
  } else if (knob < knobMid - knobPad) {
    knob = easeOutQuad(millis() - easingStartTime, knobMin, knobMid, easingLength);
  } else {
    knob = knobMid;
    return knob;
  }

  if (knob > knobMax) {
    knob = knobMax;
  } else if (knob < knobMin) {
    knob = knobMin;
  }

  return knob;
}

// void loop part
void loop() {
  dataForController_t controllerData = getBlankDataForController();

  // Knob Controller Part
  float _knobLChange = -knobL.read();
  knobL.write(0);
  knobLData = easingKnob(knobLData, easingStartTimeL);
  if (_knobLChange > 0) {
    easingStartTimeL = millis();
    knobLData = knobMax;
  } else if (_knobLChange < 0) {
    easingStartTimeL = millis();
    knobLData = knobMin;
  }
  controllerData.leftStickX = (long) knobLData;

  float _knobRChange = -knobR.read();
  knobR.write(0);
  knobRData = easingKnob(knobRData, easingStartTimeR);
  if (_knobRChange > 0) {
    easingStartTimeR = millis();
    knobRData = knobMax;
  } else if (_knobRChange < 0) {
    easingStartTimeR = millis();
    knobRData = knobMin;
  }
  controllerData.rightStickX = (long) knobRData;

  controllerData.circleOn = !autoLed(fxL, ledFxL);
  controllerData.triangleOn = !autoLed(fxR, ledFxR);
  controllerData.squareOn = !autoLed(btA, ledBtA);
  controllerData.crossOn = !autoLed(btB, ledBtB);
  controllerData.l1On = !autoLed(btC, ledBtC);
  controllerData.selectOn = !autoLed(btD, ledBtD);
  controllerData.startOn = !autoLed(str, ledStr);

  controllerData.r1On = !digitalRead(test);
  controllerData.r2On = !digitalRead(service);

  setControllerData(controllerData);
}
