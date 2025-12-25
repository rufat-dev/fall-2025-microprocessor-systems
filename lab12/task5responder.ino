#include <Wire.h>

volatile unsigned long lastTime = 0;
volatile uint8_t lastButtonState = (1 << PD2);

bool masterButtonState = false;

void onReceive(int bytes)
{
  while (Wire.available()) {
    uint8_t v = Wire.read();
    if (v == 1) {
      PORTB ^= (1 << PB5);
    }
  }
}

void onRequest()
{
  if(masterButtonState){
    Wire.write(1);
  }else{
    Wire.write(0);
  }
}

void setup()
{
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5);

  Wire.begin(4);
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);
  TWBR = ((16000000/25000) - 16) /(2*4);
  TWSR = 0b00000001;   

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

}

void loop()
{
    uint8_t buttonState = (PIND & (1 << PD2)); 
    if(lastButtonState && !buttonState){
      unsigned long now = millis();
      if (now - lastTime > 250) {
        masterButtonState = !masterButtonState;
      }
      lastTime = now;
    }
    lastButtonState = buttonState;

}
    