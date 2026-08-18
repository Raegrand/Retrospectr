#define CLK_PIN D2
#define DT_PIN D3
#define SW_PIN D1

// 'volatile' tells the ESP32 this variable will be changed by an interrupt
volatile int counter = 0; 
volatile unsigned long lastEncoderTime = 0;

unsigned long lastButtonTime = 0;

// This function runs automatically in the background exactly when CLK drops
void IRAM_ATTR readEncoder() {
  unsigned long currentTime = millis();
  
  // Debounce: Only accept a turn if 5 milliseconds have passed since the last one
  if (currentTime - lastEncoderTime > 5) {
    // Read the DT pin to determine direction
    if (digitalRead(DT_PIN) != digitalRead(CLK_PIN)) {
      counter--; // Counter-Clockwise
    } else {
      counter++; // Clockwise
    }
    lastEncoderTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000); // Wait for USB connection
  
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);

  // Attach the interrupt to the CLK pin. 
  // FALLING means it triggers the moment the pin goes from HIGH to LOW.
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readEncoder, FALLING);
  
  Serial.println("Interrupt Encoder Ready!");
}

void loop() {
  // Only print the counter when it actually changes to keep the monitor clean
  static int lastCounter = 0;
  if (counter != lastCounter) {
    Serial.print("Current Position: ");
    Serial.println(counter);
    lastCounter = counter;
  }

  // Read button using millis() instead of delay() so it never blocks the encoder
  if (digitalRead(SW_PIN) == LOW) {
    unsigned long currentTime = millis();
    // 300ms debounce for the button
    if (currentTime - lastButtonTime > 300) { 
      Serial.println("Action: Button Clicked! 🔘");
      lastButtonTime = currentTime;
    }
  }
}