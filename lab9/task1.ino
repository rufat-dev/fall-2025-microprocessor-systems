// Lab 9 - Task 1: 7-Segment Display Counter with Timer and External Interrupt
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Uses Timer1 in CTC mode to drive a 7-segment display counting 0-9
// every 0.5 seconds. External interrupt on PD2 (INT0) pauses/resumes counting.
// Timer calculation: 16MHz / 256 (prescaler) = 62500 Hz
// For 0.5s: 62500 * 0.5 = 31250 counts, OCR1A = 31249 (0-indexed)

// 7-segment display pin definitions
// Segments A and B are on PORTD, segments C-G are on PORTB
#define SEG_A (1 << PD6)
#define SEG_B (1 << PD7)
#define SEG_C (1 << PB4)
#define SEG_D (1 << PB3)
#define SEG_E (1 << PB2)
#define SEG_F (1 << PB0)
#define SEG_G (1 << PB1)

// PORTB segment patterns for digits 0-9 (bits: F, G, E, D, C, B, A)
// Only bits PB0-PB4 are used for segments F, G, E, D, C
const uint8_t portbDigits[10] = {
    0b00011101,  // 0: segments F, E, D, C, B, A
    0b00010000,  // 1: segments B, C
    0b00001110,  // 2: segments A, B, D, E, G
    0b00011010,  // 3: segments A, B, C, D, G
    0b00010011,  // 4: segments B, C, F, G
    0b00011011,  // 5: segments A, C, D, F, G
    0b00011111,  // 6: segments A, C, D, E, F, G
    0b00010000,  // 7: segments A, B, C
    0b00011111,  // 8: all segments
    0b00011011   // 9: segments A, B, C, D, F, G
};

// PORTD segment patterns for digits 0-9 (bits: A, B)
// PD6 = segment A, PD7 = segment B
const uint8_t portdDigits[10] = {
    (1<<PD6)|(1<<PD7),  // 0: A, B
    (1<<PD7),            // 1: B
    (1<<PD6)|(1<<PD7),   // 2: A, B
    (1<<PD6)|(1<<PD7),   // 3: A, B
    (1<<PD7),            // 4: B
    (1<<PD6),            // 5: A
    (1<<PD6),            // 6: A
    (1<<PD6)|(1<<PD7),   // 7: A, B
    (1<<PD6)|(1<<PD7),   // 8: A, B
    (1<<PD6)|(1<<PD7)    // 9: A, B
};

// Complete segment patterns for reading current display state
// Used to determine current digit when resuming from pause
const uint8_t segmentPatterns[10] = {
    0b00111111,  // 0: a b c d e f
    0b00000110,  // 1: b c
    0b01011011,  // 2: a b d e g
    0b01001111,  // 3: a b c d g
    0b01100110,  // 4: b c f g
    0b01101101,  // 5: a c d f g
    0b01111101,  // 6: a c d e f g
    0b00000111,  // 7: a b c
    0b01111111,  // 8: all segments
    0b01101111   // 9: a b c d f g
};

// Global state variables
volatile uint8_t enabled = 0;        // Counter pause/resume flag (0=paused, 1=running)
volatile unsigned long lastTime = 0;  // Last button press time for debouncing


void setup(void) {
    Serial.begin(9600);
    
    // Configure PORTB as output for segments C, D, E, F, G (PB0-PB4)
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
    // Configure PORTD: PD6, PD7 as output for segments A, B; PD2 as input for button
    DDRD |= (1 << PD7) | (1 << PD6) | (1 << PD5);
    DDRD &= ~(1 << PD2);  // PD2 as input
    PORTD |= (1 << PD2);  // Enable pull-up resistor on PD2
    
    // Configure Timer1 for CTC (Clear Timer on Compare) mode
    // WGM12 = 1: CTC mode, CS12 = 1: prescaler = 256
    // Timer frequency = 16MHz / 256 = 62500 Hz
    TCCR1A = 0x00;  // Normal port operation, CTC mode
    TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler 256
    
    // Set compare value for 0.5 second intervals
    // OCR1A = (16MHz / 256) * 0.5s - 1 = 31249
    OCR1A = 31249; 
    
    // Enable Timer1 compare match interrupt
    TIMSK1 = (1 << OCIE1A); 
    TCNT1 = 0;  // Reset timer counter
    
    // Configure external interrupt INT0 on PD2 for falling edge trigger
    // ISC01 = 1, ISC00 = 0: falling edge triggers interrupt
    EICRA |= (1 << ISC01);   
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);  // Enable INT0 interrupt
    
    displayDigit(0);  // Initialize display to 0
    sei();  // Enable global interrupts
}

// Timer1 Compare Match A interrupt service routine
// Called every 0.5 seconds when timer reaches OCR1A value
ISR(TIMER1_COMPA_vect) {
    if(enabled){
        // Read current display state to resume from where we left off
        uint8_t segs = readSegments();
        int current = getDigitFromSegments(segs);

        if (current == -1) return;  // Invalid state, skip update
        int next = (current + 1) % 10;  // Increment and wrap around after 9

        displayDigit(next);
    }
}

// External interrupt INT0 service routine (button press on PD2)
// Toggles counter pause/resume state with debouncing
ISR(INT0_vect)
{
    unsigned long now = millis();
    // Debounce: ignore presses within 250ms of last press
    if (now - lastTime > 250) {
        enabled = !enabled;  // Toggle pause/resume
    }
    lastTime = now;
}

// Display a digit (0-9) on the 7-segment display
// Clears all segments first, then sets appropriate segments for the digit
void displayDigit(uint8_t num) {
    PORTB &= ~0b00111111;  // Clear segments C, D, E, F, G (PB0-PB5)
    PORTD &= ~0b11000000;  // Clear segments A, B (PD6, PD7)

    PORTB |= portbDigits[num];  // Set PORTB segments
    PORTD |= portdDigits[num];  // Set PORTD segments
}

// Read current state of all 7 segments from hardware pins
// Returns a bit pattern representing which segments are currently lit
uint8_t readSegments(void) {
    uint8_t pattern = 0;

    // Read each segment pin and build pattern
    // Bit positions: 0=a, 1=b, 2=c, 3=d, 4=e, 5=f, 6=g
    if (PIND & (1 << PD6)) pattern |= (1 << 0);  // segment A
    if (PIND & (1 << PD7)) pattern |= (1 << 1);  // segment B
    if (PINB & (1 << PB4)) pattern |= (1 << 2);  // segment C
    if (PINB & (1 << PB3)) pattern |= (1 << 3);  // segment D
    if (PINB & (1 << PB2)) pattern |= (1 << 4);  // segment E
    if (PINB & (1 << PB0)) pattern |= (1 << 5);  // segment F
    if (PINB & (1 << PB1)) pattern |= (1 << 6);  // segment G

    return pattern;
}

// Convert segment pattern to digit value (0-9)
// Returns -1 if pattern doesn't match any known digit
int getDigitFromSegments(uint8_t segPattern) {
    for(uint8_t i=0;i<10;i++){
        if(segmentPatterns[i] == segPattern) return i;
    }
    return -1;  // Invalid pattern
}

void loop(void) {
    
}
