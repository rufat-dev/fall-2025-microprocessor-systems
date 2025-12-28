// Lab 12 - Task 5: I2C Communication (Controller/Master)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: I2C master that communicates with slave (address 4).
// When button on master is pressed, sends message to slave to toggle its LED.
// When slave button is pressed, slave sends state back and master lights its LED.
// I2C speed: 25 kHz (slow mode for reliability)

#include <Wire.h>
#include <util/delay.h>

#define MASTER_BUTTON  PD2
#define MASTER_LED_PB5  PB5

volatile unsigned long lastTime = 0;  // Last button press time for debouncing
volatile uint8_t lastButtonState;    // Previous button state for edge detection

void setup() {
  Wire.begin();  // Initialize I2C as master
  
  // Configure I2C clock speed to 25 kHz
  // TWSR prescaler bits: clear TWPS1, set TWPS0 (prescaler = 4)
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);
  // TWBR calculation: TWBR = ((F_CPU / I2C_FREQ) - 16) / (2 * prescaler)
  // TWBR = ((16000000 / 25000) - 16) / (2 * 4) = 79
  TWBR = ((16000000/25000) - 16) /(2*4);
  TWSR = 0b00000001;  // Prescaler = 4 (TWPS0=1, TWPS1=0)

  // Configure button pin (PD2) as input with pull-up
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  // Configure LED pin (PB5) as output
  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5);  // LED off initially
  lastButtonState = (PIND & (1 << PD2));  // Read initial button state
}

void loop() {
    uint8_t buttonState = (PIND & (1 << PD2)); 
    // Detect falling edge (button press: high to low transition)
    if(lastButtonState && !buttonState){
      unsigned long now = millis();
      // Debounce: ignore presses within 250ms of last press
      if (now - lastTime > 250) {
        // Send message to slave (address 4) to toggle its LED
        Wire.beginTransmission(4);
        Wire.write(1);  // Message value 1 = toggle command
        Wire.endTransmission();
      }
      lastTime = now;
    }
    lastButtonState = buttonState;

    // Request button state from slave (address 4)
    Wire.requestFrom(4, 1);
    if (Wire.available()) {
      int event = Wire.read();
      // If slave button is pressed (event == 1), turn on master LED
      if (event == 1) {
        PORTB |= (1 << PB5);  // LED on
      }else{
        PORTB &= ~(1 << PB5);  // LED off
      }
    }
}
