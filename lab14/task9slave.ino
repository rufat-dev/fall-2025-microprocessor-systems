// Lab 14 - Task 9: Speck32/64 Cipher in CTR Mode (Receiver)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Receives encrypted data over UART and decrypts using Speck32/64 in CTR mode.
// Synchronizes with transmitter using TAG, receives nonce, then decrypts ciphertext stream.
// CTR mode: same keystream generation as encryption, XOR with ciphertext to decrypt.
// Block size: 32 bits, Key size: 64 bits, Rounds: 22

#include <stdint.h>

#define SPECK_ROUNDS   22   // Number of Speck encryption rounds
#define KEY_WORDS      4    // Key size: 4 words (64 bits total)
#define BLOCK_BYTES    4    // Block size: 4 bytes (32 bits)
#define UART_BAUD      9600 // UART baud rate

// Synchronization tag: must match transmitter
static const uint8_t TAG[4] = { 'K', 'E', 'Y', '!' };

// Master key: 64 bits (must match transmitter)
static const uint16_t KEY[KEY_WORDS] = {
  0xA1B2, 0xC3D4, 0xE5F6, 0x1357
};

static uint16_t roundKey[SPECK_ROUNDS];  // Expanded round keys

// Rotate right: circular right shift by r bits (0-15)
static inline uint16_t ror16(uint16_t v, uint8_t r) {
  r &= 15;
  return (uint16_t)((v >> r) | (uint16_t)(v << (16 - r)));
}

// Rotate left: circular left shift by r bits (0-15)
static inline uint16_t rol16(uint16_t v, uint8_t r) {
  r &= 15;
  return (uint16_t)((uint16_t)(v << r) | (v >> (16 - r)));
}

// Speck key schedule: expands 64-bit master key into 22 round keys
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

// Speck block encryption: encrypts 32-bit block (two 16-bit words)
// Note: In CTR mode, decryption uses same encryption function
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

static bool readU32_LE(uint32_t &out) {
  if (Serial.available() < 4) return false;
  uint32_t v = 0;
  v |= (uint32_t)(uint8_t)Serial.read();
  v |= (uint32_t)(uint8_t)Serial.read() << 8;
  v |= (uint32_t)(uint8_t)Serial.read() << 16;
  v |= (uint32_t)(uint8_t)Serial.read() << 24;
  out = v;
  return true;
}

static bool syncAndReadNonce(uint32_t &nonceOut) {
  static uint8_t match = 0;

  while (Serial.available() > 0) {
    uint8_t c = (uint8_t)Serial.read();

    if (c == TAG[match]) {
      match++;
      if (match == 4) {
        match = 0;
        while (!readU32_LE(nonceOut)) { }
        return true;
      }
    } else {
      match = (c == TAG[0]) ? 1 : 0;
    }
  }
  return false;
}

void setup() {
  Serial.begin(UART_BAUD);
  makeRoundKeys(KEY, roundKey);
  Serial.println(F("Receiver ready. Waiting for KEY! tag..."));
}

void loop() {
  static bool isSynced = false;

  if (!isSynced) {
    uint32_t nonce = 0;
    if (syncAndReadNonce(nonce)) {
      stream.reset(nonce);
      isSynced = true;
      Serial.print(F("Synced. Nonce = "));
      Serial.println(nonce);
    }
    return;
  }

  if (Serial.available() > 0) {
    uint8_t c = (uint8_t)Serial.read();
    uint8_t p = stream.xorByte(c);

    Serial.print(F("Cipher: "));
    Serial.print(c);
    Serial.print(F("  Plain: "));
    Serial.println(p);
  }
}