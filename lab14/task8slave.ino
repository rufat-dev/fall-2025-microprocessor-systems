
#include <Arduino.h>
#include <stdint.h>

const uint8_t SECRET_KEY = 0x5A;

const uint8_t SBOX[16] = {
  6, 4,12, 5,
  0, 7, 2,14,
  1,15, 3,13,
  8,10, 9,11
};
void feistelRoundDecrypt(uint8_t *L, uint8_t *R, uint8_t roundKey) {
  uint8_t temp = *L;
  uint8_t f = SBOX[(*L ^ roundKey) & 0x0F];
  *L = *R ^ f;
  *R = temp;
}

uint8_t decryptByte(uint8_t c) {
  uint8_t L = (c >> 4) & 0x0F;
  uint8_t R =  c       & 0x0F;

  feistelRoundDecrypt(&L, &R, (SECRET_KEY >> 4) & 0x0F);
  feistelRoundDecrypt(&L, &R, SECRET_KEY & 0x0F);

  return (L << 4) | R;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    uint8_t cipher = Serial.read();
    uint8_t plain  = decryptByte(cipher);

    Serial.print("Cipher: ");
    Serial.print(cipher);
    Serial.print("  Plain: ");
    Serial.println(plain);
  }
}