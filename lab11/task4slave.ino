// Lab 11 - Task 4: SPI Communication (Slave)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives bytes from SPI master and displays them on Serial Monitor.
// Uses register-level SPI and UART programming.
// SPI pins: MISO (PB4), MOSI (PB3), SCK (PB5), SS (PB2)

#include <avr/io.h>

uint8_t prevVal = 0;  // Previous received value (for change detection)

// Initialize UART for 9600 baud at 16MHz (for Serial Monitor output)
void UART_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud @ 16MHz
    UCSR0B = (1 << TXEN0);  // Enable transmitter only
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit, 1 stop, no parity
}

// Send a single byte over UART
void UART_sendByte(uint8_t b) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for transmit buffer empty
    UDR0 = b;
}

// Send a null-terminated string over UART
void UART_sendString(const char *str) {
    while (*str) {
        UART_sendByte(*str++);
    }
}

// Send a number as decimal ASCII string over UART
void UART_sendNumber(uint8_t num) {
    char buf[4];  // Buffer for digits (max 3 digits + null)
    int len = 0;

    if (num == 0) {
        UART_sendByte('0');
        return;
    }

    // Convert number to string (in reverse order)
    while (num > 0) {
        buf[len++] = '0' + (num % 10);
        num /= 10;
    }

    // Send digits in correct order (most significant first)
    for (int i = len - 1; i >= 0; i--) {
        UART_sendByte(buf[i]);
    }
}

// Initialize SPI in Slave mode
// Configures SPI control register (SPCR) and data direction register (DDRB)
void SPI_SlaveInit(void) {
    DDRB |= (1 << PB4);  // MISO output (slave sends data to master)
    DDRB &= ~(1 << PB2);  // SS input (slave select from master)
    PORTB |= (1 << PB2);  // Enable pull-up on SS

    DDRB &= ~(1 << PB3);  // MOSI input (slave receives data from master)
    DDRB &= ~(1 << PB5);  // SCK input (clock from master)
    SPCR = (1 << SPE);  // Enable SPI (slave mode by default when MSTR=0)
}

// Receive one byte via SPI (slave mode)
// Waits for master to send data and SS to go low
uint8_t SPI_SlaveReceive(void) {
    while (!(SPSR & (1 << SPIF)));  // Wait for reception complete flag
    return SPDR;  // Read data from SPI Data Register
}

int main(void) {
    UART_init();
    SPI_SlaveInit();

    while (1) {
        uint8_t val = SPI_SlaveReceive();  // Receive byte from SPI master
        // Only print if value changed (avoid duplicate output)
        if(prevVal != val){
            UART_sendNumber(val);  // Send received value as decimal number
            UART_sendByte('\n');   // Newline for readability
            prevVal = val;
        } 
    }
        
    return 0;
}