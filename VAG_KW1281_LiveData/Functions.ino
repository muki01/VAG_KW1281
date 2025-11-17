void setSerial(bool enabled, int BAUDRATE) {
  if (enabled) {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
    K_Serial.begin(BAUDRATE);
#elif defined(ESP8266)
    K_Serial.begin(BAUDRATE, SERIAL_8N1);
#elif defined(ESP32)
    K_Serial.begin(BAUDRATE, SERIAL_8N1, K_line_RX, K_line_TX);
#endif
  } else {
    K_Serial.end();
    pinMode(K_line_RX, INPUT_PULLUP);
    pinMode(K_line_TX, OUTPUT);
    digitalWrite(K_line_TX, HIGH);
    delay(3000);
  }
}

byte checksum8_XOR(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum ^= data[i];  // XOR işlemi
  }
  return checksum;
}

byte checksum8_Modulo256(const byte data[], int length) {
  unsigned int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += data[i];
  }
  return (byte)(sum % 256);  // veya (byte)sum; çünkü uint8_t overflow da mod 256 etkisi verir
}

byte checksum8_TwosComplement(const byte data[], int length) {
  unsigned int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += data[i];
  }
  byte checksum = (byte)((0x100 - (sum & 0xFF)) & 0xFF);
  return checksum;
}

void K_Line_Simulate() {
  int sampleIdList[] = { 0x11, 0x18, 0x2A };
  int sampleData[] = { 0x0A, 0x1B, 0x2C, 0x3D, 0x4E, 0x5F, 0xA0, 0xB1 };
  int dataLength = random(1, 9);

  for (int i = 0; i < 3; i++) {
    int idIndex = random(sizeof(sampleIdList) / sizeof(sampleIdList[0]));
    printHex(sampleIdList[idIndex]);
  }

  Serial.print(",");
  printHex(0);
  Serial.print(",");
  printHex(0);
  Serial.print(",");
  for (int i = 0; i < dataLength; i++) {
    int dataIndex = random(sizeof(sampleData) / sizeof(sampleData[0]));
    printHex(sampleData[dataIndex]);
  }

  Serial.print("\n");
  delay(200);
}

void printPacket(const byte data[], int length) {
  // printHex(data[0]);
  // printHex(data[1]);
  // printHex(data[2]);
  // Serial.print(",");
  // printHex(0);
  // Serial.print(",");
  // printHex(0);
  // Serial.print(",");

  for (int i = 0; i < length; i++) {
    printHex(data[i]);
  }
  Serial.print("\n");
}

void printHex(byte num) {
  if (num < 0x10) Serial.print("0");
  Serial.print(num, HEX);
}


void ledGreen() {
  leds[0] = CRGB::Green;
  FastLED.show();
}

void ledRed() {
  leds[0] = CRGB::Red;
  FastLED.show();
}

uint16_t To16Bit(uint8_t msb, uint8_t lsb) {
  return ((uint16_t)msb << 8) | lsb;
}

int8_t ToSigned8(uint8_t value) {
  return (int8_t)value;  // 0..255 -> -128..127
}

int16_t ToSigned16(uint8_t msb, uint8_t lsb) {
  return (int16_t)((msb << 8) | lsb);
}