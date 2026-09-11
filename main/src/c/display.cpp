#include "../header/display.h"
#include "../header/config.h"
#include "../header/encoder.h"
#include <SPI.h>
#include <SD.h>
#include <Fonts/FreeSerifItalic18pt7b.h>

// Initialize the TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init(240, 280);
  tft.setRotation(3); // Keeping your preferred rotation
  tft.fillScreen(ST77XX_BLACK);
}

void showMessage(String msg, uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print(msg);
}

void drawBinByIndex(int index) {
  String path = "/gallery/" + String(index) + ".bin";
  drawIcon(path, 0, 0, TFT_WIDTH, TFT_HEIGHT);
}

// Attempts to open a file, and if it fails, re-inits the SD card once
// and retries. Only fires on an actual failure - never called unconditionally.
//
// IMPORTANT: the ESP32 Arduino SD driver has a known bug (see
// espressif/arduino-esp32#9218) where it doesn't reliably restore SPI mode 0
// before talking to the card. If the TFT left the bus in mode 3 from its
// last write, the very next SD command gets sent in the wrong clock
// polarity/phase and the card rejects it. Forcing mode 0 with a throwaway
// transaction right before every SD access works around this.
void forceSPIMode0ForSD() {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
}

File openWithRetry(const String &path) {
  digitalWrite(TFT_CS, HIGH);
  forceSPIMode0ForSD();
  File f = SD.open(path, FILE_READ);
  digitalWrite(SD_CS, HIGH);

  if (!f) {
    Serial.print("Open failed for ");
    Serial.print(path);
    Serial.println(", attempting SD re-init...");

    SD.end();
    bool ok = SD.begin(SD_CS, SPI, 4000000);
    Serial.print("Re-init result: ");
    Serial.println(ok ? "OK" : "FAILED");

    if (ok) {
      digitalWrite(TFT_CS, HIGH);
      forceSPIMode0ForSD();
      f = SD.open(path, FILE_READ);
      digitalWrite(SD_CS, HIGH);
      Serial.print("Retry open result: ");
      Serial.println(f ? "OK" : "still failed");
    }
  }
  return f;
}

void drawIcon(String filename, int x, int y, int width, int height) {
  Serial.print("Drawing ");
  Serial.println(filename);

  digitalWrite(TFT_CS, HIGH);
  File binFile = openWithRetry(filename);
  digitalWrite(SD_CS, HIGH);

  if (!binFile) {
    Serial.print("Failed to open: ");
    Serial.println(filename);
    return;
  }

  tft.startWrite();
  tft.setAddrWindow(x, y, width, height);
  tft.endWrite();
  digitalWrite(TFT_CS, HIGH);

  size_t lineBytes = width * 2; // one row's worth of RGB565 pixels
  uint16_t *lineBuffer = (uint16_t *)malloc(lineBytes);
  if (!lineBuffer) {
    Serial.println("Failed to allocate line buffer!");
    binFile.close();
    digitalWrite(SD_CS, HIGH);
    return;
  }

  for (int row = 0; row < height; row++) {
    // --- Phase 1: read exactly one line, SD has the bus to itself ---
    digitalWrite(TFT_CS, HIGH);
    forceSPIMode0ForSD();   // undo whatever mode the last TFT write left the bus in
    size_t bytesRead = binFile.read((uint8_t *)lineBuffer, lineBytes);
    digitalWrite(SD_CS, HIGH);

    if (bytesRead == 0) break; // ran out of data early

    size_t numPixels = bytesRead / 2;
    uint8_t *raw = (uint8_t *)lineBuffer;
    for (size_t i = 0; i < numPixels; i++) {
      lineBuffer[i] = (raw[i*2 + 1] << 8) | raw[i*2];
    }

    // --- Phase 2: write that one line, TFT has the bus to itself ---
    tft.startWrite();
    tft.writePixels(lineBuffer, numPixels);
    tft.endWrite();
    digitalWrite(TFT_CS, HIGH);
  }

  free(lineBuffer);
  binFile.close();
  digitalWrite(SD_CS, HIGH);
}

bool displayWord(String word, int speedMs) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setFont(&FreeSerifItalic18pt7b  );

  // Measure bounding box to calculate horizontal width ONLY
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(word, 0, 0, &x1, &y1, &w, &h);

  // Dynamically center horizontally across the X axis
  int cursorX = (TFT_WIDTH - w) / 2;
  if (cursorX < 0) cursorX = 0; // Prevent left overflow for long words
  
  // FIXED BASELINE: Lock Y to a constant position on screen.
  // Adding ~10-12px places the text baseline just below center screen,
  // keeping ascenders and descenders perfectly aligned across every word.
  int cursorY = (TFT_HEIGHT / 2) + 8;

  tft.setCursor(cursorX, cursorY);
  tft.print(word);

  // Non-blocking interruptible delay
  unsigned long startWait = millis();
  while (millis() - startWait < (unsigned long)speedMs) {
    if (isButtonPressed() || isExtraButtonPressed()) {
      tft.setFont(); // Reset to system font before exiting
      return true;
    }
    delay(10);
  }

  tft.setFont(); // Reset to system font
  return false;
}

// Helper function to strip punctuation and calculate dynamic delay pacing
bool processAndDisplayWord(String rawWord, int baseSpeedMs) {
  if (rawWord.length() == 0) return false;

  bool hasPeriod = false;
  bool hasComma = false;
  String cleanWord = "";

  // Strip '.' and ',' while flagging their presence
  for (size_t j = 0; j < rawWord.length(); j++) {
    char ch = rawWord[j];
    if (ch == '.') {
      hasPeriod = true;
    } else if (ch == ',') {
      hasComma = true;
    } else {
      cleanWord += ch;
    }
  }

  // If the token was strictly punctuation (e.g. "..."), skip rendering empty string
  if (cleanWord.length() == 0) return false;

  // Calculate dynamic pause duration
  int finalDelay = baseSpeedMs;
  if (hasPeriod) {
    finalDelay += 500; // Longer pause for period
  } else if (hasComma) {
    finalDelay += 300; // Medium pause for comma
  }

  return displayWord(cleanWord, finalDelay);
}

void readTextFile(int index, int speedMs) {
  String path = "/gallery/" + String(index) + ".txt";

  digitalWrite(TFT_CS, HIGH);
  File txtFile = openWithRetry(path);
  digitalWrite(SD_CS, HIGH);

  if (!txtFile) {
    Serial.print("No text file found: ");
    Serial.println(path);
    return;
  }

  size_t fileSize = txtFile.size();
  char *textBuffer = (char *)malloc(fileSize + 1);
  if (!textBuffer) {
    Serial.println("Failed to allocate text buffer!");
    txtFile.close();
    digitalWrite(SD_CS, HIGH);
    return;
  }
  size_t totalRead = txtFile.read((uint8_t *)textBuffer, fileSize);
  textBuffer[totalRead] = '\0';

  txtFile.close();
  digitalWrite(SD_CS, HIGH);

  tft.fillScreen(ST77XX_BLACK);

  String currentWord = "";
  bool interrupted = false;

  for (size_t i = 0; i < totalRead; i++) {
    char c = textBuffer[i];
    if (c == ' ' || c == '\n' || c == '\r') {
      if (currentWord.length() > 0) {
        if (processAndDisplayWord(currentWord, speedMs)) {
          interrupted = true;
          break; // Exit loop on button interruption
        }
        currentWord = "";
      }
    } else {
      currentWord += c;
    }
  }

  // Process the last word if file ends without trailing whitespace
  if (!interrupted && currentWord.length() > 0) {
    processAndDisplayWord(currentWord, speedMs);
  }

  digitalWrite(TFT_CS, HIGH);
  free(textBuffer); // Clean memory deallocation
}

void showTVStatic(int duration_ms) {
  // Ensure SD card is deselected so it doesn't eavesdrop on the SPI bus
  digitalWrite(SD_CS, HIGH); 
  digitalWrite(TFT_CS, HIGH);

  // Allocate a single line buffer to keep RAM usage low
  size_t pixelsPerLine = tft.width(); 
  uint16_t *lineBuffer = (uint16_t *)malloc(pixelsPerLine * 2);
  
  if (!lineBuffer) {
    Serial.println("Failed to allocate static buffer!");
    return;
  }

  unsigned long start = millis();
  
  tft.startWrite();
  // Loop rapidly until the duration expires
  while (millis() - start < duration_ms) {
    tft.setAddrWindow(0, 0, tft.width(), tft.height());
    
    for (int y = 0; y < tft.height(); y++) {
      for (int x = 0; x < pixelsPerLine; x++) {
        // Use the ESP32 hardware RNG for lightning-fast noise calculation
        // Bitwise AND (& 1) randomly picks either 0 (false) or 1 (true)
        lineBuffer[x] = (esp_random() & 1) ? ST77XX_WHITE : ST77XX_BLACK;
      }
      // Blast the randomized line to the screen
      tft.writePixels(lineBuffer, pixelsPerLine);
    }
  }
  tft.endWrite();
  
  digitalWrite(TFT_CS, HIGH);
  free(lineBuffer); // Always free the memory!
}

void sleepDisplay() {
  tft.startWrite();
  tft.writeCommand(0x10); // ST7789 SLPIN (Sleep In) command
  tft.endWrite();
}