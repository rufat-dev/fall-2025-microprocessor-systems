//task 1
//
// Using a button as an input like a light switch, control the
// onboard LED. Pressing the button should toggle the LED ON and
// OFF. This must be implemented using registers, not Arduino
// functions.
uint8_t lastButtonState = 0;
  // lastButton state is there so that we can properly read the button input and distinguish whether
  // it is high or low.

void setup() {

  DDRB &= ~(1 << PB4);
  // setting the pin 12 with the value 0 in the DDRB meaning it is an input
  PORTB |= (1 << PB4);
  // We set the initial value high so that we can see the led is lit up on startup
  DDRB |= (1 << PB5);
  // We set pin 13 as output so we can observe when the button is clicked from the onboard led which is connected to the pin 13
}

void loop() {
  uint8_t buttonState = PINB & (1 << PB4);
  // we get the buttons current state so that we can compare with the last saved button state and see
  // if it was acutally pressed or is just being held which gives out a continuous stream of high value
  if (PINB & (1 << PB4)) {
    // we logic AND compare the whole pinb and the mask for pin 12 (00010000) if the pin 12 on PINB is high
    // the and statement will be true otherwise false since only the PB4 is selected as high in the mask. 
    //we check if the current state of the button is different than the previous value meaning the button was actually pressed 
    if (buttonState && !lastButtonState){
      PORTB ^= (1 << PB5);
      // it will invert the value of the pin 13 so that the led will be switched
      delay(50);
      // we added a delay so that button clicks do not interupt each other
    }
  }
  lastButtonState = buttonState; 
  // we update the button state so that the button doesnt modify more times than it should per click
  // as the pin PINB & (1 << PB4) would stay true for multiple clock cycles then just one 
  delay(50);
}
