#define MAX_LIMIT 13600  // Sieve limit 
#define LAST_PRIME 17389
uint8_t sieve[(MAX_LIMIT / 8) + 1];


// Bit operations for sieve
void setBit(int index) {
  sieve[index >> 3] |= (1 << (index & 7));
}

void clearBit(int index) {
  sieve[index >> 3] &= ~(1 << (index & 7));
}

bool getBit(int index) {
  return sieve[index >> 3] & (1 << (index & 7));
}

void setup() {
  Serial.begin(9600);

  unsigned long start = micros();

  // Initialize sieve
  for (int i = 0; i <= (MAX_LIMIT >> 3); i++) sieve[i] = 0xFF;
  clearBit(0);
  clearBit(1);

  // Sieve of Eratosthenes
  for (int i = 2; i * i <= MAX_LIMIT; i++) {
    if (getBit(i)) {
      for (int j = i * i; j <= MAX_LIMIT; j += i) clearBit(j);
    }
  }

  // Count primes in sieve first
  int count = 0;
  for (int i = 2; i <= MAX_LIMIT && count < 2000; i++) {
    if (getBit(i)) count++;
  }

  // Trial division for numbers above MAX_LIMIT
  for (int i = MAX_LIMIT + 1; i <= LAST_PRIME && count < 2000; i += 2) {
      bool isPrime = true;
      for (int j = 3; j*j <= i; j += 2) {
        if (i % j == 0) {
          isPrime = false;
          break;  // Stop checking after first divisor found
        }
      }
      if (isPrime) count++;
    }

  unsigned long end = micros();
  unsigned long duration = end - start;

  Serial.print("Time (us): ");
  Serial.println(duration);
  Serial.print("Total primes found: ");
  Serial.println(count);
}

void loop() {}