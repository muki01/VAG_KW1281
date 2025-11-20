uint8_t messageCount = 0;

void KW1281() {
  if (initOBD2()) {
    ledGreen();
    while (true) {
      readBlock();
      writeBlock(ACT, sizeof(ACT));
    }
  }
}

void KW1281_Simulator() {
  if (connectionStatus == true) {
    writeBlock(ECU_ID_Response, sizeof(ECU_ID_Response));
    while (connectionStatus == true) {
      if (readBlock()) {
        if (resultBuffer[2] == 0x09) writeBlock(ACT_Response, sizeof(ACT_Response));
        if (resultBuffer[2] == 0x07) writeBlock(readDTC_Response, sizeof(readDTC_Response));
        if (resultBuffer[2] == 0x05) writeBlock(readDTC_Response, sizeof(readDTC_Response));
        if (resultBuffer[2] == 0x06) connectionStatus = false;
        if (resultBuffer[2] == 0x29) {
          if (resultBuffer[3] == 0x01) writeBlock(LiveData1_Response, sizeof(LiveData1_Response));
          if (resultBuffer[3] == 0x05) writeBlock(LiveData5_Response, sizeof(LiveData5_Response));
        }
      }
    }
  } else {
    setSerial(false, 9600);
    int receivedByte = read5baud();
    if (receivedByte >= 0) {
      debugPrint(F("Received Data: "));
      debugPrintHex(receivedByte);
      debugPrintln();
      if (receivedByte == 0x01 || receivedByte == 0x03) {
        setSerial(true, 9600);
        writeRawData(initKW1281, sizeof(initKW1281), false);
        if (readRawData()) {
          if (resultBuffer[0] == 0x75) {
            debugPrintln(F("Connection Established !!"));
            connectionStatus = true;
            ledGreen();
          }
        }
      }
    }
  }
}

bool initOBD2() {
  debugPrintln(F("Trying ISO9141 or KW1281"));
  setSerial(false, 9600);
  delay(3000);
  send5baud(0x01);

  setSerial(true, 9600);
  DATA_REQUEST_INTERVAL = 30;

  if (readRawData()) {
    if (resultBuffer[0] == 0x55) {
      debugPrintln(F("➡️ Writing KW2 Reversed"));
      K_Serial.write(~resultBuffer[2]);  //0xF7
      delay(WRITE_DELAY);
      clearEcho(1);

      DATA_REQUEST_INTERVAL = 60;

      if (readRawData()) {
        if (resultBuffer[0]) {
          connectionStatus = true;
          debugPrintln(F("✅ Connection established with car"));
          debugPrintln("");
          debugPrintln("");
          return true;
        } else {
          debugPrintln(F("❌ No Data Retrieved from Car"));
        }
      }
    }
  } else {
    DATA_REQUEST_INTERVAL = 60;
  }
  return false;
}

// ----------------------------------------- Write Functions ---------------------------------------

void writeByte(uint8_t data) {
  debugPrint(F("➡️ Sending Byte: "));
  debugPrintHex(data);
  debugPrintln(F(""));

  K_Serial.write(data);
  delay(3);

  clearEcho(1);
}

void writeBlock(const uint8_t* dataArray, uint8_t length) {
  debugPrintln("➡️ Sending Full Data...");
  uint8_t newLength = length + 2;  // New array size
  uint8_t newArray[newLength];

  messageCount = messageCount + 1;

  newArray[0] = length + 1;    // byte 0 = length + 1
  newArray[1] = messageCount;  // byte 1 = messageCount
  for (uint8_t i = 0; i < length; i++) {
    newArray[i + 2] = dataArray[i];  // dataArray
  }

  // Send
  for (size_t i = 0; i < newLength; i++) {
    writeByte(newArray[i]);

    if (!readByte()) break;
  }
  debugPrint("✅ Message Count: ");
  debugPrintHex(messageCount);
  debugPrint("   Sended All Data: ");
  for (int i = 0; i < newLength; i++) {
    debugPrintHex(newArray[i]);
    debugPrint(" ");
  }
  debugPrintln("");
  debugPrintln("");
}

void writeRawData(const uint8_t *dataArray, uint8_t length, uint8_t checksumType) {
  uint8_t totalLength = length;  // default: no checksum
  uint8_t checksum = 0;

  switch (checksumType) {
    case 0:
      totalLength = length;
      break;
    case 1:
      checksum = checksum8_XOR(dataArray, length);
      totalLength = length + 1;
      break;
    case 2:
      checksum = checksum8_Modulo256(dataArray, length);
      totalLength = length + 1;
      break;
    case 3:
      checksum = checksum8_TwosComplement(dataArray, length);
      totalLength = length + 1;
      break;
    default:
      totalLength = length;
      break;
  }

  uint8_t sendData[totalLength];
  memcpy(sendData, dataArray, length);
  if (checksumType != 0) {
    sendData[totalLength - 1] = checksum;
  }

  //printPacket(sendData, totalLength);

  debugPrint(F("➡️ Sending Raw Data: "));
  for (size_t i = 0; i < totalLength; i++) {
    K_Serial.write(sendData[i]);
    debugPrintHex(sendData[i]);
    debugPrint(" ");
    delay(WRITE_DELAY);
  }
  debugPrintln(F(""));
  clearEcho(totalLength);
}

// ----------------------------------------- Read Functions ---------------------------------------

int readByte() {
  unsigned long startMillis = millis();  // Start time for waiting the first byte

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available() > 0) {                // If data available
      uint8_t receivedData = K_Serial.read();
      delay(1);
      debugPrint("✅ Received Data: ");
      debugPrintHex(receivedData);
      debugPrintln("");

      return receivedData;
    }
  }

  debugPrintln("❌ Timeout: Not Received Data.");  // If no data is received within 1 seconds
  return -1;
}

uint8_t readBlock() {
  debugPrintln("Reading Full Data...");
  uint8_t receiveLength = 0;
  uint8_t messageLength = 0;

  memset(resultBuffer, 0, sizeof(resultBuffer));
  while (true) {

    int receivedByte = readByte();
    if (receivedByte > -1) {
      resultBuffer[receiveLength] = receivedByte;
      if (receiveLength == 0) messageLength = receivedByte;
      if (receiveLength == 1) messageCount = receivedByte;

      receiveLength++;

      if (messageLength < receiveLength) {
        debugPrint("✅ Message Count: ");
        debugPrintHex(messageCount);
        debugPrint("   Length: ");
        debugPrintHex(messageLength);
        debugPrint("   Received All Data: ");
        for (int i = 0; i < receiveLength; i++) {
          debugPrintHex(resultBuffer[i]);
          debugPrint(" ");
        }
        debugPrintln("");
        debugPrintln("");
        return receiveLength;
      }
      writeByte(~receivedByte);
    }
  }
}

int readRawData() {
  debugPrintln("Reading Raw Data...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait for data for the specified timeout
  while (millis() - startMillis < DATA_TIMEOUT) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

      // Inner loop: Read all data
      debugPrint("✅ Received Data: ");
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {
            debugPrintln("\nBuffer is full. Stopping data reception.");
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrint(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
        }
      }

      // If no new data is received within 60ms, exit the loop
      debugPrintln("\n✅ Data reception completed.");
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  debugPrintln("❌ Timeout: Not Received Data.");
  return 0;
}


void clearEcho(int length) {
  int result = K_Serial.available();
  if (result > 0) {
    debugPrint("🗑️ Cleared Echo Data: ");
    for (int i = 0; i < length; i++) {
      byte receivedByte = K_Serial.read();
      debugPrintHex(receivedByte);
      debugPrint(" ");
    }
    debugPrintln();
  } else {
    debugPrintln("❌ Not Received Echo Data");
  }
}



// ----------------------------------------- 5 Baud Functions ---------------------------------------

// Format - 7O1
void send5baud(uint8_t data) {
  uint8_t bits[10];
  bits[0] = 0;  // start bit
  bits[9] = 1;  // stop bit

  // 7-bit data
  for (int i = 0; i < 7; i++) {
    bits[i + 1] = (data >> i) & 1;
  }

  // Odd parity calculation
  uint8_t ones = 0;
  for (int i = 1; i <= 7; i++) {
    if (bits[i]) ones++;
  }
  bits[8] = (ones % 2 == 0) ? 1 : 0;  // parity bit

  debugPrint("➡️ 5 Baud Init for Module 0x");
  debugPrintHex(data);
  debugPrint(": ");

  // Set txPin as output
  pinMode(K_line_TX, OUTPUT);

  for (int i = 0; i < 10; i++) {
    debugPrint(bits[i] ? "1" : "0");
    digitalWrite(K_line_TX, bits[i] ? HIGH : LOW);
    delay(200);
  }

  debugPrintln("");
}

// Format - 7O1
int read5baud() {
  unsigned long t0 = millis();
  while (digitalRead(K_line_RX) == HIGH) {
    if (millis() - t0 > 2000) return -1;
  }

  uint8_t bits[10];
  uint8_t data = 0;
  int ones = 0;
  delayMicroseconds(100000);

  for (int i = 0; i < 10; i++) {  // Bits: 0=start, 1..7=data, 8=parity, 9=stop
    bits[i] = digitalRead(K_line_RX) ? 1 : 0;

    if (i >= 1 && i <= 7) {
      data |= (bits[i] << (i - 1));  // Save data bits
      if (bits[i]) ones++;
    } else if (i == 8) {  // parity bit
      if (bits[i]) ones++;
    } else if (i == 9) {  // stop bit
      break;
    }

    delayMicroseconds(200000);
  }

  // Parity controll (Odd)
  if (ones % 2 == 0) {
    debugPrintln("Parity error!");
    return -2;
  }

  return data;
}
