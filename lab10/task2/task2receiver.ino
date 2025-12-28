// Lab 10 - Task 2: Basic UART Communication (Receiver)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives bytes over UART at 9600 baud and controls LEDs based on value
// Receives '1', '2', or '3' and lights corresponding LED (LED1, LED2, or LED3)
// Uses register-level UART programming (no Serial library)

// LED pin definitions on PORTD
#define LED1 PD2
#define LED2 PD3
#define LED3 PD4

// Initialize UART for 9600 baud at 16MHz
// Baud rate calculation: UBRR = (F_CPU / (16 * BAUD)) - 1
// UBRR = (16000000 / (16 * 9600)) - 1 = 103.166... ≈ 103
void UART_init(void) {
    UBRR0H = 0;  // High byte of baud rate register
    UBRR0L = 103;  // Low byte: 103 for 9600 baud @ 16MHz

    UCSR0A = 0x00;  // Normal speed, no double speed
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit data, 1 stop bit, no parity
}

// Receive a byte from UART
// Waits until data is received (RXC0 flag set)
uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));  // Wait for receive complete flag
    return UDR0;  // Read data from UART data register
}

// Send a byte over UART
// Waits until transmit buffer is empty (UDRE0 flag set)
void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for data register empty flag
    UDR0 = d;  // Write data to UART data register
}

void setup() {
    UART_init();
    // Configure LED pins as outputs
    DDRD |= (1<<LED1) | (1<<LED2) | (1<<LED3);
}

void loop() {
    uint8_t r = UART_recv();  // Receive byte from transmitter
  	UART_send(r);  // Echo back to transmitter for verification
    
    // Control LEDs based on received value
    if (r == '1' || r == '2' || r == '3') {
      // Turn off all LEDs first
      PORTD &= ~((1<<LED1)|(1<<LED2)|(1<<LED3));

      // Turn on corresponding LED
      if (r == '1') PORTD |= (1<<LED1);  // LED1 for value '1'
      if (r == '2') PORTD |= (1<<LED2);  // LED2 for value '2'
      if (r == '3') PORTD |= (1<<LED3);  // LED3 for value '3'
    }
}