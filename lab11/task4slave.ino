#include <avr/io.h>

uint8_t prevVal = 0;

void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_sendByte(uint8_t b) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = b;
}

void UART_sendString(const char *str) {
    while (*str) {
        UART_sendByte(*str++);
    }
}

void UART_sendNumber(uint8_t num) {
    char buf[4];
    int len = 0;

    if (num == 0) {
        UART_sendByte('0');
        return;
    }

    while (num > 0) {
        buf[len++] = '0' + (num % 10);
        num /= 10;
    }

    for (int i = len - 1; i >= 0; i--) {
        UART_sendByte(buf[i]);
    }
}

void SPI_SlaveInit(void) {
    DDRB |= (1 << PB4);
    DDRB &= ~(1 << PB2);
    PORTB |= (1 << PB2);

    DDRB &= ~(1 << PB3);
    DDRB &= ~(1 << PB5);
    SPCR = (1 << SPE);
}

uint8_t SPI_SlaveReceive(void) {
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

int main(void) {
    UART_init();
    SPI_SlaveInit();

    while (1) {
        uint8_t val = SPI_SlaveReceive();
        if(prevVal != val){
            UART_sendNumber(val);
            UART_sendByte('\n');
            prevVal = val;
        } 
    }

    return 0;
}