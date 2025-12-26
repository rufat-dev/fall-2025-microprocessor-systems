#include <stdint.h>

#define SPECK_ROUNDS   22
#define KEY_WORDS      4
#define BLOCK_BYTES    4
#define UART_BAUD      9600

static const uint8_t TAG[4] = { 'K', 'E', 'Y', '!' };

static const uint16_t KEY[KEY_WORDS] = {
  0xA1B2, 0xC3D4, 0xE5F6, 0x1357
};

static uint16_t roundKey[SPECK_ROUNDS];

static inline uint16_t ror16(uint16_t v, uint8_t r) {
  r &= 15;
  return (uint16_t)((v >> r) | (uint16_t)(v << (16 - r)));
}
static inline uint16_t rol16(uint16_t v, uint8_t r) {
  r &= 15;
  return (uint16_t)((uint16_t)(v << r) | (v >> (16 - r)));
}

static void makeRoundKeys(const uint16_t k[KEY_WORDS], uint16_t rk[SPECK_ROUNDS]) {
  uint16_t L[3] = { k[0], k[1], k[2] };
  rk[0] = k[3];

  uint8_t idx = 0;
  for (uint8_t i = 0; i < SPECK_ROUNDS - 1; ++i) {
    uint16_t tmp = (uint16_t)((ror16(L[idx], 7) + rk[i]) ^ i);
    rk[i + 1] = (uint16_t)(rol16(rk[i], 2) ^ tmp);
    L[idx] = tmp;
    idx = (uint8_t)((idx + 1) % 3);
  }
}

static void speckEncryptBlock(uint16_t &a, uint16_t &b, const uint16_t rk[SPECK_ROUNDS]) {
  for (uint8_t i = 0; i < SPECK_ROUNDS; ++i) {
    a = (uint16_t)((ror16(a, 7) + b) ^ rk[i]);
    b = (uint16_t)(rol16(b, 2) ^ a);
  }
}

struct CtrStream {
  uint32_t counter;
  uint8_t  buf[BLOCK_BYTES];
  uint8_t  pos;

  void reset(uint32_t nonce) {
    counter = nonce;
    pos = BLOCK_BYTES;
  }

  void refill() {
    uint16_t a = (uint16_t)(counter >> 16);
    uint16_t b = (uint16_t)(counter & 0xFFFF);

    speckEncryptBlock(a, b, roundKey);

    uint32_t w = ((uint32_t)a << 16) | (uint32_t)b;

    buf[0] = (uint8_t)(w & 0xFF);
    buf[1] = (uint8_t)((w >> 8) & 0xFF);
    buf[2] = (uint8_t)((w >> 16) & 0xFF);
    buf[3] = (uint8_t)((w >> 24) & 0xFF);

    pos = 0;
    counter++;
  }

  uint8_t xorByte(uint8_t in) {
    if (pos >= BLOCK_BYTES) refill();
    return (uint8_t)(in ^ buf[pos++]);
  }
};

static CtrStream stream;

static void sendU32_LE(uint32_t v) {
  Serial.write((uint8_t)(v & 0xFF));
  Serial.write((uint8_t)((v >> 8) & 0xFF));
  Serial.write((uint8_t)((v >> 16) & 0xFF));
  Serial.write((uint8_t)((v >> 24) & 0xFF));
}

static void sendTagAndNonce(uint32_t nonce) {
  Serial.write(TAG, 4);
  sendU32_LE(nonce);
}

static uint32_t makeNonce() {
  uint32_t a = (uint32_t)micros();
  uint32_t b = (uint32_t)analogRead(A0);
  uint32_t c = (uint32_t)analogRead(A1);
  return (a << 1) ^ (b << 17) ^ (c << 3) ^ 0x9E3779B9UL;
}

void setup() {
  Serial.begin(UART_BAUD);
  makeRoundKeys(KEY, roundKey);

  uint32_t nonce = makeNonce();
  stream.reset(nonce);

  delay(500);
  sendTagAndNonce(nonce);
}

void loop() {
  static uint8_t plain = 0;

  uint8_t cipher = stream.xorByte(plain);
  Serial.write(cipher);
  plain++;

  delay(100);
}