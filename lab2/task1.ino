//Variable where we will store the button state
uint8_t lastButtonState = 0;


void setup() {
  //using the DDRB register we are setting the pin pb4 as input and pin pb5 as output
  DDRB &= ~(1 << PB4);
  DDRB |= (1 << PB5);
}

void loop() {
  //we read the current value of pin pb4 using a bitmask on PINB which will give us
  //the current state of the pin pb4 as the variable buttonState
  uint8_t buttonState = PINB & (1 << PB4);
  //We compare if the current pin value is high (button clicked) and previous is low
  if (buttonState && !lastButtonState){
    //if the button is indeed clicked we use xor to invert the value of pin pb5 if its high it will be low
    //and if its low it will be high
    PORTB ^= (1 << PB5);
  }
  //we update the last state for the next loop cycle.
  lastButtonState = buttonState; 
}
