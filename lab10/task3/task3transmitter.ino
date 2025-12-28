// Lab 10 - Task 3: UART Pattern Storage and Playback (Transmitter)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives bytes from PC Serial Monitor and forwards them to receiver.
// Forwards values '1', '2', '3' (for storage) or '4' (to trigger playback)
// Uses register-level UART programming

#define LED1 PD2
#define LED2 PD3
#define LED3 PD4

// Initialize UART for 9600 baud at 16MHz
void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud @ 16MHz

    UCSR0A = 0x00;  // Normal speed
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // Enable RX & TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit, 1 stop, no parity
}

// Receive a byte from UART (from PC Serial Monitor)
uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));  // Wait for receive complete
    return UDR0;
}

// Send a byte over UART (to receiver Arduino)
void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for transmit buffer empty
    UDR0 = d;
}

void setup() {
    UART_init();
    DDRD |= (1<<LED1) | (1<<LED2) | (1<<LED3);  // Configure LEDs as outputs
}

void loop() {
    uint8_t c = UART_recv();  // Read input from PC Serial Monitor

    // Forward only valid values to receiver
    if (c == '1' || c == '2' || c == '3' || c == '4') {
        UART_send(c);  // Send to receiver Arduino
    }
}