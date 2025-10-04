#include <EEPROM.h>

  uint8_t lastButtonState = 0;
  // lastButton state is there so that we can properly read the button input and distinguish whether
  // it is high or low.

  uint8_t countVal = 0;
  // to assign the single byte integer after we read it from EEPROM memory

  int countAdress = 0;
  // The address at which we read and write our integer

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  // to read and write from the serial monitor we open a port with the serial.begin method

  DDRB &= ~(1 << PB4);
  // setting the pin 12 with the value 0 in the DDRB meaning it is an input
  PORTB |= (1 << PB4);
  // We set the initial value high so that we can see the led is lit up on startup
  DDRB |= (1 << PB5);
  // We set pin 13 as output so we can observe when the button is clicked from the onboard led which is connected to the pin 13


  EEPROM.get(countAdress,countVal);
  // by passing the address and the variable which we want the assign the read value we are reading the value at that
  // specific adress in the eeprom
  

  Serial.print("Initial read float from EEPROM: ");
  Serial.println(countVal);
  // just checking to see if we still have the last value properly saved in the startup
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
      
      asm volatile (
        "lds r24, %A0\n\t"
        // we use lds meaning immeadite set register 24 to %A0 which is the gcc compile input for the value of countVal
        // it will set the value of register 24 to the single byte integer stored in countVal 
        "inc r24 \n\t"   
        //inc r24 will increment the value stored in register 24
        "sts %A0, r24\n\t"
        // it will store the value back in countVal 
        :
        : "m" (countVal)
        : "r24"
      );
      PORTB ^= (1 << PB5);
      // it will invert the value of the pin 13 so that the led will visually tell us that the button has been clicked and the number incremented

      EEPROM.put(countAdress,countVal);
      // we modify the value held in the memory with the increment version of countVal at the same adress. 
      Serial.print("Initial read float from EEPROM: ");
      Serial.println(countVal);
      // we print the value so we can check it has been successfully incremented
      delay(50);
      // we added a delay so that button clicks do not interupt each other
    }
  }
  lastButtonState = buttonState; 
  // we update the button state so that the button doesnt modify more times than it should per click
  // if we didnt store and compare the last state then holding the button would increment more than 1 times
  // as the pin PINB & (1 << PB4) would stay true for multiple clock cycles then just one 
  delay(50);
}
