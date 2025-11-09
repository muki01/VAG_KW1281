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

int readByte7O1() {
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

bool compareData(const uint8_t *dataArray, uint8_t length) {
  //debugPrintln("Comparing !");
  for (size_t i = 0; i < length; i++) {
    if (dataArray[i] != resultBuffer[i]) {
      // debugPrint("Byte: ");
      // debugPrint(i);
      // debugPrintln(" Not same!");
      return false;
    }
  }
  //debugPrintln(" Same!");
  return true;
}

int readData() {
  debugPrintln("Reading...");
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