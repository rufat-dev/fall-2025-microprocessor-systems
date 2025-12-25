

#include <avr/io.h>
#include <util/delay.h>

void SPI_MasterInit(void) {
    DDRB |= (1<<PB3) | (1<<PB5) | (1<<PB2);
    PORTB |= (1<<PB2);

    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterSend(uint8_t data) {
    PORTB &= ~(1<<PB2);
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    PORTB |= (1<<PB2);
}

void setup() {
    SPI_MasterInit();
}

void loop() {
    static uint8_t seq[3] = {85, 170, 255};
    static uint8_t i = 0;

    SPI_MasterSend(seq[i]);
    i = (i + 1) % 3;

    _delay_ms(1000);
}