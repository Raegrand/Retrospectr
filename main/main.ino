#include "src/header/config.h"
#include "src/header/encoder.h"
#include "src/header/storage.h"
#include "src/header/display.h"


// --- STATE VARIABLES ---
int totalImages = 0;
RTC_DATA_ATTR int currentImage = 1; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  systemStartUp();
  
  if (totalImages > 0) {
    drawBinByIndex(currentImage); // Uses the new index wrapper
  } else {
    showMessage("No images", ST77XX_YELLOW);
  }
}

void loop() {
  if (totalImages == 0) return;

  // 1. Check for scrolling
  int scroll = getEncoderScroll();
  if (scroll != 0) {
    currentImage += scroll;
    
    // Wrap around logic
    if (currentImage < 1) currentImage = totalImages; 
    if (currentImage > totalImages) currentImage = 1; 
    
    drawBinByIndex(currentImage); // Uses the new index wrapper
  }

  // 2. Check for clicks
  if (isButtonPressed()) {
    readTextFile(currentImage);
    drawBinByIndex(currentImage);
  }
  
}

void systemStartUp() {
  Serial.println("\n--- SYSTEM WAKE / START UP ---");

  initEncoder();
  initDisplay();

  if (!initSD()) {
    Serial.println("SD Card Failed!");
    showMessage("SD Failed", ST77XX_RED);
    return;
  }
  Serial.println("SD Card OK!");

  totalImages = countTotalImages();
  Serial.print("Total images found: ");
  Serial.println(totalImages);
}