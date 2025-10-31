uint8_t pressCount = 0;
uint8_t lastButtonState = 0;

void setup(void) {
    DDRD |= (1 << PD6) | (1 << PD5);
    DDRB |= (1 << PB1);

    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    TCCR0B |= (1 << CS00);

    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);
    TCCR1A |= (1 << COM1A1);
    TCCR1B |= (1 << CS10);
}

void loop(void) {
    

    uint8_t currentButtonState = PIND & (1 << PD2);

    if (currentButtonState && !lastButtonState) {
        pressCount++;
        if (pressCount > 12) pressCount = 1;

        uint8_t step = (pressCount - 1) % 4;
        uint8_t ledGroup = (pressCount - 1) / 4;
        uint8_t brightness = (step + 1) * 64;

        OCR0A = 0;
        OCR0B = 0;
        OCR1A = 0;

        if (ledGroup == 0) OCR0A = brightness;
        else if (ledGroup == 1) OCR0B = brightness;
        else if (ledGroup == 2) OCR1A = brightness;

        delay(50);
    }

    lastButtonState = currentButtonState;
}
