//variable to count the button presses
uint8_t pressCount = 0;
//variable to store the previous state of the button
uint8_t lastButtonState = 0;

void setup(void) {
    //setting PD6 and PD5 as outputs for PWM
    DDRD |= (1 << PD6) | (1 << PD5);
    //setting PB1 as output for PWM
    DDRB |= (1 << PB1);

    //setting PD2 as input for the button
    DDRD &= ~(1 << PD2);
    //enabling pullup resistor for the button
    PORTD |= (1 << PD2);

    //configuring timer0 for fast pwm mode
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    //enabling non inverting pwm on OC0A and OC0B
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    //starting the timer with no prescaler
    TCCR0B |= (1 << CS00);

    //configuring timer1 for fast pwm mode
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);
    //enabling non inverting pwm on OC1A
    TCCR1A |= (1 << COM1A1);
    //starting the timer with no prescaler
    TCCR1B |= (1 << CS10);
}

void loop(void) {
    //reading the current state of the button pin
    uint8_t currentButtonState = PIND & (1 << PD2);

    //checking if button is pressed (transition from low to high)
    if (currentButtonState && !lastButtonState) {
        //incrementing the press count
        pressCount++;
        //resetting count after 12 presses
        if (pressCount > 12) pressCount = 1;

        //calculating which brightness step we are at (0-3)
        uint8_t step = (pressCount - 1) % 4;
        //calculating which led we should control (0-2)
        uint8_t ledGroup = (pressCount - 1) / 4;
        //calculating brightness value (64, 128, 192, 255)
        uint8_t brightness = (step + 1) * 64;

        //reseting all pwm outputs to zero
        OCR0A = 0;
        OCR0B = 0;
        OCR1A = 0;

        //applying brightness to the current led based on the group
        if (ledGroup == 0) OCR0A = brightness;
        else if (ledGroup == 1) OCR0B = brightness;
        else if (ledGroup == 2) OCR1A = brightness;

        //delay for debouncing the button
        delay(50);
    }

    //updating the last state for next iteration
    lastButtonState = currentButtonState;
}
