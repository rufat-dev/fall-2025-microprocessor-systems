// Lab 14 - Task 8: Block Cipher Decryption (Receiver)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives encrypted bytes over UART and decrypts them using
// the inverse Feistel network. Displays both ciphertext and plaintext on Serial Monitor.
// Cipher: 2-round Feistel network (same as transmitter, but rounds in reverse order).
// Key: 0x5A (must match transmitter), Block size: 8 bits, Mode: ECB

#include <stdint.h>

const uint8_t SECRET_KEY = 0x5A;  // 8-bit secret key (must match transmitter)

// 4-bit S-box (same as transmitter)
const uint8_t SBOX[16] = {
  6, 4,12, 5,
  0, 7, 2,14,
  1,15, 3,13,
  8,10, 9,11
};

// Feistel round decryption function (inverse of encryption)
// Decryption uses same structure but with reversed key order
void feistelRoundDecrypt(uint8_t *L, uint8_t *R, uint8_t roundKey) {
  uint8_t temp = *L;  // Save L for later
  uint8_t f = SBOX[(*L ^ roundKey) & 0x0F];  // F function: S-box lookup
  *L = *R ^ f;  // New L = R XOR F(L, key)
  *R = temp;    // New R = old L
}

// Decrypt an 8-bit ciphertext byte using 2-round Feistel network
// Decryption uses same rounds as encryption but in reverse key order
uint8_t decryptByte(uint8_t c) {
  // Split 8-bit ciphertext into two 4-bit halves
  uint8_t L = (c >> 4) & 0x0F;  // Left half (upper 4 bits)
  uint8_t R =  c       & 0x0F;  // Right half (lower 4 bits)

  // Two Feistel rounds with keys in reverse order (for decryption)
  feistelRoundDecrypt(&L, &R, (SECRET_KEY >> 4) & 0x0F); // Round 1: upper nibble
  feistelRoundDecrypt(&L, &R, SECRET_KEY & 0x0F);        // Round 2: lower nibble

  // Combine halves back into 8-bit plaintext
  return (L << 4) | R;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    uint8_t cipher = Serial.read();  // Read ciphertext byte from UART
    uint8_t plain  = decryptByte(cipher);  // Decrypt to get plaintext

    // Display both ciphertext and plaintext for verification
    Serial.print("Cipher: ");
    Serial.print(cipher);
    Serial.print("  Plain: ");
    Serial.println(plain);
  }
}