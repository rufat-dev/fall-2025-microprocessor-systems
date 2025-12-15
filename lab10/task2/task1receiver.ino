#define LED1 PD2
#define LED2 PD3
#define LED3 PD4
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
    if (r == '1' || r == '2' || r == '3') {
//        EEPROM_write(index, r);
//        index++;
      
      PORTD &= ~((1<<LED1)|(1<<LED2)|(1<<LED3));

      if (r == '1') PORTD |= (1<<LED1);
      if (r == '2') PORTD |= (1<<LED2);
      if (r == '3') PORTD |= (1<<LED3);
    }
  
    
}