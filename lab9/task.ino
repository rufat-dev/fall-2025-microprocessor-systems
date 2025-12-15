//counter output
#define SEG_A (1 << PD6)
#define SEG_B (1 << PD7)
#define SEG_C (1 << PB4)
#define SEG_D (1 << PB3)
#define SEG_E (1 << PB2)
#define SEG_F (1 << PB0)
#define SEG_G (1 << PB1)
const uint8_t portbDigits[10] = {
    0b00011101, // 0 
    0b00010000, // 1 
    0b00001110, // 2 
    0b00011010, // 3 
    0b00010011, // 4 
    0b00011011, // 5 
    0b00011111, // 6 
    0b00010000, // 7 
    0b00011111, // 8 
    0b00011011  // 9 
};

// PD bits: PD6 (bit6) = A, PD7 (bit7) = B
const uint8_t portdDigits[10] = {
    (1<<PD6)|(1<<PD7), // 0 -> A+B 
    (1<<PD7),          // 1 -> B     
    (1<<PD6)|(1<<PD7), // 2
    (1<<PD6)|(1<<PD7), // 3
    (1<<PD7),          // 4
    (1<<PD6),          // 5
    (1<<PD6),          // 6
    (1<<PD6)|(1<<PD7), // 7
    (1<<PD6)|(1<<PD7), // 8
    (1<<PD6)|(1<<PD7)  // 9
};

const uint8_t segmentPatterns[10] = {
    0b00111111, //0 a b c d e f
    0b00000110, //1 b c
    0b01011011, //2 a b d e g
    0b01001111, //3 a b c d g
    0b01100110, //4 b c f g
    0b01101101, //5 a c d f g
    0b01111101, //6 a c d e f g
    0b00000111, //7 a b c
    0b01111111, //8 all
    0b01101111  //9 a b c d f g
};
volatile uint8_t enabled = 0;
volatile unsigned long lastTime = 0;


void setup(void) {

    Serial.begin(9600);
    //setting PB0, PB1, PB2 as outputs
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
    DDRD |= (1 << PD7) | (1 << PD6) | (1 << PD5);
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);   // enable pull-up on PD2
    //configuring timer1 for ctc mode
    TCCR1A = 0x00; 
    TCCR1B = (1 << WGM12) | (1 << CS12);  
    OCR1A = 31249; 
    //enabling timer1 compare interrupt
    TIMSK1 = (1 << OCIE1A); 
    TCNT1 = 0;
    
    //set falling edge on external interrupt on PD2 (PIN 4)
    EICRA |= (1 << ISC01);   
    EICRA &= ~(1 << ISC00);
    //enable interrupt
    EIMSK |= (1 << INT0);
    displayDigit(0);
    //enabling global interrupts
    sei(); 
}

ISR(TIMER1_COMPA_vect) {
    
  if(enabled){
    uint8_t segs = readSegments();
    int current = getDigitFromSegments(segs);

    if (current == -1) return; // invalid state
    int next = (current + 1) % 10; // wrap around after 9

     displayDigit(next);
  }
}

ISR(INT0_vect)
{
    unsigned long now = millis();
    Serial.println("time:");
    Serial.println(now);
    Serial.println(lastTime);
    if (now - lastTime > 250) {   // debounce window (ms)
        enabled = !enabled;
    }
    lastTime = now;
  Serial.println(enabled);
}

void displayDigit(uint8_t num) {
    PORTB &= ~0b00111111;
    PORTD &= ~0b11000000;

    PORTB |= portbDigits[num];
    PORTD |= portdDigits[num];
}
uint8_t readSegments(void) {
    uint8_t pattern = 0;

    // Read bits from both ports
    if (PIND & (1 << PD6)) pattern |= (1 << 0); // a
    if (PIND & (1 << PD7)) pattern |= (1 << 1); // b
    if (PINB & (1 << PB4)) pattern |= (1 << 2); // c
    if (PINB & (1 << PB3)) pattern |= (1 << 3); // d
    if (PINB & (1 << PB2)) pattern |= (1 << 4); // e
    if (PINB & (1 << PB0)) pattern |= (1 << 5); // f
    if (PINB & (1 << PB1)) pattern |= (1 << 6); // g

    return pattern;
}

int getDigitFromSegments(uint8_t segPattern) {
    for(uint8_t i=0;i<10;i++){
        if(segmentPatterns[i] == segPattern) return i;
    }
    return -1;
}

void loop(void) {
    
}
