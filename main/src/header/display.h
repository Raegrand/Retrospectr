#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

void initDisplay();
void showMessage(String msg, uint16_t color);

void drawIcon(String filename, int x, int y, int width, int height);

void drawBinByIndex(int index);

void readTextFile(int index);
void displayWord(String word);