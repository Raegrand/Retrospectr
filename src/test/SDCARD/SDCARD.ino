#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#define SD_CS     4


void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin();

  if (!SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println("SD init failed");
    return;
  }
  Serial.println("SD init OK");
}

void loop() {
  for (int i = 1; i <= 2; i++) {
    String path = "/gallery/" + String(i) + ".bin";
    File f = SD.open(path, FILE_READ);
    Serial.print(path);
    Serial.println(f ? " -> OK" : " -> FAILED");
    if (f) f.close();
    delay(200);
  }
}