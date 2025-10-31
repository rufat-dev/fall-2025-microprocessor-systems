#include <EEPROM.h>

//variable to store the previous state of the button
uint8_t lastButtonState = 0;

//variable to store the count value
uint8_t countVal = 0;

//address in eeprom where we store the count
int countAdress = 0;

void setup() {
  //initializing serial monitor
  Serial.begin(9600); 

  //setting PB4 as input for the button
  DDRB &= ~(1 << PB4);
  //setting PB5 as output for the led
  DDRB |= (1 << PB5);

  //reading the count value from eeprom
  EEPROM.get(countAdress,countVal);

  Serial.print("Initial read float from EEPROM: ");
  Serial.println(countVal);
}

void loop() {
  //reading the current state of the button
  uint8_t buttonState = PINB & (1 << PB4);
  //checking if button was pressed
  if (buttonState && !lastButtonState){
    //incrementing countVal using assembly
    asm volatile (
      "lds r24, %A0\n\t"
      "inc r24 \n\t"   
      "sts %A0, r24\n\t"
      :
      : "m" (countVal)
      : "r24"
    );
    //toggling the led
    PORTB ^= (1 << PB5);
    //saving the incremented value to eeprom
    EEPROM.put(countAdress,countVal);
    Serial.print("Initial read float from EEPROM: ");
    Serial.println(countVal);
  }
  //updating the last button state
  lastButtonState = buttonState; 
}
