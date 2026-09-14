#include "src/header/config.h"
#include "src/header/encoder.h"
#include "src/header/storage.h"
#include "src/header/display.h"
#include "src/header/power.h"
#include "src/header/menu.h"
#include <Preferences.h>
#include <esp_random.h>
#include "esp_sleep.h"
#include <SD.h>

int totalImages = 0;
RTC_DATA_ATTR int currentImage = 1;

bool isScreenOn = true;
unsigned long lastActivityTime = 0;
unsigned long lastImageChangeTime = 0;
int sleepTimeoutMinutes = 3;
int textSpeedMs = 300;

void setup() {
  Serial.begin(115200);
  delay(1000);

  sleepTimeoutMinutes = loadSleepTimeout();
  textSpeedMs = loadTextSpeed();

  systemStartUp();
  
  lastActivityTime = millis();
  lastImageChangeTime = millis();

  if (totalImages > 0) {
    currentImage = (esp_random() % totalImages) + 1;
    drawBinByIndex(currentImage); 
  } else {
    showMessage("No images", ST77XX_YELLOW);
  }
}

void loop() {
  if (totalImages == 0 || !isScreenOn) return;

  // 1. Extra Button -> Return to / Open Main Menu
  if (isExtraButtonPressed()) {
    lastActivityTime = millis();

    MenuChoice choice = openMainMenu();

    switch (choice) {
      case MENU_GALLERY:
        // Return to viewing current gallery image
        drawBinByIndex(currentImage);
        break;

      case MENU_BIBLE:
      {
        String languageFolder = openLanguageMenu();
        if (languageFolder.length() == 0) { drawBinByIndex(currentImage); break; }

        int testamentChoice = openTestamentMenu();
        if (testamentChoice == 0) { drawBinByIndex(currentImage); break; }
        
        String startPath = "/" + languageFolder;
        if (!SD.exists(startPath)) startPath = "/Bible/" + languageFolder; 

        if (!SD.exists(startPath)) {
          showMessage("Folder Missing!", ST77XX_RED);
          delay(1500); drawBinByIndex(currentImage); break;
        }

        String selectedFile = openBibleBrowser(startPath, testamentChoice);
        
        if (selectedFile.length() > 0) {
          bool reading = true;

          // CONTINUOUS CHAPTER LOOP
          while (reading) {
            String headerText = selectedFile;
            
            if (headerText.startsWith(startPath)) headerText = headerText.substring(startPath.length());
            if (headerText.startsWith("/")) headerText = headerText.substring(1);
            headerText.replace('/', ' - '); 

            // readPaginatedText returns true if you hit "next" on the final page
            bool advanceToNext = readPaginatedText(selectedFile, headerText);
            
            reading = false; // Assume we are stopping

            if (advanceToNext) {
              // Extract the number from the file name (e.g. Chapter_1.txt -> 1)
              int dotIdx = selectedFile.lastIndexOf('.');
              if (dotIdx != -1) {
                int numEnd = dotIdx;
                int numStart = dotIdx - 1;
                while (numStart >= 0 && isDigit(selectedFile[numStart])) {
                  numStart--;
                }
                numStart++;
                
                if (numStart < numEnd) {
                  String numStr = selectedFile.substring(numStart, numEnd);
                  int nextNum = numStr.toInt() + 1;
                  
                  // Construct new path (e.g. Chapter_2.txt)
                  String nextPath = selectedFile.substring(0, numStart) + String(nextNum) + selectedFile.substring(numEnd);
                  
                  // Automatically load the next chapter if it exists
                  if (SD.exists(nextPath)) {
                    selectedFile = nextPath;
                    reading = true; 
                  }
                }
              }
            }
          } // End while(reading)
        }
        
        drawBinByIndex(currentImage);
      }
      break;

      case MENU_SETTINGS:
        // Open Settings configuration menu
        openSettingsSubmenu();
        // Reload settings values in case they were updated
        textSpeedMs = loadTextSpeed();
        sleepTimeoutMinutes = loadSleepTimeout();
        drawBinByIndex(currentImage);
        break;
    }

    lastActivityTime = millis();
    lastImageChangeTime = millis();
  }

  // 2. Rotary Encoder Scroll (Gallery Navigation)
  int scroll = getEncoderScroll();
  if (scroll != 0) {
    lastActivityTime = millis();
    currentImage += scroll;
    
    if (currentImage < 1) currentImage = totalImages; 
    if (currentImage > totalImages) currentImage = 1; 
    
    drawBinByIndex(currentImage); 
    lastImageChangeTime = millis();
  }

  // 3. Encoder Button Click -> View Scripture Text for Current Image
  if (isButtonPressed()) {
    lastActivityTime = millis();
    readTextFile(currentImage, textSpeedMs);
    drawBinByIndex(currentImage);
    
    lastActivityTime = millis();
    lastImageChangeTime = millis();
  }

  // 4. Auto-Advance (Slideshow every 15 sec)
  if (millis() - lastImageChangeTime >= 15000) {
    currentImage++;
    if (currentImage > totalImages) currentImage = 1; 
    
    drawBinByIndex(currentImage); 
    lastImageChangeTime = millis();
  }

  // 5. Inactivity Deep Sleep Check
  unsigned long timeoutMs = (unsigned long)sleepTimeoutMinutes * 60000;
  if (millis() - lastActivityTime >= timeoutMs) {
    enterDeepSleep();
  }
}

void systemStartUp() {
  Serial.println("\n--- SYSTEM WAKE / START UP ---");

  initEncoder();

  if (!initSD()) {
    Serial.println("SD Card Failed!");
    return;
  }
  Serial.println("SD Card OK!");

  initDisplay();

  // 1. Play Analog TV Static Animation
  showTVStatic(1000); 

  // 2. 15% Chance Scripture Startup Trigger
  if ((esp_random() % 100) < 15) {
    Serial.println("Startup Event: Displaying Random Scripture!");
    readRandomScripture("/media/scr.txt", textSpeedMs, "/media/dailyBg.bin");
  }

  totalImages = countTotalImages();
  Serial.print("Total images found: ");
  Serial.println(totalImages);
}