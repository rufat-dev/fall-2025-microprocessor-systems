 
 
 volatile uint16_t timer1Counter = 0;
 volatile uint16_t timer2Counter= 0;
 volatile unsigned long lastMillis = 0;
 
 void setup(void) {
     DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
     PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
     
     TCCR1A = 0x00; 
     TCCR1B = (1 << WGM12) | (1 << CS12);  
     OCR1A = 31249; 
     TIMSK1 = (1 << OCIE1A); 
     TCNT1 = 0;
     
     TCCR2A = 0x00; 
     TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); 
     TIMSK2 = (1 << TOIE2);  
     TCNT2 = 0;
     
     sei(); 
 }
 
 ISR(TIMER1_COMPA_vect) {
    timer1Counter++;
     if (timer1Counter >= 2) { 
         PORTB ^= (1 << PB1); 
         timer1Counter = 0;
     }
 }
 
 ISR(TIMER2_OVF_vect) {
    timer2Counter++;
     if (timer2Counter >= 122) {  
         PORTB ^= (1 << PB2); 
         timer2Counter = 0;
     }
 }
 
 void loop(void) {
     unsigned long currentMillis = millis();
     if (currentMillis - lastMillis >= 500) {
         lastMillis = currentMillis;
         PORTB ^= (1 << PB0);  
     }
 }
 