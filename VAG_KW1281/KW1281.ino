void KW1281() {
  if (connectionStatus == true) {
    if (readKW1281()) {
      if (compareData(ACT, sizeof(ACT))) {
        writeRawData(ACT_Response, sizeof(ACT_Response), 0);
      }
      // if (resultBuffer[2] == 0x09) {
      //   writeRawDataKW1281(ACT, sizeof(ACT));
      // }
    }
  } else {
    int receivedByte = readByte7O1();
    if (receivedByte >= 0) {
      debugPrint(F("Received Data: "));
      if (receivedByte < 0x10) debugPrint("0");
      debugPrintHex(receivedByte);
      debugPrintln();
      if (receivedByte == 0x01 || receivedByte == 0x03) {
        setSerial(true, 10400);
        writeRawData(initKW1281, sizeof(initKW1281), false);
        if (readData()) {
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



void writeByte(uint8_t data) {
  debugPrint(F("➡️ Sending Byte: "));
  debugPrintHex(data);
  debugPrintln(F(""));

  K_Serial.write(data);
  delay(3);

  clearEcho(1);
}



void writeData(const uint8_t *dataArray, uint8_t length) {
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

int readKW1281() {
  debugPrintln("Reading...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait for data for the specified timeout
  while (millis() - startMillis < DATA_TIMEOUT) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

      while (millis() - lastByteTime < 20) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {       // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {
            debugPrintln("\nBuffer is full. Stopping data reception.");
            return bytesRead;
          }

          debugPrint("Received Data: ");
          resultBuffer[bytesRead] = K_Serial.read();
          if (resultBuffer[bytesRead] < 0x10) debugPrint("0");
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrintln();
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          if (bytesRead > 1 && resultBuffer[bytesRead - 1] == 0x03) break;

          writeByte(resultBuffer[bytesRead - 1], true);



          // If buffer is full, stop reading and print message
        }
      }

      debugPrint("All Data: ");
      for (int i = 0; i < bytesRead; i++) {
        if (resultBuffer[i] < 0x10) debugPrint("0");
        debugPrintHex(resultBuffer[i]);
        debugPrint(" ");
      }


      // If no new data is received within 60ms, exit the loop
      //printPacket(resultBuffer, bytesRead);
      debugPrintln("\nData reception completed.");
      debugPrintln();
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  debugPrintln("Timeout: Not Received Data.");
  return 0;
}