//#include <AltSoftSerial.h>
//AltSoftSerial obd;

//#define K_Serial obd 
//#define K_line_RX 8
//#define K_line_TX 9

#define K_Serial Serial1
#define K_line_RX 10
#define K_line_TX 11

int WRITE_DELAY = 5;
int DATA_REQUEST_INTERVAL = 30;
int DATA_TIMEOUT = 1000;

uint8_t messageCount = 0;
bool connectionStatus = false;
uint8_t ACKNOWLEDGE[2] = { 0x09, 0x03 };
byte resultBuffer[160];

void setup() {
  Serial.begin(9600);
  setSerial(true, 9600);
}

void loop() {
  if (initOBD2()) {
    while (true) {
      readBlock();
      writeBlock(ACKNOWLEDGE, sizeof(ACKNOWLEDGE));
    }
  }
}

bool initOBD2() {
  Serial.println(F("Trying KW1281"));
  setSerial(false, 9600);
  delay(3000);
  send5baud(0x01);

  setSerial(true, 9600);
  DATA_REQUEST_INTERVAL = 30;

  if (readRawData()) {
    if (resultBuffer[0] == 0x55) {
      Serial.println(F("➡️ Writing KW2 Reversed"));
      K_Serial.write(~resultBuffer[2]);  //0xF7
      delay(WRITE_DELAY);
      clearEcho(1);

      DATA_REQUEST_INTERVAL = 20;

      if (readRawData()) {
        if (resultBuffer[0]) {
          connectionStatus = true;
          Serial.println(F("✅ Connection established with car"));
          Serial.println("");
          Serial.println("");
          return true;
        } else {
          Serial.println(F("❌ No Data Retrieved from Car"));
        }
      }
    }
  } else {
    DATA_REQUEST_INTERVAL = 20;
  }
  return false;
}

// ----------------------------------------- Write Functions ---------------------------------------

void writeByte(uint8_t data) {
  // Serial.print(F("➡️ Sending Byte: "));
  // Serial.print(data, HEX);
  // Serial.println(F(""));

  K_Serial.write(data);
  delay(5);

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

// ----------------------------------------- Read Functions ---------------------------------------

int readByte() {
  unsigned long startMillis = millis();  // Start time for waiting the first byte

  while (millis() - startMillis < DATA_TIMEOUT) {  // Wait for data for the specified timeout
    if (K_Serial.available() > 0) {                // If data available
      uint8_t receivedData = K_Serial.read();
      delay(2);
      // Serial.print("✅ Received Data: ");
      // Serial.print(receivedData, HEX);
      // Serial.println("");

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

  while (millis() - startMillis < DATA_TIMEOUT) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

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
        }
      }
      Serial.println("\n✅ Data reception completed.");
      return bytesRead;
    }
  }
  Serial.println("❌ Timeout: Not Received Data.");
  return 0;
}

void clearEcho(int length) {
  int result = K_Serial.available();
  if (result > 0) {
    //Serial.print("🗑️ Cleared Echo Data: ");
    for (int i = 0; i < length; i++) {
      byte receivedByte = K_Serial.read();
      // Serial.print(receivedByte, HEX);
      // Serial.print(" ");
    }
    //Serial.println();
  } else {
    Serial.println("❌ Not Received Echo Data");
  }
}

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

  Serial.print("➡️ 5 Baud Init for Module 0x");
  Serial.print(data, HEX);
  Serial.print(": ");

  // Set txPin as output
  pinMode(K_line_TX, OUTPUT);

  for (int i = 0; i < 10; i++) {
    Serial.print(bits[i] ? "1" : "0");
    digitalWrite(K_line_TX, bits[i] ? HIGH : LOW);
    delay(200);
  }

  Serial.println("");
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
