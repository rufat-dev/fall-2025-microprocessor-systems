// Lab 13 - Task 7: Prime Number Sieve (Find 2000 Primes)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Finds the first 2000 prime numbers using Sieve of Eratosthenes
// for numbers up to MAX_LIMIT, then trial division for remaining numbers.
// Toggles LED (PB5) for each prime found above MAX_LIMIT.
// Measures and reports total execution time.

#define MAX_LIMIT 13600   // Maximum number for sieve (fits in available RAM)
#define LAST_PRIME 17389  // Last prime needed to reach 2000 primes
// Bit array for sieve: each bit represents one number (1 = prime, 0 = composite)
uint8_t sieve[(MAX_LIMIT / 8) + 1];

// Set bit at index (mark as prime)
void setBit(int index) {
  sieve[index >> 3] |= (1 << (index & 7));
}

// Clear bit at index (mark as composite)
void clearBit(int index) {
  sieve[index >> 3] &= ~(1 << (index & 7));
}

// Get bit at index (check if prime)
bool getBit(int index) {
  return sieve[index >> 3] & (1 << (index & 7));
}

void setup() {
  Serial.begin(9600);
  DDRB |= (1 << PB5);  // Configure LED pin (PB5) as output
  PORTB &= ~(1 << PB5); 
  
  unsigned long start = micros();  // Start timing

  // Initialize sieve: mark all numbers as prime (all bits = 1)
  for (int i = 0; i <= (MAX_LIMIT >> 3); i++) sieve[i] = 0xFF;
  clearBit(0);  // 0 is not prime
  clearBit(1);  // 1 is not prime

  // Sieve of Eratosthenes: mark multiples of each prime as composite
  // Only need to check up to sqrt(MAX_LIMIT)
  for (int i = 2; i * i <= MAX_LIMIT; i++) {
    if (getBit(i)) {  // If i is prime
      // Mark all multiples of i as composite (start from i*i)
      for (int j = i * i; j <= MAX_LIMIT; j += i) clearBit(j);
    }
  }

  // Count primes found in sieve (up to MAX_LIMIT)
  int count = 0;
  for (int i = 2; i <= MAX_LIMIT && count < 2000; i++) {
    if (getBit(i)) count++;
  }

  // For numbers above MAX_LIMIT, use trial division
  // Only check odd numbers (skip even numbers)
  for (int i = MAX_LIMIT + 1; i <= LAST_PRIME && count < 2000; i += 2) {
      bool isPrime = true;
      // Check divisibility by odd numbers up to sqrt(i)
      for (int j = 3; j*j <= i; j += 2) {
        if (i % j == 0) {
          isPrime = false;
          break;  // Found a divisor, not prime
        }
      }
      if (isPrime) {
        count++;  // Found a prime
        // Toggle LED for each prime found (visual indicator)
        PORTB |= (1 << PB5); 
        PORTB &= ~(1 << PB5); 
      }
    }

  unsigned long end = micros();  // End timing
  unsigned long duration = end - start;

  // Report results
  Serial.print("Time (us): ");
  Serial.println(duration);
  Serial.print("Total primes found: ");
  Serial.println(count);
}

void loop() {}