#include <Wire.h>
#include <util/delay.h>

#define MASTER_BUTTON  PD2
#define MASTER_LED_PB5  PB5

volatile unsigned long lastTime = 0;
volatile uint8_t lastButtonState ;

void setup() {

  Wire.begin();
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);
  TWBR = ((16000000/25000) - 16) /(2*4);
  TWSR = 0b00000001;

  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5); 
  lastButtonState = (PIND & (1 << PD2));
}


void loop() {

    uint8_t buttonState = (PIND & (1 << PD2)); 
    if(lastButtonState && !buttonState){
      unsigned long now = millis();
      if (now - lastTime > 250) {
        Wire.beginTransmission(4);
        Wire.write(1);
        Wire.endTransmission();
      }
      lastTime = now;
    }
    lastButtonState = buttonState;

    Wire.requestFrom(4, 1);
    if (Wire.available()) {
      int event = Wire.read();
      if (event == 1) {
        PORTB |= (1 << PB5);
      }else{
        PORTB &= ~(1 << PB5);
      }
    }

}
