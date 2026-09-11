#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

int loadSleepTimeout();
void saveSleepTimeout(int minutes);
void enterDeepSleep();
int loadTextSpeed();
void saveTextSpeed(int speedMs);
int loadBrightness();
void saveBrightness(int val);

#endif