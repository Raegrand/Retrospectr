#include "../header/encoder.h"
#include "../header/config.h"

// Variables modified by the interrupt must be declared 'volatile'
volatile int encoderCount = 0;
volatile uint8_t prev_state = 0;

// Quadrature state machine table. 
// Valid forward movements = 1, backward = -1. Invalid jumps (noise) = 0.
static const int8_t encoder_table[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

void IRAM_ATTR readEncoderISR() {
  // Shift the previous state to the left
  prev_state <<= 2;
  
  // Read current pin states
  uint8_t clk = digitalRead(ROTARY_CLK);
  uint8_t dt = digitalRead(ROTARY_DT);
  
  // Combine into a 2-bit number and add to previous state
  prev_state |= ((clk << 1) | dt);
  
  // Look up the score in the table and add it to the total
  encoderCount += encoder_table[prev_state & 0x0F];
}

void initEncoder() {
  pinMode(ROTARY_CLK, INPUT_PULLUP);
  pinMode(ROTARY_DT, INPUT_PULLUP);
  pinMode(ROTARY_SW, INPUT_PULLUP);
  pinMode(EXTRA_BTN, INPUT_PULLUP); 
  
  // Initialize starting state
  uint8_t clk = digitalRead(ROTARY_CLK);
  uint8_t dt = digitalRead(ROTARY_DT);
  prev_state = (clk << 1) | dt;

  // Track EVERY change on BOTH pins to map the exact physical movement
  attachInterrupt(digitalPinToInterrupt(ROTARY_CLK), readEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_DT), readEncoderISR, CHANGE);
}

int getEncoderScroll() {
  int direction = 0;
  
  // Most KY-040 encoders register 4 internal state changes per physical "click"
  if (encoderCount >= 4) {
    direction = 1;
    encoderCount -= 4; // Subtract rather than reset to 0 to preserve fast scrolling
  } else if (encoderCount <= -4) {
    direction = -1;
    encoderCount += 4; 
  }
  
  return direction;
}

bool isButtonPressed() {
  if (digitalRead(ROTARY_SW) == LOW) {
    delay(50); 
    if (digitalRead(ROTARY_SW) == LOW) {
      while(digitalRead(ROTARY_SW) == LOW) { delay(10); } 
      return true;
    }
  }
  return false;
}

bool isExtraButtonPressed() {
  if (digitalRead(EXTRA_BTN) == LOW) {
    delay(50); 
    if (digitalRead(EXTRA_BTN) == LOW) {
      while(digitalRead(EXTRA_BTN) == LOW) { delay(10); } 
      return true;
    }
  }
  return false;
}