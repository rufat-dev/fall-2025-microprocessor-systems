//task 1
//
// Blink the Arduino’s built-in LED using four different register-level
// methods.
// First, use direct assignment by writing full binary or hex values to the PORT register. Second, use
// bit set/clear (masking) with bitwise OR (|=) and AND (&= ~) to control the LED pin. Third, use
// bit toggle with XOR (^=) or the PINx register to flip the LED state. Finally, use shift operations
// (<< or >>) to move a bit across positions and apply it for blinking.

uint8_t method = 0; // Current method (0-3)
uint8_t ledState = 0; // LED state for blinking

void setup() {
  Serial.begin(9600);
  DDRB |= (1 << PB5); // Set pin 13 as output
}

//first method

void loop() {
  // Method 1: Direct assignment with binary/hex values
  if (ledState) {
    PORTB = 0b00100000; // Turn on LED with binary
  } else {
    PORTB = 0b00000000; // Turn off LED with binary
  }
  ledState = !ledState;
  
  delay(500); // Blink every 500ms
}

//second method

void loop() {
  if (ledState) {
    PORTB |= (1 << PB5);  // Set bit using OR
  } else {
    PORTB &= ~(1 << PB5); // Clear bit using AND with NOT
  }
  ledState = !ledState;
  delay(500); // Blink every 500ms
}

//third method

void loop() {
  PORTB ^= (1 << PB5); // Toggle LED using XOR
  ledState = !ledState;
  delay(500); // Blink every 500ms
}

//last method

void loop() {
  if (ledState) {
    PORTB <<= 1; // Shift PORTB left to turn off LED
  } else {
    PORTB >>= 1; // Shift PORTB right to turn on LED
  }
  ledState = !ledState;
  delay(500); // Blink every 500ms
}
