
#include <Arduino.h>
#include <stdint.h>

const uint8_t SECRET_KEY = 0x5A;

const uint8_t SBOX[16] = {
  6, 4,12, 5,
  0, 7, 2,14,
  1,15, 3,13,
  8,10, 9,11
};

void feistelRoundEncrypt(uint8_t *L, uint8_t *R, uint8_t roundKey) {
  uint8_t temp = *R;
  uint8_t f = SBOX[(*R ^ roundKey) & 0x0F];
  *R = *L ^ f;
  *L = temp;
}
uint8_t encryptByte(uint8_t p) {
  uint8_t L = (p >> 4) & 0x0F;
  uint8_t R =  p       & 0x0F;

  feistelRoundEncrypt(&L, &R, SECRET_KEY & 0x0F);
  feistelRoundEncrypt(&L, &R, (SECRET_KEY >> 4) & 0x0F);

  return (L << 4) | R;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  static uint8_t counter = 0;

  uint8_t cipher = encryptByte(counter);
  Serial.write(cipher);

  counter++;
  delay(100);
}