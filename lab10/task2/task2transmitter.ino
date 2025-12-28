// Lab 10 - Task 2: Basic UART Communication (Transmitter)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives bytes from PC Serial Monitor over USB and forwards them
// to receiver Arduino over UART at 9600 baud. Only forwards valid values '1', '2', '3', or '4'
// Uses register-level UART programming (no Serial library)

#define F_CPU 16000000UL
#include <avr/io.h>

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

// Receive a byte from UART (from PC Serial Monitor via USB)
// Waits until data is received (RXC0 flag set)
uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));  // Wait for receive complete flag
    return UDR0;  // Read data from UART data register
}

// Send a byte over UART (to receiver Arduino)
// Waits until transmit buffer is empty (UDRE0 flag set)
void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for data register empty flag
    UDR0 = d;  // Write data to UART data register
}

void setup() {
    UART_init();
}

void loop() {
    uint8_t c = UART_recv();  // Read input from PC Serial Monitor

    // Accept only valid values and forward to receiver
    if (c == '1' || c == '2' || c == '3' || c == '4') {
        UART_send(c);  // Send to receiver Arduino over UART
    }
}