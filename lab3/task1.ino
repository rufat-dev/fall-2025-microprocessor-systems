//the variable to check if the led is on or off.
uint8_t ledState = 0;

void setup() {
  //setting PB5 as output
  DDRB |= (1 << PB5);
  //setting PB5 high
  PORTB |= (1 << PB5);
}

void loop() {
    //we check if the led is on or off
  if (!ledState) {
    //setting PB5 high by changing all bits
    PORTB = 0b00100000;
  } else {
    //setting PB5 low by changing all bits
    PORTB = 0b00000000;
  }
  //inverting the led state since we did so by clicking the button
  ledState = !ledState;
  
  delay(500);
}

void loop() {
    //we check if the led is on or off
  if (!ledState) {
    //setting PB5 high by using a bit mask and or Assignment (changes only PB5)
    PORTB |= (1 << PB5);
  } else {
    //setting PB5 low by using a bit mask and a AND Inverted (changes only PB5 sets it to 0)
    PORTB &= ~(1 << PB5);
  }
  //inverting the led state since we did so by clicking the button
  ledState = !ledState;
  delay(500);
}

void loop() {
  //we dont need to check its state. XOR assignment ensures its the opposite of its current value
  PORTB ^= (1 << PB5);
  delay(500);
}

void loop() {
  if (!ledState) {
  //we shift left to make the value of PB5 0 
    PORTB <<= 1;
  } else {
  //we shift right to bring the value 1 on the left back from PB6 to PB5 which will make the led high
    PORTB >>= 1;
  }
  ledState = !ledState;
  delay(500);
}
