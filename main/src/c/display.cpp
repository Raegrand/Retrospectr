#include "../header/display.h"
#include "../header/config.h"
#include <SPI.h>
#include <SD.h>

// Initialize the TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init(240, 280);
  tft.setRotation(3); // Keeping your preferred rotation
  tft.fillScreen(ST77XX_BROWN);
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

void displayWord(String word) {
  tft.fillScreen(ST77XX_BLACK);
  int textWidth = word.length() * 18;
  int cursorX = (TFT_WIDTH - textWidth) / 2;
  if (cursorX < 0) cursorX = 0;

  tft.setCursor(cursorX, 100);
  tft.print(word);
  delay(350);
}

void readTextFile(int index) {
  String path = "/gallery/" + String(index) + ".txt";

  digitalWrite(TFT_CS, HIGH);
  File txtFile = openWithRetry(path);
  digitalWrite(SD_CS, HIGH);

  if (!txtFile) {
    Serial.print("No text file found: ");
    Serial.println(path);
    return;
  }

  // --- Phase 1: read the ENTIRE text file into RAM first. ---
  // No TFT activity happens during this phase.
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

  // --- Phase 2: display words from the buffer. ---
  // No SD activity happens during this phase.
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);

  String currentWord = "";
  for (size_t i = 0; i < totalRead; i++) {
    char c = textBuffer[i];
    if (c == ' ' || c == '\n' || c == '\r') {
      if (currentWord.length() > 0) {
        displayWord(currentWord);
        currentWord = "";
      }
    } else {
      currentWord += c;
    }
  }
  if (currentWord.length() > 0) displayWord(currentWord);

  digitalWrite(TFT_CS, HIGH);
  free(textBuffer);
}