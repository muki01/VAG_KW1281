uint8_t messageCount = 0;
String ecuData[10];
uint8_t supportedGroups[256];
uint8_t supportedGroupsCount;

void KW1281() {
  if (initOBD2()) {
    ledGreen();
    readECUInfo();
    getSupportedGroups();
    while (true) {
      getPID(1);
      getPID(2);
      getPID(3);
      getPID(4);

      //readDTC();
      //clearDTC();

      //getECUInfo();
    }
  }
}

void KW1281_Simulator() {
  if (connectionStatus == true) {
    writeBlock(ECU_Data1_Response, sizeof(ECU_Data1_Response));
    while (connectionStatus == true) {
      if (readBlock()) {
        if (resultBuffer[2] == 0x09) writeBlock(ACKNOWLEDGE_Response, sizeof(ACKNOWLEDGE_Response));
        if (resultBuffer[2] == 0x07) writeBlock(readDTC_Response, sizeof(readDTC_Response));
        if (resultBuffer[2] == 0x05) writeBlock(readDTC_Response, sizeof(readDTC_Response));
        if (resultBuffer[2] == 0x06) connectionStatus = false;
        if (resultBuffer[2] == 0x29) {
          if (resultBuffer[3] == 0x01) writeBlock(LiveData1_Response, sizeof(LiveData1_Response));
          if (resultBuffer[3] == 0x02) writeBlock(LiveData2_Response, sizeof(LiveData2_Response));
          if (resultBuffer[3] == 0x03) writeBlock(LiveData3_Response, sizeof(LiveData3_Response));
          if (resultBuffer[3] == 0x04) writeBlock(LiveData4_Response, sizeof(LiveData4_Response));
          if (resultBuffer[3] == 0x05) writeBlock(LiveData5_Response, sizeof(LiveData5_Response));
          if (resultBuffer[3] == 0x06) writeBlock(LiveData6_Response, sizeof(LiveData6_Response));
          if (resultBuffer[3] == 0x07) writeBlock(LiveData7_Response, sizeof(LiveData7_Response));
          if (resultBuffer[3] == 0x08) writeBlock(LiveData8_Response, sizeof(LiveData8_Response));
          if (resultBuffer[3] == 0x09) writeBlock(LiveData9_Response, sizeof(LiveData9_Response));
          if (resultBuffer[3] == 0x10) writeBlock(LiveData10_Response, sizeof(LiveData10_Response));
          if (resultBuffer[3] == 0x11) writeBlock(LiveData11_Response, sizeof(LiveData11_Response));
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
  debugPrintln(F("Trying KW1281"));
  setSerial(false, 9600);
  delay(3000);
  send5baud(0x01);

  setSerial(true, 9600);
  DATA_REQUEST_INTERVAL = 30;

  if (readRawData()) {
    if (resultBuffer[0] == 0x55) {
      debugPrintln(F("➡️ Writing KW2 Reversed"));
      K_Serial.write(~resultBuffer[2]);  //0xF7
      clearEcho(1);

      DATA_REQUEST_INTERVAL = 60;

      if (readRawData()) {
        if (resultBuffer[0]) {
          connectionStatus = true;
          debugPrintln(F("✅ Connection established with car"));
          debugPrintln();
          debugPrintln();
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
  // debugPrint(F("➡️ Sending Byte: "));
  // debugPrintHex(data);
  // debugPrintln();

  K_Serial.write(data);
  clearEcho(1);
}

void writeBlock(const uint8_t* dataArray, uint8_t length) {
  debugPrint(F("➡️ Sending Full Data: "));
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
  debugPrint(F("✅ Message Count: "));
  debugPrintHex(messageCount);
  debugPrint(F("   Sended All Data: "));
  for (int i = 0; i < newLength; i++) {
    debugPrintHex(newArray[i]);
    debugPrint(F(" "));
  }
  debugPrintln();
}

void writeRawData(const uint8_t* dataArray, uint8_t length, uint8_t checksumType) {
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
    debugPrint(F(" "));
    delay(WRITE_DELAY);
  }
  debugPrintln();
  clearEcho(totalLength);
}


// ----------------------------------------- Read Functions ---------------------------------------

int readByte() {
  unsigned long startMillis = millis();  // Start time for waiting the first byte

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available()) {                    // If data available
      uint8_t receivedData = K_Serial.read();
      delay(2);
      // debugPrint(F("✅ Received Byte: "));
      // debugPrintHex(receivedData);
      // debugPrintln();

      return receivedData;
    }
    delayMicroseconds(100);
  }

  debugPrintln(F("❌ Timeout: Not Received Data."));  // If no data is received within 1 seconds
  return -1;
}

uint8_t readBlock() {
  debugPrint(F("↩️ Reading Full Data: "));
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
        debugPrint(F("✅ Message Count: "));
        debugPrintHex(messageCount);
        //debugPrint(F("   Length: "));
        //debugPrintHex(messageLength);
        debugPrint(F("   Received All Data: "));
        for (int i = 0; i < receiveLength; i++) {
          debugPrintHex(resultBuffer[i]);
          debugPrint(F(" "));
        }
        debugPrintln();
        debugPrintln();
        return receiveLength;
      }
      writeByte(~receivedByte);
    }
  }
}

int readRawData() {
  debugPrint(F("Reading Raw Data: "));
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available() > 0) {                // If the first byte is received
      unsigned long lastByteTime = millis();       // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

      // Inner loop: Read all data
      debugPrint(F("✅ Received Data: "));
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {               // If buffer is full, stop reading and print message
            debugPrintln(F("\nBuffer is full. Stopping data reception."));
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrint(F(" "));
          bytesRead++;
          lastByteTime = millis();  // Reset timer
        }
      }
      debugPrintln(F("\n✅ Data reception completed."));
      return bytesRead;  // If no new data is received within 60ms, exit the loop
    }
    delayMicroseconds(100);
  }
  debugPrintln(F("❌ Timeout: Not Received Data."));
  return 0;  // If no data is received within 1 seconds
}

void clearEcho(int length) {
  unsigned long startMillis = millis();            // Start time for waiting the first byte
  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available()) {                    // If data available
      //debugPrint(F("🗑️ Cleared Echo Data: "));
      for (int i = 0; i < length; i++) {
        byte receivedByte = K_Serial.read();
        //debugPrintHex(receivedByte);
        //debugPrint(F(" "));
      }
      return;
      //debugPrintln();
    }
    delayMicroseconds(100);
  }
  debugPrintln(F("Not Received Echo Data"));
}



// ----------------------------------------- Functions ----------------------------------------

void getPID(uint8_t group) {
  Serial.print("✅ Reading Group: "), Serial.println(group);
  uint8_t pidBytes[3] = { 0x29, 0x00, 0x03 };
  pidBytes[1] = group;
  writeBlock(pidBytes, 3);
  int length = readBlock();

  if (resultBuffer[2] == 0xE7) {
    Serial.println("✅ Calculating Data.");
    int pidStart = 3;
    int pidBytesLength = length - pidStart;
    int pidCount = pidBytesLength / 3;  // 3 byte = 1 PID

    for (int i = 0; i < pidCount; i++) {
      int index = pidStart + i * 3;

      uint8_t id = resultBuffer[index];
      uint8_t a = resultBuffer[index + 1];
      uint8_t b = resultBuffer[index + 2];

      if (id < 0x01 || id > 0xB5) continue;

      float value = calculatePID(id, a, b);

      Serial.print(pidTable[id - 1].name);
      Serial.print(": ");
      Serial.print(value);
      Serial.print(" ");
      Serial.println(pidTable[id - 1].unit);
    }
  } else {
    debugPrint(F("❌ Group "));
    debugPrint(group);
    debugPrintln(F(" is not supported."));
  }
  Serial.println();
}

void readDTC() {
  Serial.println("✅ Reading DTCs.");
  writeBlock(readDTCs, 2);
  int length = readBlock();

  if (resultBuffer[2] == 0xFC) {
    int dtcStart = 3;
    int dtcBytesLength = length - 4;
    int dtcCount = dtcBytesLength / 3;

    if (dtcCount == 0) {
      Serial.println("✅ No DTCs Found.");
      return;
    }

    for (int i = 0; i < dtcCount; i++) {
      int index = dtcStart + i * 3;

      uint8_t high = resultBuffer[index];
      uint8_t low = resultBuffer[index + 1];
      uint8_t detail = resultBuffer[index + 2];

      if (high == 0x00 && low == 0x00 && detail == 0x00) continue;  // 0x00 0x00 0x00 empty DTC

      uint16_t dtcCode = ((uint16_t)high << 8) | low;

      char dtcString[6];
      sprintf(dtcString, "%05u", dtcCode);

      Serial.print("DTC: "), Serial.print(dtcString);
      Serial.print("  Detail: 0x"), Serial.println(detail, HEX);
    }
  }
}

void clearDTC() {
  Serial.println("✅ Clearing DTCs.");
  writeBlock(clearDTCs, 2);
  int length = readBlock();

  if (resultBuffer[2] == 0x09) {
    Serial.println("✅ DTCs Cleared Successfully.");
  } else {
    Serial.println("❌ DTCs Not Cleared.");
  }
}

void getECUInfo() {
  writeBlock(ECU_Data, 2);
  readECUInfo();
}

void readECUInfo() {
  Serial.println("✅ Reading ECU Info.");
  int ecuInfoLength = 0;

  for (int i = 0; i < sizeof(ecuData); i++) {
    int byteLength = readBlock();
    if (resultBuffer[2] == 0x09) break;

    ecuInfoLength++;
    writeBlock(ACKNOWLEDGE, 2);

    ecuData[i] = extractAscii(resultBuffer, byteLength);
  }

  for (int i = 0; i < ecuInfoLength; i++) {
    Serial.println("ECU Data " + String(i + 1) + ": " + ecuData[i]);
  }
}

void getSupportedGroups() {
  Serial.println("✅ Reading Supported Groups.");
  uint8_t pidBytes[3] = { 0x29, 0x00, 0x03 };
  int supportedCount = 0;

  for (uint16_t group = 0x00; group <= 0xFF; group++) {
    pidBytes[1] = group;

    writeBlock(pidBytes, 3);
    int length = readBlock();

    if (resultBuffer[2] == 0xE7) {
      supportedGroups[supportedCount++] = group;
    }
  }
  supportedGroupsCount = supportedCount;

  Serial.println("Supported Groups:");
  for (int i = 0; i < supportedCount; i++) {
    if (supportedGroups[i] < 0x10) Serial.print("0");
    Serial.print(supportedGroups[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  Serial.print("Total Supported Groups: ");
  Serial.println(supportedCount);
}



// ----------------------------------------- 5 Baud Functions ---------------------------------------

// Format - 7O1
void send5baud(uint8_t data) {
  debugPrintln(F("✅ Sending 5 Baud data."));
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

  debugPrint(F("➡️ 5 Baud Init for Module 0x"));
  debugPrintHex(data);
  debugPrint(F(": "));

  // Set txPin as output
  pinMode(K_line_TX, OUTPUT);

  for (int i = 0; i < 10; i++) {
    debugPrint(bits[i] ? "1" : "0");
    digitalWrite(K_line_TX, bits[i] ? HIGH : LOW);
    delay(200);
  }

  debugPrintln();
}

// Format - 7O1
int read5baud() {
  debugPrintln(F("✅ Reading 5 Baud data."));
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
    debugPrintln(F("Parity error!"));
    return -2;
  }

  debugPrint(F("✅ Received 5 Baud data: "));
  debugPrintHex(data);
  debugPrintln();
  return data;
}
