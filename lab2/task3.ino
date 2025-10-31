
uint8_t sub = 0; 
uint8_t sreg_flags = 0; 

void setup() {
    Serial.begin(9600); 
   asm volatile (
          "ldi r18, 200 \n\t"     
          "ldi r24, 50 \n\t"        
          "sub r18, r24 \n\t"
          "sts %0, r18 \n\t"
          :
          : "m" (sub)
          : "r18", "r24"      
        );
    Serial.print("subtraction ");
    Serial.println(sub);
    Serial.print("SREG flags: ");
    Serial.println(SREG, BIN);
    if (SREG & (1<<1)) {
        Serial.println("Zero flag (Z) is 1");
    } else {
        Serial.println("Zero flag (Z) is 0");
    }
    if (SREG & (1<<0)) {
        Serial.println("Carry flag (C) is 1");
    } else {
        Serial.println("Carry flag (C) is 0");
    }
  }
  
  void loop() {
   
  }