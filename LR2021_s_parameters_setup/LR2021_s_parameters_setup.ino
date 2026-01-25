#define DEBUG
#define SPI_SPEED 8000000

#include <SPI.h>

#define BUSY 3   // A4
#define NRST 6   // A2

#define SCK 21
#define MISO 23
#define MOSI 22
#define CS 5     // A3

SPIClass spi(FSPI);

/* ---------- Forward declarations ---------- */
void SetPacketType(uint8_t packetType);
void SetLoRaModulationParams(uint8_t sf, uint8_t bw, uint8_t cr, uint8_t idro);
void SetLoRaPacketParams(uint16_t pbl_len, uint8_t payload_len, uint8_t header_type, uint8_t crc, uint8_t invert_iq);
void SetRfFrequency(uint32_t frequency);
void SetRxPath(uint8_t rx_path, uint8_t rx_boost);
void SetAgcGainManual(uint8_t gain_step);
void CalibFe(void);
void SetRx(uint64_t rx_timeout);

void setup() {
  Serial.begin(115200);

  pinMode(BUSY, INPUT_PULLUP);
  pinMode(NRST, OUTPUT);
  pinMode(CS, OUTPUT);

  digitalWrite(CS, HIGH);

  digitalWrite(NRST, LOW);
  delay(10);
  digitalWrite(NRST, HIGH);

  spi.begin(SCK, MISO, MOSI, CS);
  delay(2500);

  Serial.println("Setting frequency...");
  SetRfFrequency(915000000);

  Serial.println("Setting HF path, 0 boosted gain...");
  SetRxPath(0, 0);

  Serial.println("Setting AGC manual gain...");
  SetAgcGainManual(1);

  Serial.println("Calibrating FE...");
  CalibFe();

  Serial.println("Setting packet type...");
  SetPacketType(0x00);

  Serial.println("Setting modulation params...");
  SetLoRaModulationParams(0x9, 0x4, 0x4, 0x1);

  Serial.println("Setting packet params...");
  SetLoRaPacketParams(16, 0, 0, 1, 0);

  Serial.println("Setting RX mode...");
  SetRx(0xFFFFFF);

  Serial.println("Setup done.");
}

void loop() {}

/* ---------- Radio commands ---------- */

void SetPacketType(uint8_t packetType) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[3] = { 0x02, 0x07, packetType };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 3);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetLoRaModulationParams(uint8_t sf, uint8_t bw, uint8_t cr, uint8_t idro) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t byte2 = (sf << 4) | (bw & 0x0F);
  uint8_t byte3 = (cr << 4) | (idro & 0x03);
  uint8_t bytes[4] = { 0x02, 0x20, byte2, byte3 };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 4);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetLoRaPacketParams(uint16_t pbl_len, uint8_t payload_len,
                         uint8_t header_type, uint8_t crc, uint8_t invert_iq) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[6] = {
    0x02,
    0x21,
    (uint8_t)(pbl_len >> 8),
    (uint8_t)(pbl_len & 0xFF),
    payload_len,
    (uint8_t)((header_type << 2) | (crc << 1) | invert_iq)
  };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 6);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetRfFrequency(uint32_t frequency) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[6] = {
    0x02,
    0x00,
    (uint8_t)(frequency >> 24),
    (uint8_t)(frequency >> 16),
    (uint8_t)(frequency >> 8),
    (uint8_t)(frequency)
  };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 6);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetAgcGainManual(uint8_t gain_step) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[3] = { 0x02, 0x1A, (uint8_t)(gain_step & 0x0F) };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 3);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetRxPath(uint8_t rx_path, uint8_t rx_boost) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[4] = { 0x02, 0x01, (uint8_t)(rx_path & 0x01), (uint8_t)(rx_boost & 0x07) };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 4);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void CalibFe(void) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[8] = { 0x02, 0x23, 0, 0, 0, 0, 0, 0 };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 8);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void SetRx(uint64_t rx_timeout) {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[5] = {
    0x02,
    0x0C,
    (uint8_t)((rx_timeout >> 16) & 0xFF),
    (uint8_t)((rx_timeout >> 8) & 0xFF),
    (uint8_t)(rx_timeout & 0xFF)
  };

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 5);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}
