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

  Serial.println("Clearing errors...");
  ClearErrors();
  GetAndPrintStatusAndErrors();

  Serial.println("Setting frequency...");
  SetRfFrequency(868000000);
  GetAndPrintStatusAndErrors();

  Serial.println("Setting LF path, no boosted gain...");
  SetRxPath(0, 0);
  GetAndPrintStatusAndErrors();

  Serial.println("Setting AGC manual gain...");
  SetAgcGainManual(1);
  GetAndPrintStatusAndErrors();

  Serial.println("Calibrating FE...");
  CalibFe();
  GetAndPrintStatusAndErrors();

  Serial.println("Setting packet type...");
  SetPacketType(0x00);
  GetAndPrintStatusAndErrors();

  Serial.println("Setting modulation params...");
  SetLoRaModulationParams(0x9, 0x4, 0x4, 0x1);
  GetAndPrintStatusAndErrors();

  Serial.println("Setting packet params...");
  SetLoRaPacketParams(16, 0, 0, 1, 0);
  GetAndPrintStatusAndErrors();

  Serial.println("Setting RX mode...");
  SetRx(0xFFFFFF);
  GetAndPrintStatusAndErrors();

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

  uint8_t bytes[8] = { 0x01, 0x23, 0, 0, 0, 0, 0, 0 };

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

void ClearErrors() {
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  uint8_t bytes[2] = { 0x01, 0x11};

  digitalWrite(CS, LOW);
  spi.writeBytes(bytes, 2);
  digitalWrite(CS, HIGH);

  spi.endTransaction();
}

void GetAndPrintStatusAndErrors()
{
  spi.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  while (digitalRead(BUSY)) {}

  /* -------- Phase 1: issue GetErrors command -------- */
  uint8_t cmd[2] = { 0x01, 0x10 }; // READ, GetErrors

  digitalWrite(CS, LOW);
  spi.writeBytes(cmd, 2);
  digitalWrite(CS, HIGH);

  /* -------- Wait for data to be ready -------- */
  while (digitalRead(BUSY)) {}

  /* -------- Phase 2: read response --------
     Byte 0: Stat(15:8)
     Byte 1: Stat(7:0)
     Byte 2: ErrorStat(15:8)
     Byte 3: ErrorStat(7:0)
  */
  uint8_t rx[4] = {0};

  digitalWrite(CS, LOW);
  spi.transfer(rx, 4);
  digitalWrite(CS, HIGH);

  spi.endTransaction();

  uint16_t stat     = ((uint16_t)rx[0] << 8) | rx[1];
  uint16_t errorStat = ((uint16_t)rx[2] << 8) | rx[3];

  /* ================= STATUS ================= */

  uint8_t commandStatus   = (stat >> 9) & 0x07;
  uint8_t interruptStatus = (stat >> 8) & 0x01;
  uint8_t resetSource     = (stat >> 4) & 0x0F;
  uint8_t chipMode        =  stat       & 0x07;

  Serial.println(F("=== LR2021 STATUS ==="));
  Serial.printf("Raw Status: 0x%04X\n", stat);

  Serial.print(F("CommandStatus: "));
  switch (commandStatus) {
    case 0x0: Serial.println(F("CMD_FAIL")); break;
    case 0x1: Serial.println(F("CMD_PERR")); break;
    case 0x2: Serial.println(F("CMD_OK"));   break;
    case 0x3: Serial.println(F("CMD_DAT"));  break;
    default:  Serial.println(F("Reserved")); break;
  }

  Serial.print(F("InterruptStatus: "));
  Serial.println(interruptStatus ? F("IRQ active") : F("No IRQ"));

  Serial.print(F("ResetSource: "));
  switch (resetSource) {
    case 0x0: Serial.println(F("Cleared")); break;
    case 0x1: Serial.println(F("POR / Brown-Out")); break;
    case 0x2: Serial.println(F("NRESET pin")); break;
    case 0x3: Serial.println(F("RFU")); break;
    default:  Serial.println(F("Reserved")); break;
  }

  Serial.print(F("ChipMode: "));
  switch (chipMode) {
    case 0x0: Serial.println(F("SLEEP")); break;
    case 0x1: Serial.println(F("STDBY RC")); break;
    case 0x2: Serial.println(F("STDBY XOSC")); break;
    case 0x3: Serial.println(F("FS")); break;
    case 0x4: Serial.println(F("RX")); break;
    case 0x5: Serial.println(F("TX")); break;
    default:  Serial.println(F("Reserved")); break;
  }

  /* ================= ERRORS ================= */

  Serial.println(F("=== ERROR STATUS ==="));
  Serial.printf("Raw ErrorStat: 0x%04X\n", errorStat);

  if (errorStat == 0) {
    Serial.println(F("No errors"));
  } else {
    if (errorStat & (1 << 0))  Serial.println(F("HF_XOSC_START_ERR"));
    if (errorStat & (1 << 1))  Serial.println(F("LF_XOSC_START_ERR"));
    if (errorStat & (1 << 2))  Serial.println(F("PLL_LOCK_ERR"));
    if (errorStat & (1 << 3))  Serial.println(F("LF_RC_CALIB_ERR"));
    if (errorStat & (1 << 4))  Serial.println(F("HF_RC_CALIB_ERR"));
    if (errorStat & (1 << 5))  Serial.println(F("PLL_CALIB_ERR"));
    if (errorStat & (1 << 6))  Serial.println(F("AAF_CALIB_ERR"));
    if (errorStat & (1 << 7))  Serial.println(F("IMG_CALIB_ERR"));
    if (errorStat & (1 << 8))  Serial.println(F("CHIP_BUSY"));
    if (errorStat & (1 << 9))  Serial.println(F("RXFREQ_NO_FE_CAL_ERR"));
    if (errorStat & (1 << 10)) Serial.println(F("MEAS_UNIT_ADC_CALIB_ERR"));
    if (errorStat & (1 << 11)) Serial.println(F("PA_OFFSET_CALIB_ERR"));
    if (errorStat & 0xF000)    Serial.println(F("RFU bits set (12–15)"));
  }

  Serial.println();
}