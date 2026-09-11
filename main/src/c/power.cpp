#include "../header/power.h"
#include "../header/config.h"
#include "../header/display.h"
#include <Preferences.h>
#include "esp_sleep.h"

Preferences prefs;

int loadSleepTimeout() {
  prefs.begin("retrospect", true); // Read-only mode
  int timeout = prefs.getInt("sleep_time", 3);
  prefs.end();
  return timeout;
}

void saveSleepTimeout(int minutes) {
  prefs.begin("retrospect", false); // Read-Write mode
  prefs.putInt("sleep_time", minutes);
  prefs.end();
}

void enterDeepSleep() {
  Serial.println("Entering Deep Sleep...");

  // Shut down TFT backlight and put ST7789 controller to sleep
  digitalWrite(TFT_BL, LOW);
  sleepDisplay(); // NEW: Uses the encapsulated call

  // High-impedance isolation for SPI CS pins
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);

  // Configure GPIO 5 (EXTRA_BTN) as active-LOW wakeup trigger
  esp_deep_sleep_enable_gpio_wakeup(1ULL << EXTRA_BTN, ESP_GPIO_WAKEUP_GPIO_LOW);

  esp_deep_sleep_start();
}

int loadTextSpeed() {
  prefs.begin("retrospect", true);
  // Default base speed set to 300ms for a more comfortable reading experience
  int speedMs = prefs.getInt("text_speed", 330); 
  prefs.end();
  return speedMs;
}

void saveTextSpeed(int speedMs) {
  prefs.begin("retrospect", false);
  prefs.putInt("text_speed", speedMs);
  prefs.end();
}

int loadBrightness() {
  prefs.begin("retrospect", true);
  int bright = prefs.getInt("brightness", 200); // Default 200 (~80% brightness)
  prefs.end();
  return bright;
}

void saveBrightness(int val) {
  prefs.begin("retrospect", false);
  prefs.putInt("brightness", val);
  prefs.end();
}