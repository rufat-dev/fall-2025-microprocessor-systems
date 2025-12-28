// Lab 12 - Task 5: I2C Communication (Responder/Slave)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: I2C slave (address 4) that responds to master commands.
// When master sends message, toggles slave LED.
// When master requests data, sends button state (1 if pressed, 0 if not).
// I2C speed: 25 kHz (must match master)

#include <Wire.h>

volatile unsigned long lastTime = 0;  // Last button press time for debouncing
volatile uint8_t lastButtonState = (1 << PD2);  // Previous button state (high = not pressed)

bool masterState = false;  // Button state to send to master (true = pressed)

// I2C receive callback: called when master writes data to slave
void onReceive(int bytes)
{
  while (Wire.available()) {
    uint8_t v = Wire.read();
    if (v == 1) {
      // Toggle LED when master sends value 1
      PORTB ^= (1 << PB5);
    }
  }
}

// I2C request callback: called when master requests data from slave
void onRequest()
{
  // Send button state to master: 1 if pressed, 0 if not pressed
  if(masterState){
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
  PORTB &= ~(1 << PB5);  // LED off initially

  Wire.begin(4);  // Initialize I2C as slave with address 4
  
  // Configure I2C clock speed to 25 kHz (must match master)
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
    // Detect falling edge (button press: high to low transition)
    if(lastButtonState && !buttonState){
      unsigned long now = millis();
      // Debounce: ignore presses within 250ms of last press
      if (now - lastTime > 250) {
        masterState = !masterState;  // Toggle button state flag
      }
      lastTime = now;
    }
    lastButtonState = buttonState;
}
    