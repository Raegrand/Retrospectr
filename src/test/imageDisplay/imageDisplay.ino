#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SD.h>

// --- TFT & SD PINS ---
#define TFT_CS    5
#define TFT_DC    6
#define TFT_RST   -1
#define TFT_BL    7
#define SD_CS     4

// --- ROTARY ENCODER PINS ---
#define ROTARY_CLK 2  
#define ROTARY_DT  3  
#define ROTARY_SW  1  

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// --- GALLERY STATE VARIABLES ---
int totalImages = 0;
int lastClkState = HIGH;

// RTC_DATA_ATTR keeps this variable alive during Deep Sleep
RTC_DATA_ATTR int currentImage = 1; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  systemStartUp();
  
  // Render the image immediately upon waking up
  if (totalImages > 0) {
    drawBinFile(currentImage);
  } else {
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.print("No images");
  }
}

void loop() {
  if (totalImages == 0) return;

  // --- ROTARY ENCODER SCROLLING LOGIC ---
  int clkState = digitalRead(ROTARY_CLK);
  
  if (clkState != lastClkState && clkState == HIGH) {
    if (digitalRead(ROTARY_DT) != clkState) {
      currentImage--;
      if (currentImage < 1) currentImage = totalImages; 
    } else {
      currentImage++;
      if (currentImage > totalImages) currentImage = 1; 
    }
    drawBinFile(currentImage);
  }
  lastClkState = clkState;

  // --- ENCODER BUTTON PRESS LOGIC ---
  if (digitalRead(ROTARY_SW) == LOW) {
    delay(50); // Debounce
    if (digitalRead(ROTARY_SW) == LOW) {
      readTextFile(currentImage);
      while(digitalRead(ROTARY_SW) == LOW) { delay(10); } 
    }
  }
}

// --- START UP FUNCTION ---
void systemStartUp() {
  Serial.println("\n--- SYSTEM WAKE / START UP ---");

  pinMode(ROTARY_CLK, INPUT_PULLUP);
  pinMode(ROTARY_DT, INPUT_PULLUP);
  pinMode(ROTARY_SW, INPUT_PULLUP);
  lastClkState = digitalRead(ROTARY_CLK);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  // Initialize native bounds
  tft.init(240, 280); 
  // Set to Landscape to match the 280x240 image files
  tft.setRotation(1); 
  tft.fillScreen(ST77XX_BLACK);

  SPI.begin(8, 9, 10);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  delay(10);

  if (!SD.begin(SD_CS, SPI, 1000000)) {
    Serial.println("SD Card Failed!");
    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.print("SD Failed");
    return;
  }
  
  Serial.println("SD Card OK!");

  totalImages = countTotalImages();
  Serial.print("Total images found: ");
  Serial.println(totalImages);
}

// --- UTILITY FUNCTIONS ---

int countTotalImages() {
  int count = 0;
  while (true) {
    String path = "/gallery/" + String(count + 1) + ".bin";
    if (SD.exists(path)) {
      count++;
    } else {
      break; 
    }
  }
  return count;
}

void drawBinFile(int index) {
  String path = "/gallery/" + String(index) + ".bin";
  Serial.print("Drawing ");
  Serial.println(path);

  File binFile = SD.open(path, FILE_READ);
  if (!binFile) return;

  tft.startWrite();
  // Fixed window: 280 Width x 240 Height
  tft.setAddrWindow(0, 0, 280, 240);
  tft.endWrite();

  uint8_t sdbuffer[512];
  uint16_t pixelBuffer[256]; 

  while (binFile.available()) {
    int bytesRead = binFile.read(sdbuffer, sizeof(sdbuffer));
    int numPixels = bytesRead / 2;

    for (int i = 0; i < numPixels; i++) {
      // NOTE: If colors are inverted/neon, change to: 
      // pixelBuffer[i] = (sdbuffer[i*2] << 8) | sdbuffer[i*2 + 1];
      pixelBuffer[i] = (sdbuffer[i*2 + 1] << 8) | sdbuffer[i*2];
    }

    tft.startWrite();
    tft.writePixels(pixelBuffer, numPixels);
    tft.endWrite();
  }
  binFile.close();
}

void readTextFile(int index) {
  String path = "/gallery/" + String(index) + ".txt";
  if (!SD.exists(path)) return;

  File txtFile = SD.open(path, FILE_READ);
  if (!txtFile) return;

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);

  String currentWord = "";
  
  while (txtFile.available()) {
    char c = txtFile.read();
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
  txtFile.close();
  drawBinFile(index);
}

void displayWord(String word) {
  tft.fillScreen(ST77XX_BLACK); 
  
  // Recalculated for 280 screen width
  int textWidth = word.length() * 18;
  int cursorX = (280 - textWidth) / 2; 
  if (cursorX < 0) cursorX = 0; 
  
  // Centered vertically on a 240-tall screen
  tft.setCursor(cursorX, 100); 
  tft.print(word);
  
  delay(350); 
}