
#include <EEPROM.h>

  uint8_t lastButtonState = 0;

  uint8_t countVal = 0;

  int countAdress = 0;

void setup() {
  Serial.begin(9600); 

  DDRB &= ~(1 << PB4);
  DDRB |= (1 << PB5);

  EEPROM.get(countAdress,countVal);

  Serial.print("Initial read float from EEPROM: ");
  Serial.println(countVal);
}

void loop() {
  uint8_t buttonState = PINB & (1 << PB4);
  if (buttonState && !lastButtonState){
    asm volatile (
      "lds r24, %A0\n\t"
      "inc r24 \n\t"   
      "sts %A0, r24\n\t"
      :
      : "m" (countVal)
      : "r24"
    );
    PORTB ^= (1 << PB5);
    EEPROM.put(countAdress,countVal);
    Serial.print("Initial read float from EEPROM: ");
    Serial.println(countVal);
  }
  lastButtonState = buttonState; 
}
