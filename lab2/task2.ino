//Task 2
// Add two numbers (X = 5, Y = 8) using registers, store the
// result in R18

uint8_t sum = 0; // Variable to store the result

void setup() {
    Serial.begin(9600); 
    // to read and write from the serial monitor we open a port with the serial.begin method
  
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
