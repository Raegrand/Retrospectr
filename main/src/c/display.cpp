#include "../header/display.h"
#include "../header/config.h"
#include "../header/encoder.h"
#include <SPI.h>
#include <SD.h>
#include <Fonts/FreeSerifItalic18pt7b.h>
#include <Fonts/FreeSerifItalic12pt7b.h> 
#include <Fonts/FreeSerifBold18pt7b.h> 
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>

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

bool displayWord(String word, int speedMs, String bgPath, String headerText) {
  if (bgPath.length() > 0) {
    drawIcon(bgPath, 0, 0, TFT_WIDTH, TFT_HEIGHT);
  } else {
    tft.fillScreen(ST77XX_BLACK);
  }

  // Draw Header / Title if provided (e.g., "DAILY SCRIPTURE")
  if (headerText.length() > 0) {
    tft.setFont(); // Use standard small font for header
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(10, 10);
    tft.print(headerText);
  }

  // Render Main Word
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setFont(&FreeSerifItalic18pt7b);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(word, 0, 0, &x1, &y1, &w, &h);

  int cursorX = (TFT_WIDTH - w) / 2;
  if (cursorX < 0) cursorX = 0;
  
  int cursorY = (TFT_HEIGHT / 2) + 8;

  tft.setCursor(cursorX, cursorY);
  tft.print(word);

  // Non-blocking wait loop
  unsigned long startWait = millis();
  while (millis() - startWait < (unsigned long)speedMs) {
    if (isButtonPressed() || isExtraButtonPressed()) {
      tft.setFont();
      return true; // Interrupted
    }
    delay(10);
  }

  tft.setFont();
  return false;
}

bool processAndDisplayWord(String rawWord, int baseSpeedMs, String bgPath, String headerText) {
  if (rawWord.length() == 0) return false;

  bool hasPeriod = false;
  bool hasComma = false;
  String cleanWord = "";

  // Strip punctuation while detecting pauses
  for (size_t j = 0; j < rawWord.length(); j++) {
    char ch = rawWord[j];
    if (ch == '.') hasPeriod = true;
    else if (ch == '!') hasPeriod = true;
    else if (ch == '?') hasPeriod = true;
    else if (ch == ':') hasPeriod = true;
    else if (ch == ',') hasComma = true;
    else cleanWord += ch;
  }

  if (cleanWord.length() == 0) return false;

  // 1. DYNAMIC LENGTH PACING: Add ~25ms per character
  // A 2-letter word ("in") adds +50ms
  // A 10-letter word ("strengthens") adds +250ms
  int lengthBonus = cleanWord.length() * 25;
  int finalDelay = baseSpeedMs + lengthBonus;

  // 2. PUNCTUATION DELAYS: Stack on top of length calculation
  if (hasPeriod) {
    finalDelay += 500;
  } else if (hasComma) {
    finalDelay += 300;
  }

  return displayWord(cleanWord, finalDelay, bgPath, headerText);
}

void readTextFileCustom(String textPath, int speedMs, String bgPath) {
  digitalWrite(TFT_CS, HIGH);
  File txtFile = openWithRetry(textPath);
  digitalWrite(SD_CS, HIGH);

  if (!txtFile) {
    Serial.print("Custom text file not found: ");
    Serial.println(textPath);
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

  String currentWord = "";
  bool interrupted = false;

  for (size_t i = 0; i < totalRead; i++) {
    char c = textBuffer[i];
    if (c == ' ' or c == '\n' or c == '\r') {
      if (currentWord.length() > 0) {
        if (processAndDisplayWord(currentWord, speedMs, bgPath)) {
          interrupted = true;
          break;
        }
        currentWord = "";
      }
    } else {
      currentWord += c;
    }
  }

  if (!interrupted && currentWord.length() > 0) {
    processAndDisplayWord(currentWord, speedMs, bgPath);
  }

  digitalWrite(TFT_CS, HIGH);
  free(textBuffer);
}

void displayStaticText(String headerText, String bodyText, String bgPath) {
  if (bgPath.length() > 0) {
    drawIcon(bgPath, 0, 0, TFT_WIDTH, TFT_HEIGHT);
  } else {
    tft.fillScreen(ST77XX_BLACK);
  }

  int marginX = 16;
  int maxWidth = TFT_WIDTH - (marginX * 2);
  int bodyLineHeight = 24;
  int headerSpacing = 36;

  // --- PASS 1: Pre-calculate total height for vertical centering ---
  tft.setFont(&FreeSerifItalic12pt7b);
  tft.setTextSize(1);

  // Dynamically calculate accurate space character width for 12pt font
  int16_t x1, y1;
  uint16_t w1, w2, h;
  tft.getTextBounds("a a", 0, 0, &x1, &y1, &w1, &h);
  tft.getTextBounds("aa", 0, 0, &x1, &y1, &w2, &h);
  int spaceWidth = w1 - w2; 

  int lineCount = 1;
  int testX = marginX;
  String currentWord = "";

  for (size_t i = 0; i < bodyText.length(); i++) {
    char c = bodyText[i];
    if (c == ' ' or c == '\n' or i == bodyText.length() - 1) {
      if (i == bodyText.length() - 1 && c != ' ' && c != '\n') {
        currentWord += c;
      }
      if (currentWord.length() > 0) {
        uint16_t w;
        tft.getTextBounds(currentWord, 0, 0, &x1, &y1, &w, &h);

        if (testX + w > marginX + maxWidth) {
          lineCount++;
          testX = marginX;
        }
        testX += w + spaceWidth; // Include calculated space width
        currentWord = "";
      }
      if (c == '\n') {
        lineCount++;
        testX = marginX;
      }
    } else {
      currentWord += c;
    }
  }

  int totalBodyHeight = lineCount * bodyLineHeight;
  int totalBlockHeight = (headerText.length() > 0 ? headerSpacing : 0) + totalBodyHeight;

  int startY = (TFT_HEIGHT - totalBlockHeight) / 2;
  if (startY < 8) startY = 8;

  // --- PASS 2: Render Header & Body Text ---
  int cursorY = startY;

  // 1. Draw Location Reference Header
  if (headerText.length() > 0) {
    String cleanHeader = headerText;
    cleanHeader.replace('_', ' '); // Automatically converts "1_Corinthians" -> "1 Corinthians"
    tft.setFont(&FreeSerifBold18pt7b);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(marginX, cursorY + 20);
    tft.print(cleanHeader);
    cursorY += headerSpacing;
  }

  // 2. Draw Scripture Body Paragraph
  tft.setFont(&FreeSerifItalic12pt7b);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  int cursorX = marginX;
  int bodyBaselineY = cursorY + 16;

  currentWord = "";
  for (size_t i = 0; i < bodyText.length(); i++) {
    char c = bodyText[i];
    if (c == ' ' || c == '\n' || i == bodyText.length() - 1) {
      if (i == bodyText.length() - 1 && c != ' ' && c != '\n') {
        currentWord += c;
      }
      if (currentWord.length() > 0) {
        uint16_t w;
        tft.getTextBounds(currentWord, 0, 0, &x1, &y1, &w, &h);

        // Word-wrap if text exceeds screen margin
        if (cursorX + w > marginX + maxWidth) {
          cursorX = marginX;
          bodyBaselineY += bodyLineHeight;
        }

        tft.setCursor(cursorX, bodyBaselineY);
        tft.print(currentWord);
        cursorX += w + spaceWidth; // Add word width PLUS space width
        currentWord = "";
      }
      if (c == '\n') {
        cursorX = marginX;
        bodyBaselineY += bodyLineHeight;
      }
    } else {
      currentWord += c;
    }
  }

  // 3. Static hold until user presses button
  while (true) {
    if (isButtonPressed() || isExtraButtonPressed()) {
      tft.setFont();
      return;
    }
    delay(10);
  }
}

bool readPaginatedText(String textPath, String headerText) {
  digitalWrite(TFT_CS, HIGH);
  File txtFile = openWithRetry(textPath);
  digitalWrite(SD_CS, HIGH);

  if (!txtFile) {
    showMessage("File missing", ST77XX_RED);
    delay(1000);
    return false; // Failed to open
  }

  size_t fileSize = txtFile.size();
  char *textBuffer = (char *)malloc(fileSize + 1);
  if (!textBuffer) {
    txtFile.close();
    digitalWrite(SD_CS, HIGH);
    return false;
  }
  
  size_t totalRead = txtFile.read((uint8_t *)textBuffer, fileSize);
  textBuffer[totalRead] = '\0';
  txtFile.close();
  digitalWrite(SD_CS, HIGH);

  String bodyText = String(textBuffer);
  free(textBuffer);

  String cleanHeader = headerText;
  cleanHeader.replace('_', ' ');
  int dotIndex = cleanHeader.lastIndexOf('.');
  if (dotIndex > 0) cleanHeader = cleanHeader.substring(0, dotIndex);

  int marginX = 12; 
  int maxWidth = TFT_WIDTH - (marginX * 2);
  int bodyLineHeight = 16; 
  int maxBottomY = TFT_HEIGHT - 10;

  tft.setFont(&FreeSerifItalic9pt7b);
  tft.setTextSize(1);
  int16_t x1, y1; uint16_t w1, w2, h;
  tft.getTextBounds("a a", 0, 0, &x1, &y1, &w1, &h);
  tft.getTextBounds("aa", 0, 0, &x1, &y1, &w2, &h);
  int spaceWidth = w1 - w2;

  int currentIdx = 0;
  int textLen = bodyText.length();
  bool isFirstPage = true; 

  while (currentIdx < textLen) {
    tft.fillScreen(ST77XX_BLACK);
    
    int cursorY = 35; 

    if (cleanHeader.length() > 0 && isFirstPage) {
      tft.setFont(&FreeSerifBold12pt7b);
      tft.setTextColor(ST77XX_WHITE);
      tft.setCursor(marginX, cursorY + 16); 
      tft.print(cleanHeader);
      cursorY += 28; 
    }

    tft.setFont(&FreeSerifItalic9pt7b);
    tft.setTextColor(ST77XX_WHITE);
    int cursorX = marginX;
    int bodyBaselineY = cursorY + 12; 

    String currentWord = "";
    bool pageFull = false;

    while (currentIdx < textLen && !pageFull) {
      char c = bodyText[currentIdx];
      
      if (c == ' ' || c == '\n' || currentIdx == textLen - 1) {
        if (currentIdx == textLen - 1 && c != ' ' && c != '\n') {
          currentWord += c;
        }
        if (currentWord.length() > 0) {
          uint16_t w;
          tft.getTextBounds(currentWord, 0, 0, &x1, &y1, &w, &h);

          if (cursorX + w > marginX + maxWidth) {
            cursorX = marginX;
            bodyBaselineY += bodyLineHeight;
          }

          if (bodyBaselineY > maxBottomY) {
            pageFull = true;
            break; 
          }

          tft.setCursor(cursorX, bodyBaselineY);
          tft.print(currentWord);
          cursorX += w + spaceWidth;
          currentWord = "";
        }
        
        if (!pageFull && c == '\n') {
          cursorX = marginX;
          bodyBaselineY += bodyLineHeight;
          if (bodyBaselineY > maxBottomY) {
             pageFull = true;
             currentIdx++; 
             break;
          }
        }
      } else {
        currentWord += c;
      }
      if (!pageFull) {
        currentIdx++;
      }
    }

    isFirstPage = false; 

    bool waiting = true;
    while (waiting) {
      if (isButtonPressed()) waiting = false; // Move to next page
      if (isExtraButtonPressed()) {
        tft.setFont();
        return false; // Exit immediately, user pressed Back
      }
      delay(10);
    }
  }
  
  tft.setFont();
  return true; // Exit successfully, user advanced past the final page
}

void readRandomScripture(String textPath, int speedMs, String bgPath) {
  digitalWrite(TFT_CS, HIGH);
  File txtFile = openWithRetry(textPath);
  digitalWrite(SD_CS, HIGH);

  if (!txtFile) {
    Serial.print("Scripture file missing: ");
    Serial.println(textPath);
    return;
  }

  size_t fileSize = txtFile.size();
  char *textBuffer = (char *)malloc(fileSize + 1);
  if (!textBuffer) {
    txtFile.close();
    digitalWrite(SD_CS, HIGH);
    return;
  }
  
  size_t totalRead = txtFile.read((uint8_t *)textBuffer, fileSize);
  textBuffer[totalRead] = '\0';
  txtFile.close();
  digitalWrite(SD_CS, HIGH);

  String fullContent = String(textBuffer);
  free(textBuffer); // Deallocate memory early

  // Count scripture entries using "---"
  int totalEntries = 1;
  int pos = 0;
  while ((pos = fullContent.indexOf("\n---", pos)) != -1) {
    totalEntries++;
    pos += 4;
  }

  int targetIndex = esp_random() % totalEntries;

  // Extract chosen scripture entry
  int currentIdx = 0;
  int startPos = 0;
  int endPos = fullContent.length();

  pos = 0;
  while (currentIdx < targetIndex) {
    int delim = fullContent.indexOf("\n---", pos);
    if (delim == -1) break;
    startPos = delim + 4;
    pos = startPos;
    currentIdx++;
  }

  int nextDelim = fullContent.indexOf("\n---", startPos);
  if (nextDelim != -1) {
    endPos = nextDelim;
  }

  String selectedEntry = fullContent.substring(startPos, endPos);
  selectedEntry.trim();

  // Separate line 1 (Reference) from body text
  int firstLineEnd = selectedEntry.indexOf('\n');
  String locationRef = "";
  String bodyText = "";

  if (firstLineEnd != -1) {
    locationRef = selectedEntry.substring(0, firstLineEnd);
    locationRef.trim();
    bodyText = selectedEntry.substring(firstLineEnd + 1);
    bodyText.trim();
  } else {
    bodyText = selectedEntry; // Fallback if no header line present
  }

  // Render full static block onto the display
  displayStaticText(locationRef, bodyText, bgPath);
}

void readTextFile(int index, int speedMs) {
  String path = "/gallery/" + String(index) + ".txt";
  readTextFileCustom(path, speedMs, ""); // Reads standard gallery text with black background
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

