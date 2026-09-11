#include "../header/encoder.h"
#include "../header/config.h"

int lastClkState = HIGH;

void initEncoder() {
  pinMode(ROTARY_CLK, INPUT_PULLUP);
  pinMode(ROTARY_DT, INPUT_PULLUP);
  pinMode(ROTARY_SW, INPUT_PULLUP);
  
  pinMode(EXTRA_BTN, INPUT_PULLUP); 
  
  delay(10); 
  lastClkState = digitalRead(ROTARY_CLK);
}

int getEncoderScroll() {
  int direction = 0;
  int clkState = digitalRead(ROTARY_CLK);
  
  // If a change is detected
  if (clkState != lastClkState && clkState == HIGH) {
    delay(5); // Software debounce (wait 5ms to ignore noise)
    
    // Check if it's still HIGH after the noise settles
    if (digitalRead(ROTARY_CLK) == HIGH) {
      if (digitalRead(ROTARY_DT) != clkState) {
        direction = -1; 
      } else {
        direction = 1;  
      }
    }
  }
  lastClkState = clkState;
  return direction;
}

bool isButtonPressed() {
  if (digitalRead(ROTARY_SW) == LOW) {
    delay(50); // Button debounce
    if (digitalRead(ROTARY_SW) == LOW) {
      while(digitalRead(ROTARY_SW) == LOW) { delay(10); } 
      return true;
    }
  }
  return false;
}

bool isExtraButtonPressed() {
  if (digitalRead(EXTRA_BTN) == LOW) {
    delay(50); // Button debounce
    if (digitalRead(EXTRA_BTN) == LOW) {
      while(digitalRead(EXTRA_BTN) == LOW) { delay(10); } 
      return true;
    }
  }
  return false;
}