//Task 2
// Write an AVR assembly program that calculates the sum of the first
// ten natural numbers entirely in registers.
// One method to do this: use one register as a counter starting from 1, a second register to hold
// the running sum (initially 0), and a third register as the limit set to 10. In a loop, add the counter
// to the sum with ADD, increment the counter with INC, then compare it against the limit using CP
// and repeat with BRNE until the counter reaches 10, leaving the final result (55 = 0x37) stored in
// the second.
// instructions: ADD, INC, CP (compare) and BRNE (branch if not equal).

uint8_t sum = 0; // Variable to store the final result

void setup() {
  Serial.begin(9600);
  
  asm volatile (
    "ldi r18, 0 \n\t"
    "ldi r19, 0 \n\t"
    "ldi r20, 10 \n\t"
    "loop: \n\t"
    "inc r18 \n\t"
    "add r19, r18 \n\t"
    "cp r18, r20 \n\t"
    "brne loop \n\t"
    "sts %0, r19 \n\t"
    :
    : "m" (sum)
    : "r18", "r19", "r20"
  );
  
  Serial.print("Sum ");
  Serial.println(sum);
}

void loop() {
  // Empty loop
}