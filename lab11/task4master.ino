// Lab 11 - Task 4: SPI Communication (Master)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Demonstrates SPI master-slave communication at register level.
// Master sends sequence 85 -> 170 -> 255 every second to slave.
// SPI pins: MOSI (PB3), SCK (PB5), SS (PB2), MISO (PB4)
// Clock speed: F_CPU/16 = 1MHz (SPR0=1, SPR1=0, SPI2X=0)

#include <avr/io.h>
#include <util/delay.h>

// Initialize SPI in Master mode
// Configures SPI control register (SPCR) and data direction register (DDRB)
void SPI_MasterInit(void) {
    // Configure SPI pins as outputs: MOSI (PB3), SCK (PB5), SS (PB2)
    DDRB |= (1<<PB3) | (1<<PB5) | (1<<PB2);
    // Set SS (Slave Select) high initially (idle state)
    PORTB |= (1<<PB2);

    // Configure SPI Control Register (SPCR):
    // SPE = 1: Enable SPI
    // MSTR = 1: Master mode
    // SPR0 = 1: Clock prescaler bit 0 (with SPR1=0, gives F_CPU/16 = 1MHz)
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

// Send one byte over SPI to slave
// Protocol: Pull SS low, send data, wait for completion, pull SS high
void SPI_MasterSend(uint8_t data) {
    PORTB &= ~(1<<PB2);  // SS LOW: activate slave
    SPDR = data;  // Write data to SPI Data Register (starts transmission)
    while (!(SPSR & (1<<SPIF)));  // Wait for transmission complete flag
    PORTB |= (1<<PB2);  // SS HIGH: deactivate slave
}

void setup() {
    SPI_MasterInit();
}

void loop() {
    // Sequence to send: 85 (0x55), 170 (0xAA), 255 (0xFF)
    // These values have distinctive bit patterns for easy verification
    static uint8_t seq[3] = {85, 170, 255};
    static uint8_t i = 0;

    SPI_MasterSend(seq[i]);  // Send current value in sequence
    i = (i + 1) % 3;  // Cycle through sequence: 0->1->2->0

    _delay_ms(1000);  // Send every second
}