//counter for timer1 interrupts
volatile uint16_t timer1Counter = 0;
//counter for timer2 interrupts
volatile uint16_t timer2Counter = 0;
//variable to store the last time we blinked the led
volatile unsigned long lastMillis = 0;

void setup(void) {
    //setting PB0, PB1, PB2 as outputs
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    //initializing all leds as off
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
    
    //configuring timer1 for ctc mode
    TCCR1A = 0x00; 
    TCCR1B = (1 << WGM12) | (1 << CS12);  
    OCR1A = 31249; 
    //enabling timer1 compare interrupt
    TIMSK1 = (1 << OCIE1A); 
    TCNT1 = 0;
    
    //configuring timer2 for normal mode
    TCCR2A = 0x00; 
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); 
    //enabling timer2 overflow interrupt
    TIMSK2 = (1 << TOIE2);  
    TCNT2 = 0;
    
    //enabling global interrupts
    sei(); 
}

ISR(TIMER1_COMPA_vect) {
    timer1Counter++;
     //toggling PB1 every 2 interrupts
     if (timer1Counter >= 2) { 
         PORTB ^= (1 << PB1); 
         timer1Counter = 0;
     }
}

ISR(TIMER2_OVF_vect) {
    timer2Counter++;
     //toggling PB2 every 122 interrupts
     if (timer2Counter >= 122) {  
         PORTB ^= (1 << PB2); 
         timer2Counter = 0;
     }
}

void loop(void) {
    unsigned long currentMillis = millis();
    //blinking PB0 every 500ms
    if (currentMillis - lastMillis >= 500) {
        lastMillis = currentMillis;
        PORTB ^= (1 << PB0);  
    }
}
