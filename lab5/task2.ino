void setup(void) {
    //setting PD6 and PD5 as outputs for PWM
    DDRD |= (1 << PD6) | (1 << PD5);
    //setting PB1 as output for PWM
    DDRB |= (1 << PB1);
    //configuring timer0 for fast pwm mode
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    //enabling non inverting pwm on OC0A and OC0B
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    //starting timer0 with no prescaler
    TCCR0B |= (1 << CS00);
    //configuring timer1 for fast pwm mode and enabling non inverting pwm on OC1A
    TCCR1A |= (1 << WGM10) | (1 << COM1A1);
    //configuring timer1 waveform and starting with no prescaler
    TCCR1B |= (1 << WGM12) | (1 << CS00);
    
    //setting OCR0A to 30% duty cycle for LED1
    OCR0A = 7;
    //setting OCR0B to 80% duty cycle for LED2
    OCR0B = 204;
    //setting OCR1A to 50% duty cycle for LED3
    OCR1A = 128;
    
}
void loop(void){}