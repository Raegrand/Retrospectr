#include "src/header/config.h"
#include "src/header/encoder.h"
#include "src/header/storage.h"
#include "src/header/display.h"
#include "src/header/power.h"
#include <Preferences.h>
#include "esp_sleep.h"

int totalImages = 0;
RTC_DATA_ATTR int currentImage = 1; 

bool isScreenOn = true; 

// Inactivity and Sleep Tracking
unsigned long lastActivityTime = 0;
int sleepTimeoutMinutes = 3;
int textSpeedMs = 330;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Load settings
  sleepTimeoutMinutes = loadSleepTimeout();
  textSpeedMs = loadTextSpeed();

  systemStartUp();
  
  lastActivityTime = millis(); // Reset inactivity timer on boot

  if (totalImages > 0) {
    drawBinByIndex(currentImage); 
  } else {
    showMessage("No images", ST77XX_YELLOW);
  }
}

void loop() {
  // 1. Check Extra Button press
  if (isExtraButtonPressed()) {
    lastActivityTime = millis(); // Activity detected
    isScreenOn = !isScreenOn; 
    digitalWrite(TFT_BL, isScreenOn ? HIGH : LOW);
  }

  if (totalImages == 0 || !isScreenOn) return;

  // 2. Check for scrolling
  int scroll = getEncoderScroll();
  if (scroll != 0) {
    lastActivityTime = millis(); // Activity detected
    currentImage += scroll;
    
    if (currentImage < 1) currentImage = totalImages; 
    if (currentImage > totalImages) currentImage = 1; 
    
    drawBinByIndex(currentImage); 
  }

  // 3. Check for select clicks
  if (isButtonPressed()) {
    lastActivityTime = millis(); // Activity detected
    readTextFile(currentImage, textSpeedMs);
    drawBinByIndex(currentImage);
    lastActivityTime = millis(); // Reset after text viewing finishes
  }

  // 4. Inactivity Timeout Check
  unsigned long timeoutMs = (unsigned long)sleepTimeoutMinutes * 60000;
  if (millis() - lastActivityTime >= timeoutMs) {
    enterDeepSleep();
  }
}

void systemStartUp() {
  Serial.println("\n--- SYSTEM WAKE / START UP ---");

  initEncoder();

  // IMPORTANT: SD initialized FIRST to prevent SPI mode conflict
  if (!initSD()) {
    Serial.println("SD Card Failed!");
    return;
  }
  Serial.println("SD Card OK!");

  // Display initialized SECOND
  initDisplay();
  
  // NEW: Play 1000 milliseconds (1 second) of analog TV static
  showTVStatic(1000); 

  totalImages = countTotalImages();
  Serial.print("Total images found: ");
  Serial.println(totalImages);
}