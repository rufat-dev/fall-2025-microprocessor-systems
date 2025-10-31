
void setup(void) {
    DDRD |= (1 << PD6) | (1 << PD5);
    DDRB |= (1 << PB1);
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    TCCR0B |= (1 << CS00);
    TCCR1A |= (1 << WGM10) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << CS00);
    
    OCR0A = 7;
    OCR0B = 204;
    OCR1A = 128;
    
}
void loop(void){}