// Lab 14 - Task 8: Block Cipher Encryption (Transmitter)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Implements a simple 8-bit Feistel network block cipher in ECB mode.
// Counts from 0-255, encrypts each value, and sends ciphertext over UART every 0.1s.
// Cipher: 2-round Feistel network with 4-bit S-box substitution.
// Key: 0x5A (8 bits), Block size: 8 bits, Mode: ECB (Electronic Codebook)

#include <stdint.h>

const uint8_t SECRET_KEY = 0x5A;  // 8-bit secret key

// 4-bit S-box (Substitution box) for Feistel round function
// Maps 4-bit input to 4-bit output (provides non-linearity)
const uint8_t SBOX[16] = {
  6, 4,12, 5,
  0, 7, 2,14,
  1,15, 3,13,
  8,10, 9,11
};

// Feistel round encryption function
// Feistel structure: L' = R, R' = L XOR F(R, roundKey)
// F function: S-box lookup of (R XOR roundKey)
void feistelRoundEncrypt(uint8_t *L, uint8_t *R, uint8_t roundKey) {
  uint8_t temp = *R;  // Save R for later
  uint8_t f = SBOX[(*R ^ roundKey) & 0x0F];  // F function: S-box lookup
  *R = *L ^ f;  // New R = L XOR F(R, key)
  *L = temp;    // New L = old R
}

// Encrypt an 8-bit plaintext byte using 2-round Feistel network
// Block size: 8 bits (split into two 4-bit halves)
// Key schedule: uses lower and upper nibbles of SECRET_KEY
uint8_t encryptByte(uint8_t p) {
  // Split 8-bit plaintext into two 4-bit halves
  uint8_t L = (p >> 4) & 0x0F;  // Left half (upper 4 bits)
  uint8_t R =  p       & 0x0F;  // Right half (lower 4 bits)

  // Two Feistel rounds with different round keys
  feistelRoundEncrypt(&L, &R, SECRET_KEY & 0x0F);        // Round 1: lower nibble
  feistelRoundEncrypt(&L, &R, (SECRET_KEY >> 4) & 0x0F); // Round 2: upper nibble

  // Combine halves back into 8-bit ciphertext
  return (L << 4) | R;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static uint8_t counter = 0;  // Counter from 0 to 255

  uint8_t cipher = encryptByte(counter);  // Encrypt counter value
  Serial.write(cipher);  // Send ciphertext over UART (one byte per frame)

  counter++;  // Increment counter (wraps to 0 after 255)
  delay(100);  // Send every 0.1 seconds
}