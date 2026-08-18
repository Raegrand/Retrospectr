#include "../header/storage.h"
#include "../header/config.h"
#include <SPI.h>
#include <SD.h>

bool initSD() {
  // Explicitly tell the TFT to stay off the SPI bus during SD init
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  SPI.begin();
  delay(10);
  
  for (int attempt = 0; attempt < 5; attempt++) {
    if (SD.begin(SD_CS, SPI, 4000000)) return true;
    delay(100);
  }
  return false;
}

int countTotalImages() {
  int count = 0;
  while (true) {
    String path = "/gallery/" + String(count + 1) + ".bin";
    
    // Test if file exists by trying to open it directly
    File testFile = SD.open(path, FILE_READ);
    if (testFile) {
      count++;
      testFile.close(); // CRITICAL: Free up the file handle!
    } else {
      break; 
    }
  }
  return count;
}