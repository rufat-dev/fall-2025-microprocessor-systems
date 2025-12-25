
#include <Wire.h>
#include <util/delay.h>

unsigned long startTime;
unsigned long endTime;

#define MASTER_BUTTON  PD2
#define MASTER_LED_PB5  PB5

volatile unsigned long lastTime = 0;
volatile uint8_t lastButtonState ;
volatile uint8_t speedIncrement = 1;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);

  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5); 
  lastButtonState = (PIND & (1 << PD2));
  delay(500);

}

void send50Messages(uint8_t speed) {

  Wire.beginTransmission(4);
  Wire.write(speed);
  Wire.endTransmission();
  delay(100);
  Serial.println("Starting 50-message timing test...");
  startTime = micros();
  for (int i = 0; i < 50; i++) {
    Wire.beginTransmission(4);
    Wire.write(0);
    Wire.endTransmission();

    Wire.requestFrom(4, 1);
    if (Wire.available()) {
      int event = Wire.read();
      PORTB ^= (1 << PB5);
    }
  }
  endTime = micros();
  Serial.print("Time for 50 messages: ");
  Serial.print(endTime - startTime);
  Serial.println(" micros");
  PORTB &= ~(1 << PB5);
}

void loop() {

  uint8_t buttonState = (PIND & (1 << PD2)); 
  if(lastButtonState && !buttonState){
    unsigned long now = millis();
    if (now - lastTime > 250) {
      if(speedIncrement == 1){
        TWBR = ((16000000/25000) - 16) /(2*4);
        TWSR = 0b00000001;
        send50Messages(1);
        speedIncrement++;
        PORTB &= ~(1 << PB5);
      }else if(speedIncrement == 2){
        TWBR = ((16000000/100000) - 16) /(2);
        TWSR = 0b00000000;
        send50Messages(2);
        speedIncrement++;
        PORTB &= ~(1 << PB5);
      }else if(speedIncrement == 3){
        TWBR = ((16000000/400000) - 16) /(2);
        TWSR = 0b00000000;
        send50Messages(3);
        speedIncrement=1;
        PORTB &= ~(1 << PB5);
      }
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
