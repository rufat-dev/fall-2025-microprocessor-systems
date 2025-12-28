// Lab 10 - Task 3: UART Pattern Storage and Playback with EEPROM
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives sequence of values (1, 2, or 3) over UART and stores them in EEPROM.
// When '4' is received, plays back the stored sequence by lighting LEDs in order.
// Uses register-level UART and EEPROM programming

// LED pin definitions on PORTD
#define LED1 PD2
#define LED2 PD3
#define LED3 PD4

uint16_t index = 0;  // Current EEPROM write index (stores sequence length)

// Initialize UART for 9600 baud at 16MHz
// UBRR = (16000000 / (16 * 9600)) - 1 = 103
void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0A = 0x00;  // Normal speed
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // Enable RX & TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit, 1 stop, no parity
}

// Receive a byte from UART
uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));  // Wait for receive complete
    return UDR0;
}

// Write a byte to EEPROM at specified address
// Uses register-level EEPROM control (EECR, EEDR, EEAR)
void EEPROM_write(uint16_t addr, uint8_t data) {
    while (EECR & (1 << EEPE));  // Wait for previous write to complete
    EEAR = addr;  // Set EEPROM address register
    EEDR = data;  // Set EEPROM data register
    EECR |= (1 << EEMPE);  // Master write enable
    EECR |= (1 << EEPE);   // Start write operation
}

// Read a byte from EEPROM at specified address
uint8_t EEPROM_read(uint16_t addr) {
    while (EECR & (1 << EEPE));  // Wait for any write to complete
    EEAR = addr;  // Set EEPROM address register
    EECR |= (1 << EERE);  // Start read operation
    return EEDR;  // Return data from EEPROM data register
}

// Send a byte over UART
void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for transmit buffer empty
    UDR0 = d;
}

void setup() {
    UART_init();
    DDRD |= (1<<LED1) | (1<<LED2) | (1<<LED3);  // Configure LEDs as outputs
}

void loop() {
    uint8_t r = UART_recv();  // Receive byte from transmitter
  	UART_send(r);  // Echo back for verification
    _delay_ms(2);  // Small delay for UART stability
    
    if (r == '1' || r == '2' || r == '3') {
       // Store received value in EEPROM
       EEPROM_write(index, r);
      _delay_ms(2);  // Wait for EEPROM write to complete
      
      // Verify write by reading back
      uint8_t v = EEPROM_read(index);
      UART_send(v);  // Send verification byte
       index++;  // Increment storage index
      _delay_ms(2);
      
    }else if(r == '4'){
        // Playback mode: read all stored values and light LEDs in sequence
        for(uint16_t i = 0; i<index; i++){
            UART_send('0' + i);  // Send index number
             _delay_ms(2);
            
            uint8_t v = EEPROM_read(i);  // Read value from EEPROM
            UART_send(v);  // Send stored value
            _delay_ms(2);

            // Turn off all LEDs
            PORTD &= ~((1<<LED1)|(1<<LED2)|(1<<LED3));

            // Light corresponding LED based on stored value
            if (v == '1') PORTD |= (1<<LED1);
            if (v == '2') PORTD |= (1<<LED2);
            if (v == '3') PORTD |= (1<<LED3);

            _delay_ms(200);  // Display each value for 200ms
        }
        index = 0;  // Reset for new sequence
    }
}