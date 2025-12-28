// Lab 12 - Task 6: I2C Speed Testing (Slave)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: I2C slave that dynamically changes speed based on master commands.
// Receives speed value (1, 2, or 3) and reconfigures I2C clock accordingly.
// Must match master speed for proper communication.
// Speed 1 = 25 kHz, Speed 2 = 100 kHz, Speed 3 = 400 kHz

#include <Wire.h>

volatile unsigned long lastTime = 0;  // Last button press time for debouncing
volatile uint8_t lastButtonState = (1 << PD2);  // Previous button state
volatile uint8_t lastMessage = 1;  // Last received message value

bool masterButtonState = false;  // Button state to send to master

// I2C receive callback: called when master writes data to slave
void onReceive(int bytes)
{
  while (Wire.available()) {
    uint8_t v = Wire.read();
    if(v==1){
      // Configure for 25 kHz (slow mode)
      TWBR = ((16000000/25000) - 16) /(2*4);
      TWSR = 0b00000001;  // Prescaler = 4
    }else if(v==2){
      // Configure for 100 kHz (standard mode)
      TWBR = ((16000000/100000) - 16) /(2);
      TWSR = 0b00000000;  // Prescaler = 1
    }else if(v==3){
      // Configure for 400 kHz (fast mode)
      TWBR = ((16000000/400000) - 16) /(2);
      TWSR = 0b00000000;  // Prescaler = 1
    }else{
      // Normal data message: toggle LED
      PORTB ^= (1 << PB5);
    }
  }
}

// I2C request callback: called when master requests data from slave
void onRequest()
{
  // Send button state to master: 1 if pressed, 0 if not pressed
  if(masterButtonState){
    Wire.write(1);
  }else{
    Wire.write(0);
  }
}

void setup()
{
  // Configure button pin (PD2) as input with pull-up
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  // Configure LED pin (PB5) as output
  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5);

  Wire.begin(4);  // Initialize I2C as slave with address 4
  
  // Initialize to 25 kHz (slow mode)
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);
  TWBR = ((16000000/25000) - 16) /(2*4);
  TWSR = 0b00000001;  // Prescaler = 4

  // Register I2C event handlers
  Wire.onReceive(onReceive);  // Called when master writes to slave
  Wire.onRequest(onRequest);  // Called when master reads from slave
}

void loop()
{
    uint8_t buttonState = (PIND & (1 << PD2)); 
    // Detect falling edge (button press)
    if(lastButtonState && !buttonState){
      unsigned long now = millis();
      if (now - lastTime > 250) {  // Debounce
        masterButtonState = !masterButtonState;  // Toggle button state
      }
      lastTime = now;
    }
    lastButtonState = buttonState;
}
    