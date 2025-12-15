#include <avr/io.h>

uint8_t prevVal = 0;

// UART init for 9600 baud
void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;                     // 9600 baud at 16 MHz
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_sendByte(uint8_t b) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = b;
}

// Send null-terminated string over UART
void UART_sendString(const char *str) {
    while (*str) {
        UART_sendByte(*str++);
    }
}

// Send number as decimal ASCII over UART
void UART_sendNumber(uint8_t num) {
    char buf[4];  // max 3 digits + null
    int len = 0;

    if (num == 0) {
        UART_sendByte('0');
        return;
    }

    // Convert number to string (in reverse)
    while (num > 0) {
        buf[len++] = '0' + (num % 10);
        num /= 10;
    }

    // Send digits in correct order
    for (int i = len - 1; i >= 0; i--) {
        UART_sendByte(buf[i]);
    }
}

// SPI slave init
void SPI_SlaveInit(void) {
    DDRB |= (1 << PB4);                 // MISO output
    DDRB &= ~(1 << PB2);                // SS input
    PORTB |= (1 << PB2);                // pull-up on SS

    DDRB &= ~(1 << PB3);                // MOSI input
    DDRB &= ~(1 << PB5);                // SCK input
    SPCR = (1 << SPE);                  // enable SPI
}

// Receive one byte via SPI (slave)
uint8_t SPI_SlaveReceive(void) {
    while (!(SPSR & (1 << SPIF)));     // wait for reception complete
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
            // newline for readability
            prevVal = val;
        } 
    }

    return 0;
}