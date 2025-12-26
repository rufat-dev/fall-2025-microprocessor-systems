#define LED1 PD2
#define LED2 PD3
#define LED3 PD4

uint16_t index = 0;

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


void EEPROM_write(uint16_t addr, uint8_t data) {
    while (EECR & (1 << EEPE));
    EEAR = addr;
    EEDR = data;
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}
uint8_t EEPROM_read(uint16_t addr) {
    while (EECR & (1 << EEPE));
    EEAR = addr;
    EECR |= (1 << EERE);
    return EEDR;
}

void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = d;
}

void setup() {
    UART_init();
    DDRD |= (1<<LED1) | (1<<LED2) | (1<<LED3);
}

void loop() {
    uint8_t r = UART_recv();
  	UART_send(r);   
  _delay_ms(2);
    if (r == '1' || r == '2' || r == '3') {
       EEPROM_write(index, r);
      _delay_ms(2);
      uint8_t v = EEPROM_read(index);
      UART_send(v); 
       index++;
      _delay_ms(2);
      
    }else if(r == '4'){

        for(uint16_t i = 0; i<index; i++){
            UART_send('0' + i); 
             _delay_ms(2);
            uint8_t v = EEPROM_read(i);
            UART_send(v); 
            _delay_ms(2);

            PORTD &= ~((1<<LED1)|(1<<LED2)|(1<<LED3));

            if (v == '1') PORTD |= (1<<LED1);
            if (v == '2') PORTD |= (1<<LED2);
            if (v == '3') PORTD |= (1<<LED3);

            _delay_ms(200);
        }
        index = 0;
    }
    
}