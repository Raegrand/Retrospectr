#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;

void initDisplay();
void showMessage(String msg, uint16_t color);
void drawIcon(String filename, int x, int y, int width, int height);
void drawBinByIndex(int index);

void readTextFile(int index, int speedMs);
bool displayWord(String word, int speedMs, String bgPath = "", String headerText = "");
bool processAndDisplayWord(String rawWord, int baseSpeedMs, String bgPath = "", String headerText = "");
void displayStaticText(String headerText, String bodyText, String bgPath = "");
bool readPaginatedText(String textPath, String headerText);

void readTextFileCustom(String textPath, int speedMs, String bgPath = "");
void readRandomScripture(String textPath, int speedMs, String bgPath = "");

void showTVStatic(int duration_ms);
void sleepDisplay();    