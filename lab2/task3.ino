
// initializing variable where i will store the subtraction value
uint8_t sub = 0; 
// initialazing the value where I will store the sreg byte 
uint8_t sreg_flags = 0; 

void setup() {
    Serial.begin(9600); 
   asm volatile (
            // we set the register r18 with the constant value 200
          "ldi r18, 200 \n\t"     
            // we set the register r24 with the constant value 50
          "ldi r24, 50 \n\t"        
            // we use the subtraction instruction to subtract their values and set the value back in r18
          "sub r18, r24 \n\t"
            // we store the value back in the address of sub in sram
          "sts %0, r18 \n\t"
          :
          : "m" (sub)
          : "r18", "r24" //clobbered registers 
        );
    Serial.print("subtraction ");
    Serial.println(sub);
    Serial.print("SREG flags: ");
    Serial.println(SREG, BIN);
    // we check the Z flag which is the second bit in the SREG register
    if (SREG & (1<<1)) {
        Serial.println("Zero flag (Z) is 1");
    } else {
        Serial.println("Zero flag (Z) is 0");
    }
    // we check the C flag which is the first bit in the SREG register
    if (SREG & (1<<0)) {
        Serial.println("Carry flag (C) is 1");
    } else {
        Serial.println("Carry flag (C) is 0");
    }
  }
  
  void loop() {
   
  }