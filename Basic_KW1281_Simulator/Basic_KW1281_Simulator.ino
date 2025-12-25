#include "Codes.h"
//#include <AltSoftSerial.h>
//AltSoftSerial obd;

//#define K_Serial obd
//#define K_line_RX 8
//#define K_line_TX 9

#define K_Serial Serial1
#define K_line_RX 10
#define K_line_TX 11

int WRITE_DELAY = 5;             // Delay between each byte of the transmitted data (5ms - 20ms)
int DATA_REQUEST_INTERVAL = 20;  // Time to wait before sending a new request after receiving a response (55ms - 5000ms)
int DATA_TIMEOUT = 1000;         // Time to wait before sending a new request after receiving a response (500ms - 5000ms)

uint8_t messageCount = 0;
bool connectionStatus = false;
byte resultBuffer[160];

void setup() {
  Serial.begin(9600);
  setSerial(true, 9600);
}

void loop() {
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
          if (resultBuffer[3] == 0x05) writeBlock(LiveData5_Response, sizeof(LiveData5_Response));
        }
      }
    }
  } else {
    setSerial(false, 9600);
    int receivedByte = read5baud();
    if (receivedByte >= 0) {
      Serial.print(F("Received Data: "));
      Serial.print(receivedByte, HEX);
      Serial.println();
      if (receivedByte == 0x01 || receivedByte == 0x03) {
        setSerial(true, 9600);
        writeRawData(initKW1281_Response, sizeof(initKW1281_Response), false);
        if (readRawData()) {
          if (resultBuffer[0] == 0x75) {
            Serial.println(F("Connection Established !!"));
            connectionStatus = true;
          }
        }
      }
    }
  }
}

// ----------------------------------------- Write Functions ---------------------------------------

void writeByte(uint8_t data) {
  Serial.print(F("➡️ Sending Byte: "));
  Serial.print(data, HEX);
  Serial.println(F(""));

  K_Serial.write(data);
  delay(3);

  clearEcho(1);
}

void writeBlock(const uint8_t* dataArray, uint8_t length) {
  Serial.println("➡️ Sending Full Data...");
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
  Serial.print("✅ Message Count: ");
  Serial.print(messageCount, HEX);
  Serial.print("   Sended All Data: ");
  for (int i = 0; i < newLength; i++) {
    Serial.print(newArray[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("");
}

void writeRawData(const uint8_t* dataArray, uint8_t length, uint8_t checksumType) {
  uint8_t totalLength = length;  // default: no checksum
  uint8_t checksum = 0;

  switch (checksumType) {
    case 0:
      totalLength = length;
      break;
    // case 1:
    //   checksum = checksum8_XOR(dataArray, length);
    //   totalLength = length + 1;
    //   break;
    // case 2:
    //   checksum = checksum8_Modulo256(dataArray, length);
    //   totalLength = length + 1;
    //   break;
    // case 3:
    //   checksum = checksum8_TwosComplement(dataArray, length);
    //   totalLength = length + 1;
    //   break;
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

  Serial.print(F("➡️ Sending Raw Data: "));
  for (size_t i = 0; i < totalLength; i++) {
    K_Serial.write(sendData[i]);
    Serial.print(sendData[i], HEX);
    Serial.print(" ");
    delay(WRITE_DELAY);
  }
  Serial.println(F(""));
  clearEcho(totalLength);
}

// ----------------------------------------- Read Functions ---------------------------------------

int readByte() {
  unsigned long startMillis = millis();  // Start time for waiting the first byte

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available() > 0) {                // If data available
      uint8_t receivedData = K_Serial.read();
      delay(1);
      Serial.print("✅ Received Data: ");
      Serial.print(receivedData, HEX);
      Serial.println("");

      return receivedData;
    }
  }

  Serial.println("❌ Timeout: Not Received Data.");  // If no data is received within 1 seconds
  return -1;
}

uint8_t readBlock() {
  Serial.println("Reading Full Data...");
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
        Serial.print("✅ Message Count: ");
        Serial.print(messageCount, HEX);
        Serial.print("   Length: ");
        Serial.print(messageLength, HEX);
        Serial.print("   Received All Data: ");
        for (int i = 0; i < receiveLength; i++) {
          Serial.print(resultBuffer[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
        Serial.println("");
        return receiveLength;
      }
      writeByte(~receivedByte);
    }
  }
}

int readRawData() {
  Serial.println("Reading Raw Data...");
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait for data for the specified timeout
  while (millis() - startMillis < DATA_TIMEOUT) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

      // Inner loop: Read all data
      Serial.print("✅ Received Data: ");
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {
            Serial.println("\nBuffer is full. Stopping data reception.");
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          Serial.print(resultBuffer[bytesRead], HEX);
          Serial.print(" ");
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
        }
      }

      // If no new data is received within 60ms, exit the loop
      Serial.println("\n✅ Data reception completed.");
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  Serial.println("❌ Timeout: Not Received Data.");
  return 0;
}


void clearEcho(int length) {
  int result = K_Serial.available();
  if (result > 0) {
    Serial.print("🗑️ Cleared Echo Data: ");
    for (int i = 0; i < length; i++) {
      byte receivedByte = K_Serial.read();
      Serial.print(receivedByte, HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("❌ Not Received Echo Data");
  }
}



// ----------------------------------------- 5 Baud Functions ---------------------------------------

// Format - 7O1
int read5baud() {
  //Serial.println(F("Waiting for 5-baud init..."));

  // HIGH -> LOW (start bit decrease)
  while (digitalRead(K_line_RX) == HIGH)
    ;
  unsigned long tStart = micros();

  // Measure the start-bit LOW duration
  while (digitalRead(K_line_RX) == LOW)
    ;
  unsigned long lowDuration = micros() - tStart;

  // 5-baud controll
  if (lowDuration < 150000 || lowDuration > 300000) {
    //Serial.println(F("Not 5-baud, ignored"));
    return -1;
  }

  Serial.print(F("5 Baud detected - "));
  const unsigned long BIT_US = 200000;
  uint8_t bits[10];

  while (micros() - tStart < (BIT_US / 2))
    ;

  for (int i = 0; i < 10; i++) {
    bits[i] = digitalRead(K_line_RX);
    delayMicroseconds(BIT_US);
  }

  Serial.print(F("Bits: "));
  for (int i = 0; i < 10; i++) {
    Serial.print(bits[i]);
  }

  uint8_t data = 0;
  int ones = 0;
  for (int i = 1; i <= 7; i++) {
    data |= (bits[i] << (i - 1));
    if (bits[i]) ones++;
  }
  if (bits[8]) ones++;

  Serial.print(F(", DATA: 0x"));
  Serial.print(data, HEX);

  if ((ones & 1) == 0) Serial.println(F(", Parity ERROR (odd expected)"));
  else Serial.println(F(", Parity OK"));
  //Serial.println();

  return data;
}

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
  }
}