
uint8_t sum = 0;

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
}