#define F_CPU 16000000UL
#include <avr/io.h>

void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0A = 0x00;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = d;
}

void setup() {
    UART_init();
}

void loop() {
    uint8_t c = UART_recv();

    if (c == '1' || c == '2' || c == '3' || c == '4') {
        UART_send(c);
    }
}