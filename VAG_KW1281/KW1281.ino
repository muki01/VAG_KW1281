uint8_t messageCount = 0;

void KW1281() {
  if (initOBD2()) {
    while (true) {
      readBlock();
      writeBlock(ACT, sizeof(ACT));
    }
  }
}

// void KW1281_Simulator() {
//   if (connectionStatus == true) {
//     if (readKW1281()) {
//       if (compareData(ACT, sizeof(ACT))) {
//         writeRawData(ACT_Response, sizeof(ACT_Response), 0);
//       }
//       // if (resultBuffer[2] == 0x09) {
//       //   writeRawDataKW1281(ACT, sizeof(ACT));
//       // }
//     }
//   } else {
//     int receivedByte = readByte7O1();
//     if (receivedByte >= 0) {
//       debugPrint(F("Received Data: "));
//       debugPrintHex(receivedByte);
//       debugPrintln();
//       if (receivedByte == 0x01 || receivedByte == 0x03) {
//         setSerial(true, 9600);
//         writeRawData(initKW1281, sizeof(initKW1281), false);
//         if (readData()) {
//           if (resultBuffer[0] == 0x75) {
//             debugPrintln(F("Connection Established !!"));
//             connectionStatus = true;
//             ledGreen();
//           }
//         }
//       }
//     }
//   }
// }

bool initOBD2() {
  debugPrintln(F("Trying ISO9141 or KW1281"));
  setSerial(false, 9600);
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

int readByte() {
  unsigned long startMillis = millis();  // Start time for waiting the first byte

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available() > 0) {                // If the first byte is received
      unsigned long lastByteTime = millis();       // Get the last received byte time

      uint8_t receivedData = K_Serial.read();
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
  while (true) {

    int receivedByte = readByte();
    if (receivedByte > -1) {
      resultBuffer[receiveLength] = receivedByte;
      receiveLength++;
      if (receiveLength == 2) {
        messageCount = receivedByte;
      }
      if (receivedByte == 0x03 && receiveLength > 2) {
        debugPrint("✅ Message Count: ");
        debugPrintHex(messageCount);
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
