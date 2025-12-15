#define LED1 PD2
#define LED2 PD3
#define LED3 PD4

uint16_t index = 0;

void UART_init(void) {
    // Baud rate = 9600 @ 16 MHz
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0A = 0x00;   // normal speed
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // Enable RX & TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit, 1 stop, no parity
}

uint8_t UART_recv(void) {
    while (!(UCSR0A & (1 << RXC0)));   // wait until data received
    return UDR0;
}

void EEPROM_write(uint16_t addr, uint8_t data) {
    eeprom_write_byte((uint8_t*)addr, data);
}

uint8_t EEPROM_read(uint16_t addr) {
    return eeprom_read_byte((uint8_t*)addr);
}
void UART_send(uint8_t d) {
    while (!(UCSR0A & (1 << UDRE0)));  // wait until TX buffer empty
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
       index++;
        UART_send('0' + index); 
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