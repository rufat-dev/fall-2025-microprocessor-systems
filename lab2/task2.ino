
uint8_t sum = 0; 

void setup() {
    Serial.begin(9600); 
  
   asm volatile (
          "ldi r18, 5 \n\t"        
          "ldi r24, 8 \n\t"       
          "add r18, r24 \n\t"     
          "sts %0, r18 \n\t"       
          :
          : "m" (sum)          
          : "r18", "r24"          
        );
        
    Serial.print("sum ");
    Serial.println(sum);
  }
  
  void loop() {
   
  }
