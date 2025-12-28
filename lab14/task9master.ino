// Lab 14 - Task 9: Speck32/64 Cipher in CTR Mode (Transmitter)
// Hardware: ATmega328P (Arduino Uno R3)
// Description: Implements Speck32/64 block cipher in CTR (Counter) mode for secure
// serial communication. Speck is a lightweight ARX (Add-Rotate-XOR) cipher designed
// for resource-constrained devices like microcontrollers.
// Block size: 32 bits (4 bytes), Key size: 64 bits (4 words), Rounds: 22
// Mode: CTR (Counter) - generates keystream by encrypting counter values
// Suitability: Small code size, low RAM usage, efficient on ATmega328P instruction set

#include <stdint.h>

#define SPECK_ROUNDS   22   // Number of Speck encryption rounds
#define KEY_WORDS      4    // Key size: 4 words (64 bits total)
#define BLOCK_BYTES    4    // Block size: 4 bytes (32 bits)
#define UART_BAUD      9600 // UART baud rate

// Synchronization tag: 4-byte identifier for receiver to detect start of transmission
static const uint8_t TAG[4] = { 'K', 'E', 'Y', '!' };

// Master key: 64 bits (4 words of 16 bits each)
// Must match receiver for successful decryption
static const uint16_t KEY[KEY_WORDS] = {
  0xA1B2, 0xC3D4, 0xE5F6, 0x1357
};

static uint16_t roundKey[SPECK_ROUNDS];  // Expanded round keys for encryption

// Rotate right: circular right shift by r bits (0-15)
static inline uint16_t ror16(uint16_t v, uint8_t r) {
  r &= 15;  // Limit rotation to 0-15 bits
  return (uint16_t)((v >> r) | (uint16_t)(v << (16 - r)));
}

// Rotate left: circular left shift by r bits (0-15)
static inline uint16_t rol16(uint16_t v, uint8_t r) {
  r &= 15;  // Limit rotation to 0-15 bits
  return (uint16_t)((uint16_t)(v << r) | (v >> (16 - r)));
}

// Speck key schedule: expands 64-bit master key into 22 round keys
// Uses ARX operations (Add-Rotate-XOR) optimized for 16-bit operations
static void makeRoundKeys(const uint16_t k[KEY_WORDS], uint16_t rk[SPECK_ROUNDS]) {
  uint16_t L[3] = { k[0], k[1], k[2] };  // First 3 key words
  rk[0] = k[3];  // First round key is last key word

  uint8_t idx = 0;  // Index rotating through L array
  for (uint8_t i = 0; i < SPECK_ROUNDS - 1; ++i) {
    // Generate next round key using ARX operations
    uint16_t tmp = (uint16_t)((ror16(L[idx], 7) + rk[i]) ^ i);
    rk[i + 1] = (uint16_t)(rol16(rk[i], 2) ^ tmp);
    L[idx] = tmp;
    idx = (uint8_t)((idx + 1) % 3);  // Rotate through L[0..2]
  }
}

// Speck block encryption: encrypts 32-bit block (two 16-bit words)
// Uses ARX structure: Add, Rotate, XOR operations
static void speckEncryptBlock(uint16_t &a, uint16_t &b, const uint16_t rk[SPECK_ROUNDS]) {
  for (uint8_t i = 0; i < SPECK_ROUNDS; ++i) {
    // Speck round function: ARX operations
    a = (uint16_t)((ror16(a, 7) + b) ^ rk[i]);
    b = (uint16_t)(rol16(b, 2) ^ a);
  }
}

// CTR mode keystream generator
// CTR mode: encrypts counter values to generate keystream, then XORs with plaintext
// Advantages: parallelizable, no padding needed, same encryption function for encrypt/decrypt
struct CtrStream {
  uint32_t counter;        // Current counter value (incremented for each block)
  uint8_t  buf[BLOCK_BYTES]; // Keystream buffer (4 bytes from one block encryption)
  uint8_t  pos;            // Current position in keystream buffer

  // Initialize counter with nonce (number used once)
  void reset(uint32_t nonce) {
    counter = nonce;
    pos = BLOCK_BYTES;  // Force refill on first use
  }

  // Generate next 4 bytes of keystream by encrypting counter
  void refill() {
    // Split 32-bit counter into two 16-bit words for Speck encryption
    uint16_t a = (uint16_t)(counter >> 16);  // Upper 16 bits
    uint16_t b = (uint16_t)(counter & 0xFFFF); // Lower 16 bits

    // Encrypt counter to generate keystream block
    speckEncryptBlock(a, b, roundKey);

    // Combine encrypted words back into 32-bit value
    uint32_t w = ((uint32_t)a << 16) | (uint32_t)b;

    // Extract keystream bytes in little-endian order
    buf[0] = (uint8_t)(w & 0xFF);
    buf[1] = (uint8_t)((w >> 8) & 0xFF);
    buf[2] = (uint8_t)((w >> 16) & 0xFF);
    buf[3] = (uint8_t)((w >> 24) & 0xFF);

    pos = 0;      // Reset buffer position
    counter++;    // Increment counter for next block
  }

  // XOR one byte of plaintext with keystream to produce ciphertext
  // CTR mode: ciphertext = plaintext XOR keystream
  uint8_t xorByte(uint8_t in) {
    if (pos >= BLOCK_BYTES) refill();  // Refill buffer if exhausted
    return (uint8_t)(in ^ buf[pos++]);  // XOR and advance position
  }
};

static CtrStream stream;  // Global keystream generator

// Send 32-bit value as little-endian bytes over UART
static void sendU32_LE(uint32_t v) {
  Serial.write((uint8_t)(v & 0xFF));
  Serial.write((uint8_t)((v >> 8) & 0xFF));
  Serial.write((uint8_t)((v >> 16) & 0xFF));
  Serial.write((uint8_t)((v >> 24) & 0xFF));
}

// Send synchronization tag and nonce to receiver
// Tag allows receiver to detect start of transmission
// Nonce initializes counter for CTR mode
static void sendTagAndNonce(uint32_t nonce) {
  Serial.write(TAG, 4);      // Send 4-byte tag
  sendU32_LE(nonce);         // Send nonce (little-endian)
}

// Generate pseudo-random nonce from system state
// Uses micros() timer and analog readings for entropy
static uint32_t makeNonce() {
  uint32_t a = (uint32_t)micros();           // Timer value
  uint32_t b = (uint32_t)analogRead(A0);     // Analog noise
  uint32_t c = (uint32_t)analogRead(A1);     // Analog noise
  // Combine with XOR and shifts, add constant for mixing
  return (a << 1) ^ (b << 17) ^ (c << 3) ^ 0x9E3779B9UL;
}

void setup() {
  Serial.begin(UART_BAUD);
  makeRoundKeys(KEY, roundKey);  // Expand master key into round keys

  uint32_t nonce = makeNonce();  // Generate unique nonce
  stream.reset(nonce);           // Initialize CTR stream with nonce

  delay(500);                     // Wait for receiver to boot
  sendTagAndNonce(nonce);         // Send tag and nonce for synchronization
}

void loop() {
  static uint8_t plain = 0;  // Plaintext counter (0-255)

  uint8_t cipher = stream.xorByte(plain);  // Encrypt: XOR plaintext with keystream
  Serial.write(cipher);  // Send ciphertext over UART
  plain++;  // Increment plaintext counter

  delay(100);  // Send every 0.1 seconds
}