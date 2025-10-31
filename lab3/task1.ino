
uint8_t ledState = 0;

void setup() {
  DDRB |= (1 << PB5);
  PORTB |= (1 << PB5);
}

void loop() {
  if (!ledState) {
    PORTB = 0b00100000;
  } else {
    PORTB = 0b00000000;
  }
  ledState = !ledState;
  
  delay(500);
}

void loop() {
  if (!ledState) {
    PORTB |= (1 << PB5);
  } else {
    PORTB &= ~(1 << PB5);
  }
  ledState = !ledState;
  delay(500);
}

void loop() {
  PORTB ^= (1 << PB5);
  delay(500);
}

void loop() {
  if (!ledState) {
    PORTB <<= 1;
  } else {
    PORTB >>= 1;
  }
  ledState = !ledState;
  delay(500);
}
