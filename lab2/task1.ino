
uint8_t lastButtonState = 0;

void setup() {
  DDRB &= ~(1 << PB4);
  DDRB |= (1 << PB5);
}

void loop() {
  
  uint8_t buttonState = PINB & (1 << PB4);
  if (buttonState && !lastButtonState){
    PORTB ^= (1 << PB5);
  }
  lastButtonState = buttonState; 
}
