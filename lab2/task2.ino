
//we initialize the sum variable so that we can pass it to serial.print
uint8_t sum = 0; 

void setup() {
  //initializing serial monitor at port 9600
    Serial.begin(9600); 
  
    //asm means it involves assembly code and volatile means the compilar should not modify its execution
   asm volatile (
    // we set the register r18 with the constant value 5 
          "ldi r18, 5 \n\t"        
    // we set the register r24 with the constant value 8
          "ldi r24, 8 \n\t"       
    // we use the add instruction to add the values of these registers which stores the value back in r18
          "add r18, r24 \n\t"  
    // we pass the address where we want to store the value of the register r18 using %0 which will retrieve the adress
    //of sum in the sram   
          "sts %0, r18 \n\t"       
          :
          : "m" (sum)          
    // clobbered registers
          : "r18", "r24"          
        );

    Serial.print("sum ");
    // we output the sum in the serial monitor
    Serial.println(sum);
  }
  
  void loop() {
   
  }
