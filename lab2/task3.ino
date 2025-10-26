//Task 3
// Subtract two numbers (X = 50, Y = 200) using registers,
// store the result in R18, and print the register values. Check Zero
// (Z) and Carry (C) flags in SREG.

uint8_t sub = 0; // Variable to store the result
uint8_t sreg_flags = 0; // Variable to store SREG flags

void setup() {
    Serial.begin(9600); 
    // to read and write from the serial monitor we open a port with the serial.begin method
  
   asm volatile (
          "ldi r18, 200 \n\t"        // Load 200 into register 18 (Y)
          "ldi r24, 50 \n\t"         // Load 50 into register 24 (X)
          "sub r18, r24 \n\t"        // Subtract r24 from r18 
          "sts %0, r18 \n\t"         // Store result in sub variable
          "in r25, 0x3f \n\t"        // Read SREG (Status Register) into r25
          "sts %1, r25 \n\t"         // Store SREG flags
          :
          : "m" (sub), "m" (sreg_flags) 
          : "r18", "r24", "r25"        
        );
    
    Serial.print("subtraction ");
    Serial.println(sub);
    
    // Check and print flags
    Serial.print("SREG flags: ");
    Serial.println(sreg_flags, BIN);
    
    // Check specific flags
    if (sreg_flags & 0x02) {  // Z flag (bit 1)
        Serial.println("Zero flag (Z) is 1");
    } else {
        Serial.println("Zero flag (Z) is 0");
    }
    
    if (sreg_flags & 0x01) {  // C flag (bit 0)
        Serial.println("Carry flag (C) is 1");
    } else {
        Serial.println("Carry flag (C) is 0");
    }
  }
  
  void loop() {
   
  }