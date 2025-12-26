#define SEG_A (1 << PD6)
#define SEG_B (1 << PD7)
#define SEG_C (1 << PB4)
#define SEG_D (1 << PB3)
#define SEG_E (1 << PB2)
#define SEG_F (1 << PB0)
#define SEG_G (1 << PB1)
const uint8_t portbDigits[10] = {
    0b00011101,
    0b00010000,
    0b00001110,
    0b00011010,
    0b00010011,
    0b00011011,
    0b00011111,
    0b00010000,
    0b00011111,
    0b00011011
};

const uint8_t portdDigits[10] = {
    (1<<PD6)|(1<<PD7),
    (1<<PD7),
    (1<<PD6)|(1<<PD7),
    (1<<PD6)|(1<<PD7),
    (1<<PD7),
    (1<<PD6),
    (1<<PD6),
    (1<<PD6)|(1<<PD7),
    (1<<PD6)|(1<<PD7),
    (1<<PD6)|(1<<PD7)
};

const uint8_t segmentPatterns[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};


volatile uint8_t enabled = 0;
volatile unsigned long lastTime = 0;


void setup(void) {

    Serial.begin(9600);
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
    DDRD |= (1 << PD7) | (1 << PD6) | (1 << PD5);
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    TCCR1A = 0x00; 
    TCCR1B = (1 << WGM12) | (1 << CS12);  
    OCR1A = 31249; 
    TIMSK1 = (1 << OCIE1A); 
    TCNT1 = 0;
    
    EICRA |= (1 << ISC01);   
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);
    displayDigit(0);
    sei(); 
}

ISR(TIMER1_COMPA_vect) {
    if(enabled){
        uint8_t segs = readSegments();
        int current = getDigitFromSegments(segs);

        if (current == -1) return;
        int next = (current + 1) % 10;

        displayDigit(next);
    }
}

ISR(INT0_vect)
{
    unsigned long now = millis();
    if (now - lastTime > 250) {
        enabled = !enabled;
    }
    lastTime = now;
}

void displayDigit(uint8_t num) {
    PORTB &= ~0b00111111;
    PORTD &= ~0b11000000;

    PORTB |= portbDigits[num];
    PORTD |= portdDigits[num];
}
uint8_t readSegments(void) {
    uint8_t pattern = 0;

    if (PIND & (1 << PD6)) pattern |= (1 << 0);
    if (PIND & (1 << PD7)) pattern |= (1 << 1);
    if (PINB & (1 << PB4)) pattern |= (1 << 2);
    if (PINB & (1 << PB3)) pattern |= (1 << 3);
    if (PINB & (1 << PB2)) pattern |= (1 << 4);
    if (PINB & (1 << PB0)) pattern |= (1 << 5);
    if (PINB & (1 << PB1)) pattern |= (1 << 6);

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
