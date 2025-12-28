// Lab 12 - Task 6: I2C Speed Testing (Master)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Tests I2C communication at three different speeds:
// 1. 25 kHz (slow mode)
// 2. 100 kHz (standard mode)
// 3. 400 kHz (fast mode)
// Measures time to send and receive 50 messages at each speed.
// Button press cycles through speeds and runs timing test.

#include <Wire.h>
#include <util/delay.h>

unsigned long startTime;  // Start time for timing measurement
unsigned long endTime;    // End time for timing measurement

#define MASTER_BUTTON  PD2
#define MASTER_LED_PB5  PB5

volatile unsigned long lastTime = 0;  // Last button press time for debouncing
volatile uint8_t lastButtonState;     // Previous button state
volatile uint8_t speedIncrement = 1;  // Current speed mode (1, 2, or 3)

void setup() {
  Serial.begin(9600);
  Wire.begin();  // Initialize I2C as master
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  TWSR |= (1 << TWPS0);

  // Configure button pin (PD2) as input with pull-up
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  // Configure LED pin (PB5) as output
  DDRB |= (1 << PB5);
  PORTB &= ~(1 << PB5); 
  lastButtonState = (PIND & (1 << PD2));
  delay(500);  // Initial delay for stability
}

// Send 50 messages to slave and measure total time
// Each message consists of: write to slave, then read from slave
void send50Messages(uint8_t speed) {
  // First, notify slave of speed change
  Wire.beginTransmission(4);
  Wire.write(speed);  // Send speed value (1, 2, or 3)
  Wire.endTransmission();
  delay(100);  // Wait for slave to configure speed
  
  Serial.println("Starting 50-message timing test...");
  startTime = micros();  // Record start time
  
  // Send and receive 50 message pairs
  for (int i = 0; i < 50; i++) {
    // Write message to slave
    Wire.beginTransmission(4);
    Wire.write(0);  // Data value
    Wire.endTransmission();

    // Read response from slave
    Wire.requestFrom(4, 1);
    if (Wire.available()) {
      int event = Wire.read();
      PORTB ^= (1 << PB5);  // Toggle LED for each message
    }
  }
  
  endTime = micros();  // Record end time
  Serial.print("Time for 50 messages: ");
  Serial.print(endTime - startTime);
  Serial.println(" micros");
  PORTB &= ~(1 << PB5);  // Turn off LED
}

void loop() {
  uint8_t buttonState = (PIND & (1 << PD2)); 
  // Detect falling edge (button press)
  if(lastButtonState && !buttonState){
    unsigned long now = millis();
    if (now - lastTime > 250) {  // Debounce
      if(speedIncrement == 1){
        // Configure for 25 kHz (slow mode)
        // TWBR = ((F_CPU / I2C_FREQ) - 16) / (2 * prescaler)
        // TWBR = ((16000000 / 25000) - 16) / (2 * 4) = 79
        TWBR = ((16000000/25000) - 16) /(2*4);
        TWSR = 0b00000001;  // Prescaler = 4 (TWPS0=1, TWPS1=0)
        send50Messages(1);  // Run timing test
        speedIncrement++;
        PORTB &= ~(1 << PB5);
      }else if(speedIncrement == 2){
        // Configure for 100 kHz (standard mode)
        // TWBR = ((16000000 / 100000) - 16) / (2 * 1) = 72
        TWBR = ((16000000/100000) - 16) /(2);
        TWSR = 0b00000000;  // Prescaler = 1 (TWPS0=0, TWPS1=0)
        send50Messages(2);  // Run timing test
        speedIncrement++;
        PORTB &= ~(1 << PB5);
      }else if(speedIncrement == 3){  
        // Configure for 400 kHz (fast mode)
        // TWBR = ((16000000 / 400000) - 16) / (2 * 1) = 12
        TWBR = ((16000000/400000) - 16) /(2);
        TWSR = 0b00000000;  // Prescaler = 1
        send50Messages(3);  // Run timing test
        speedIncrement=1;  // Cycle back to speed 1
        PORTB &= ~(1 << PB5);
      }
    }
    lastTime = now;
  }
  lastButtonState = buttonState;

  // Normal operation: request button state from slave
  Wire.requestFrom(4, 1);
  if (Wire.available()) {
    int event = Wire.read();
    if (event == 1) {
      PORTB |= (1 << PB5);  // LED on if slave button pressed
    }else{
      PORTB &= ~(1 << PB5);  // LED off
    }
  }
}
