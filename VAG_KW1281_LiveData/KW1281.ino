uint8_t messageCount = 0;
String ecu_data_1 = "";
String ecu_data_2 = "";
String ecu_data_3 = "";
String ecu_data_4 = "";

void KW1281() {
  if (initOBD2()) {
    ledGreen();
    readECUInfo();
    while (true) {
      getPID(1);
      getPID(2);
      getPID(3);
      getPID(4);
      
      readDTC();
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
  delay(3);

  clearEcho(1);
}

void writeBlock(const uint8_t* dataArray, uint8_t length) {
  debugPrintln(F("➡️ Sending Full Data..."));
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
  debugPrintln();
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
    if (K_Serial.available() > 0) {                // If data available
      uint8_t receivedData = K_Serial.read();
      delay(1);
      // debugPrint(F("✅ Received Data: "));
      // debugPrintHex(receivedData);
      // debugPrintln();

      return receivedData;
    }
  }

  debugPrintln(F("❌ Timeout: Not Received Data."));  // If no data is received within 1 seconds
  return -1;
}

uint8_t readBlock() {
  debugPrintln(F("Reading Full Data..."));
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
        debugPrint(F("   Length: "));
        debugPrintHex(messageLength);
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
  debugPrintln(F("Reading Raw Data..."));
  unsigned long startMillis = millis();  // Start time for waiting the first byte
  int bytesRead = 0;

  // Wait for data for the specified timeout
  while (millis() - startMillis < DATA_TIMEOUT) {
    if (K_Serial.available() > 0) {           // If the first byte is received
      unsigned long lastByteTime = millis();  // Get the last received byte time
      memset(resultBuffer, 0, sizeof(resultBuffer));

      // Inner loop: Read all data
      debugPrint(F("✅ Received Data: "));
      while (millis() - lastByteTime < DATA_REQUEST_INTERVAL) {  // Wait up to 60ms for new data
        if (K_Serial.available() > 0) {                          // If new data is available, read it
          if (bytesRead >= sizeof(resultBuffer)) {
            debugPrintln(F("\nBuffer is full. Stopping data reception."));
            return bytesRead;
          }

          resultBuffer[bytesRead] = K_Serial.read();
          debugPrintHex(resultBuffer[bytesRead]);
          debugPrint(F(" "));
          bytesRead++;
          lastByteTime = millis();  // Reset timer

          // If buffer is full, stop reading and print message
        }
      }

      // If no new data is received within 60ms, exit the loop
      debugPrintln(F("\n✅ Data reception completed."));
      return bytesRead;
    }
  }

  // If no data is received within 1 seconds
  debugPrintln(F("❌ Timeout: Not Received Data."));
  return 0;
}

void clearEcho(int length) {
  int result = K_Serial.available();
  if (result > 0) {
    //debugPrint(F("🗑️ Cleared Echo Data: "));
    for (int i = 0; i < length; i++) {
      byte receivedByte = K_Serial.read();
      //debugPrintHex(receivedByte);
      //debugPrint(F(" "));
    }
    //debugPrintln();
  } else {
    debugPrintln(F("❌ Not Received Echo Data"));
  }
}



// ----------------------------------------- PID Functions ----------------------------------------

struct PIDFormula {
  uint8_t id;
  const char* name;
  const char* unit;
};

const PIDFormula pidTable[182] = {
  { 0x01, "RPM", "rpm" },
  { 0x02, "Load", "%" },
  { 0x03, "Throttle angle", "deg" },
  { 0x04, "Ignition angle", "deg" },
  { 0x05, "Temperature", "°C" },
  { 0x06, "Voltage", "V" },
  { 0x07, "Speed", "km/h" },
  { 0x08, "N/A", "N/A" },
  { 0x09, "Steering angle", "deg" },
  { 0x0A, "Text", "Warm/Cold" },
  { 0x0B, "Lambda factor", "N/A" },
  { 0x0C, "Resistance", "Ohm" },
  { 0x0D, "Distance", "mm" },
  { 0x0E, "Pressure", "bar" },
  { 0x0F, "Injection time", "ms" },
  { 0x10, "Switch positions", "bin(B&A)" },
  { 0x11, "2 ASCII letters", "char(A)char(B)" },
  { 0x12, "Absolute pressure", "mbar" },
  { 0x13, "Fuel level", "l" },
  { 0x14, "Lambda integrator", "%" },
  { 0x15, "Voltage", "V" },
  { 0x16, "Injection time", "ms" },
  { 0x17, "Duty cycle", "%" },
  { 0x18, "Current", "A" },
  { 0x19, "Mass flow", "g/s" },
  { 0x1A, "Temperature", "°C" },
  { 0x1B, "Ignition angle", "deg" },
  { 0x1C, "N/A", "N/A" },
  { 0x1D, "Map number", "Map1/Map2" },
  { 0x1E, "Knock correction depth", "deg" },
  { 0x1F, "N/A", "N/A" },
  { 0x20, "N/A", "N/A" },
  { 0x21, "Load", "%" },
  { 0x22, "Idle correction", "KW" },
  { 0x23, "Fuel rate", "l/h" },
  { 0x24, "Mileage", "km" },
  { 0x25, "Text from table", "text" },
  { 0x26, "Segment correction", "KW" },
  { 0x27, "Injection mass", "mg/h" },
  { 0x28, "Current", "A" },
  { 0x29, "Charge", "N/A" },
  { 0x2A, "Power", "kW" },
  { 0x2B, "Battery voltage", "V" },
  { 0x2C, "Time", "hh:mm" },
  { 0x2D, "Fuel rate", "l/km" },
  { 0x2E, "Segment correction", "KW" },
  { 0x2F, "Time correction", "ms" },
  { 0x30, "N/A", "N/A" },
  { 0x31, "Air mass per stroke", "mg/h" },
  { 0x32, "Pressure", "mbar" },
  { 0x33, "Injection mass", "mg/h" },
  { 0x34, "Torque", "Nm" },
  { 0x35, "Mass flow", "g/s" },
  { 0x36, "Count", "N/A" },
  { 0x37, "Time", "s" },
  { 0x38, "Code for WSC16=0", "N/A" },
  { 0x39, "Code for WSC16=1", "N/A" },
  { 0x3A, "Misfires", "/s" },
  { 0x3B, "Segment correction", "N/A" },
  { 0x3C, "Time resolution", "s" },
  { 0x3D, "Difference", "N/A" },
  { 0x3E, "Time", "s" },
  { 0x40, "Resistance", "Ohm" },
  { 0x41, "Distance", "mm" },
  { 0x42, "Voltage", "V" },
  { 0x43, "Steering angle", "deg" },
  { 0x44, "Turn rate", "deg/s" },
  { 0x45, "Pressure", "bar" },
  { 0x46, "Lat. acceleration", "m/s²" },
  { 0x47, "Distance", "cm" },
  { 0x48, "Voltage", "V" },
  { 0x49, "Resistance", "Ohm" },
  { 0x4A, "Time", "months" },
  { 0x4B, "Fault code", "N/A" },
  { 0x4C, "Resistance", "kOhm" },
  { 0x4D, "Voltage", "V" },
  { 0x4E, "Misfires", "/s" },
  { 0x4F, "Channel number", "N/A" },
  { 0x50, "Resistance", "kOhm" },
  { 0x51, "Steering angle", "deg" },
  { 0x52, "Lat. acceleration", "m/s²" },
  { 0x53, "Pressure", "bar" },
  { 0x54, "Long. acceleration", "m/s²" },
  { 0x55, "Turn rate", "deg/s" },
  { 0x56, "Current", "A" },
  { 0x57, "Turn rate", "deg/s" },
  { 0x58, "Resistance", "kOhm" },
  { 0x59, "On-time", "h" },
  { 0x5A, "Weight", "kg" },
  { 0x5B, "Steering angle", "deg" },
  { 0x5C, "Mileage", "km" },
  { 0x5D, "Torque", "Nm" },
  { 0x5E, "Torque", "Nm" },
  { 0x60, "Pressure", "mbar" },
  { 0x61, "Cat-temperature", "°C" },
  { 0x62, "Impulses", "/km" },
  { 0x63, "N/A", "N/A" },
  { 0x64, "Pressure", "bar" },
  { 0x65, "Fuel level factor", "l/mm" },
  { 0x66, "Fuel level", "mm" },
  { 0x67, "Voltage", "V" },
  { 0x68, "Volume", "ml" },
  { 0x69, "Distance", "m" },
  { 0x6A, "Speed", "km/h" },
  { 0x6B, "Hex bytes", "hex(A)hex(B)" },
  { 0x6F, "Mileage", "km" },
  { 0x70, "Angle", "deg" },
  { 0x71, "N/A", "N/A" },
  { 0x72, "Altitude", "m" },
  { 0x73, "Power", "W" },
  { 0x74, "RPM", "rpm" },
  { 0x75, "Temperature", "°C" },
  { 0x77, "Percentage", "%" },
  { 0x78, "Angle", "deg" },
  { 0x79, "N/A", "N/A" },
  { 0x7A, "N/A", "N/A" },
  { 0x7B, "Text from table", "text" },
  { 0x7C, "Current", "mA" },
  { 0x7D, "Attenuation", "dB" },
  { 0x7E, "N/A", "N/A" },
  { 0x80, "RPM", "rpm" },
  { 0x81, "Load", "%" },
  { 0x82, "Current", "A" },
  { 0x83, "Ignition angle", "deg" },
  { 0x84, "Throttle angle", "deg" },
  { 0x85, "Voltage", "V" },
  { 0x86, "Speed", "km/h" },
  { 0x87, "N/A", "N/A" },
  { 0x88, "Switch positions", "bin(B&A)" },
  { 0x89, "Injection time", "ms" },
  { 0x8A, "Knock sensor", "V" },
  { 0x8E, "2 ASCII letters", "char(A)char(B)" },
  { 0x8F, "Ignition angle", "deg" },
  { 0x90, "Consumption", "l/h" },
  { 0x91, "Consumption", "N/A" },
  { 0x92, "Lambda factor", "N/A" },
  { 0x94, "Slip RPM", "N/A" },
  { 0x95, "Temperature", "°C" },
  { 0x96, "Mass flow", "g/s" },
  { 0x97, "Idle correction", "KW" },
  { 0x98, "Air mass per stroke", "mg/h" },
  { 0x99, "Injection quantity", "mg/h" },
  { 0x9A, "Angle", "deg" },
  { 0x9B, "Angle", "deg" },
  { 0x9C, "Distance", "cm" },
  { 0x9D, "Distance", "cm" },
  { 0x9E, "Speed", "km/h" },
  { 0x9F, "Temperature", "°C" },
  { 0xA1, "Binary bytes", "bin(A)bin(B)" },
  { 0xA2, "Angle", "deg" },
  { 0xA3, "Time", "hh:mm" },
  { 0xA4, "Percentage", "%" },
  { 0xA5, "Current", "mA" },
  { 0xA6, "Turn rate", "deg/s" },
  { 0xA7, "Resistance", "mOhm" },
  { 0xA8, "Percentage", "%" },
  { 0xA9, "Nernst voltage", "mV" },
  { 0xAA, "Ammonia", "g" },
  { 0xAB, "Nitrogen oxide", "g" },
  { 0xAC, "Nitrogen oxide", "mg/km" },
  { 0xAD, "NOx mass flow", "mg/s" },
  { 0xAE, "Average DEF", "km" },
  { 0xAF, "DEF pressure", "bar" },
  { 0xB0, "NOx concentration", "ppm" },
  { 0xB1, "Torque", "Nm" },
  { 0xB2, "Turn rate", "deg/s" },
  { 0xB3, "Quantity", "N/A" },
  { 0xB4, "Mass flow", "kg/h" },
  { 0xB5, "Mass flow", "mg/s" }
};


float calculatePID(uint8_t id, uint8_t A, uint8_t B) {
  switch (id) {
    //0x00 - UNKNOWN                                                               //N/A                    [N/A]
    case 0x01: return B * A * 0.2;                                                 //RPM                    [/min]
    case 0x02: return B * A * 0.002;                                               //Load                   [%]
    case 0x03: return B * A * 0.002;                                               //Throttle angle         [deg]
    case 0x04: return (B - 127) * A * -0.01;                                       //Ignition angle         [deg]
    case 0x05: return (B - 100) * A * 0.1;                                         //Temperature            [degC]
    case 0x06: return B * A * 0.001;                                               //Voltage                [V]
    case 0x07: return B * A * 0.01;                                                //Speed                  [km/h]
    case 0x08: return B * A * 0.1;                                                 //N/A                    [N/A]
    case 0x09: return (B - 127) * A * 0.02;                                        //Steering angle         [deg]
    case 0x0A: return (B ? 1 : 0);                                                 //Text                   [Warm/Cold]
    case 0x0B: return 1 + ((B - 128) * A * 0.0001);                                //Lambda factor          [N/A]
    case 0x0C: return B * A * 0.001;                                               //Resistance             [Ohm]
    case 0x0D: return (B - 127) * A * 0.001;                                       //Distance               [mm]
    case 0x0E: return B * A * 0.005;                                               //Pressure               [bar]
    case 0x0F: return B * A * 0.01;                                                //Injection time         [ms]
    case 0x10: return B & A;                                                       //Switch positions       [bin(B&A)]
    case 0x11: return To16Bit(A, B);                                               //2 ASCII letters        [char(A)char(B)]
    case 0x12: return B * A * 0.04;                                                //Absolute pressure      [mbar]
    case 0x13: return B * A * 0.01;                                                //Fuel level             [l]
    case 0x14: return (B - 128) * A * 1 / 128;                                     //Lambda integrator      [%]
    case 0x15: return B * A * 0.001;                                               //Voltage                [V]
    case 0x16: return B * A * 0.001;                                               //Injection time         [ms]
    case 0x17: return B * A * 1 / 256;                                             //Duty cycle             [%]
    case 0x18: return B * A * 0.001;                                               //Current                [A]
    case 0x19: return (256 * B + A) * 1 / 180;                                     //Mass flow              [g/s]
    case 0x1A: return B - A;                                                       //Temperature            [degC]
    case 0x1B: return (B - 128) * A * 0.01;                                        //Ignition angle         [deg]
    case 0x1C: return B - A;                                                       //N/A                    [N/A]
    case 0x1D: return ((B < A) ? 1 : 2);                                           //Map number             [Map1/Map2]
    case 0x1E: return B * A * 1 / 12;                                              //Knock correction depth [deg]
    case 0x1F: return B * A * 1 / 2560;                                            //N/A                    [N/A]
    case 0x20: return ToSigned8(B);                                                 //N/A                    [N/A]
    case 0x21: return (B / A) * 100;                                               //Load                   [%]
    case 0x22: return (B - 128) * A * 0.01;                                        //Idle correction        [KW]
    case 0x23: return B * A * 0.01;                                                //Fuel rate              [l/h]
    case 0x24: return To16Bit(A, B) * 10;                                          //Mileage                [km]
    case 0x25: return To16Bit(A, B);                                               //Text from table        [text]
    case 0x26: return (B - 128) * A * 0.001;                                       //Segment correction     [KW]
    case 0x27: return A * B * 1 / 255;                                             //Injection mass         [mg/h]
    case 0x28: return ((A * 255 + B) - 4000) * 0.1;                                //Current                [A]
    case 0x29: return A * 255 + B;                                                 //Charge                 [N/A]
    case 0x2A: return ((A * 255 + B) - 4000) * 0.1;                                //Power                  [kW]
    case 0x2B: return (A * 255 + B) * 0.1;                                         //Battery voltage        [V]
    case 0x2C: return B / 100.0 + A;                                               //Time                   [hh:mm]
    case 0x2D: return B * A * 0.1;                                                 //Fuel rate              [l/km]
    case 0x2E: return ((A * 256) + B - 32768) * 0.00275;                           //Segment correction     [KW]
    case 0x2F: return (B - 128) * A;                                               //Time correction        [ms]
    case 0x30: return A * 255 + B;                                                 //N/A                    [N/A]
    case 0x31: return A * B * 0.025;                                               //Air mass per stroke    [mg/h]
    case 0x32: return (B - 128) * 100 / A;                                         //Pressure               [mbar]
    case 0x33: return (B - 128) * A / 255;                                         //Injection mass         [mg/h]
    case 0x34: return (B - 50) * A * 0.02;                                         //Torque                 [Nm]
    case 0x35: return ((B - 128) * 256 + A) / 180;                                 //Mass flow              [g/s]
    case 0x36: return A * 256 + B;                                                 //Count                  [N/A]
    case 0x37: return B * A * 0.005;                                               //Time                   [s]
    case 0x38: return (A << 8) & B;                                                //Code for WSC16=0       [N/A]
    case 0x39: return ((A << 8) & B) + 65536;                                      //Code for WSC16=1       [N/A]
    case 0x3A: return ToSigned8(B) * 1.023;                                         //Misfires               [/s]
    case 0x3B: return To16Bit(A, B) * 1 / 32768;                                   //Segment correction     [N/A]
    case 0x3C: return To16Bit(A, B) * 1 / 100;                                     //Time resolution        [s]
    case 0x3D: return (B - 128) / A;                                               //Difference             [N/A]
    case 0x3E: return B * A * 256 / 1000;                                          //Time                   [s]
    case 0x40: return B + A;                                                       //Resistance             [Ohm]
    case 0x41: return (B - 127) * A * 0.01;                                        //Distance               [mm]
    case 0x42: return B * A / 512;                                                 //Voltage                [V]
    case 0x43: return ToSigned16(A, B) * 2.5;                                        //Steering angle         [deg]
    case 0x44: return ToSigned16(A, B) * 0.1358;                                     //Turn rate              [deg/s]
    case 0x45: return ToSigned16(A, B) * 0.3255;                                     //Pressure               [bar]
    case 0x46: return ToSigned16(A, B) * 0.192;                                      //Lat. acceleration      [m/s^2]
    case 0x47: return B * A;                                                       //Distance               [cm]
    case 0x48: return (A * 255 + B * (211 - A)) / 4080;                            //Voltage                [V]
    case 0x49: return A * B * 0.01;                                                //Resistance             [Ohm]
    case 0x4A: return 0.1 * A * B;                                                 //Time                   [months]
    case 0x4B: return A * 256 + B;                                                 //Fault code             [N/A]
    case 0x4C: return A * 255 + B;                                                 //Resistance             [kOhm]
    case 0x4D: return (255 * A + B * 60) / 4080;                                   //Voltage                [V]
    case 0x4E: return ToSigned8(B) * 1.819;                                         //Misfires               [/s]
    case 0x4F: return B;                                                           //Channel number         [N/A]
    case 0x50: return To16Bit(A, B) / 100;                                         //Resistance             [kOhm]
    case 0x51: return ToSigned16(A, B) * 0.04375;                                    //Steering angle         [deg]
    case 0x52: return ToSigned16(A, B) * 0.00981;                                    //Lat. acceleration      [m/s^2]
    case 0x53: return ToSigned16(A, B) * 0.01;                                       //Pressure               [bar]
    case 0x54: return ToSigned16(A, B) * 0.0973;                                     //Long. acceleration     [m/s^2]
    case 0x55: return ToSigned16(A, B) * 0.002865;                                   //Turn rate              [deg/s]
    case 0x56: return B * A * 0.1;                                                 //Current                [A]
    case 0x57: return A * (B - 128) * 0.1;                                         //Turn rate              [deg/s]
    case 0x58: return B * A * 0.01;                                                //Resistance             [kOhm]
    case 0x59: return A * 256 + B;                                                 //On-time                [h]
    case 0x5A: return A * B * 0.1;                                                 //Weight                 [kg]
    case 0x5B: return A * (B - 128) * 0.1;                                         //Steering angle         [deg]
    case 0x5C: return A * B;                                                       //Mileage                [km]
    case 0x5D: return (B - 128) * A * 0.001;                                       //Torque                 [Nm]
    case 0x5E: return A * (B - 128) * 0.1;                                         //Torque                 [Nm]
    case 0x60: return A * B * 0.1;                                                 //Pressure               [mbar]
    case 0x61: return (B - A) * 5;                                                 //Cat-temperature        [degC]
    case 0x62: return A * B * 0.1;                                                 //Impulses               [/km]
    case 0x63: return ToSigned16(A, B);                                              //N/A                    [N/A]
    case 0x64: return A * B * 0.1;                                                 //Pressure               [bar]
    case 0x65: return B * A * 0.001;                                               //Fuel level factor      [l/mm]
    case 0x66: return B * A * 0.1;                                                 //Fuel level             [mm]
    case 0x67: return A + B * 0.05;                                                //Voltage                [V]
    case 0x68: return (B - 128) * A * 0.2;                                         //Volume                 [ml]
    case 0x69: return (B - 128) * A * 0.01;                                        //Distance               [m]
    case 0x6A: return (B - 128) * A * 0.1;                                         //Speed                  [km/h]
    case 0x6B: return ToSigned16(A, B);                                              //Hex bytes              [hex(A)hex(B)]
    case 0x6F: return 0x6F0000 | (A << 8) | B;                                     //Mileage                [km]
    case 0x70: return (B - 128) * A * 0.001;                                       //Angle                  [deg]
    case 0x71: return (B - 128) * A * 0.01;                                        //N/A                    [N/A]
    case 0x72: return (B - 128) * A;                                               //Altitude               [m]
    case 0x73: return ToSigned16(A, B);                                              //Power                  [W]
    case 0x74: return ToSigned16(A, B);                                              //RPM                    [/min]
    case 0x75: return (B - 64) * A * 0.01;                                         //Temperature            [degC]
    case 0x77: return B * A * 0.01;                                                //Percentage             [%]
    case 0x78: return B * A * 1.41;                                                //Angle                  [deg]
    case 0x79: return ((B * 256) + A) * 0.5;                                       //N/A                    [N/A]
    case 0x7A: return ((B * 256) + A - 32768) * 0.01;                              //N/A                    [N/A]
    case 0x7B: return To16Bit(A, B);                                               //Text from table        [text]
    case 0x7C: return B * A * 0.1;                                                 //Current                [mA]
    case 0x7D: return A - B;                                                       //Attenuation            [dB]
    case 0x7E: return A * B * 0.1;                                                 //N/A                    [N/A]
    case 0x80: return B * A;                                                       //RPM                    [/min]
    case 0x81: return B * A * 1 / 256;                                             //Load                   [%]
    case 0x82: return B * A * 1 / 2560;                                            //Current                [A]
    case 0x83: return (B * A * 0.5) - 30;                                          //Ignition angle         [deg]
    case 0x84: return B * A * 0.5;                                                 //Throttle angle         [deg]
    case 0x85: return B * A * 1 / 256;                                             //Voltage                [V]
    case 0x86: return B * A;                                                       //Speed                  [km/h]
    case 0x87: return B * A;                                                       //N/A                    [N/A]
    case 0x88: return B & A;                                                       //Switch positions       [bin(B&nWb)]
    case 0x89: return B * A * 0.01;                                                //Injection time         [ms]
    case 0x8A: return B * A * 0.001;                                               //Knock sensor           [V]
    case 0x8E: return To16Bit(A, B);                                               //2 ASCII letters        [char(A)char(B)]
    case 0x8F: return (B - 128) * A * 0.01;                                        //Ignition angle         [deg]
    case 0x90: return B * A * 0.01;                                                //Consumption            [l/h]
    case 0x91: return B * A * 0.01;                                                //Consumption            [N/A]
    case 0x92: return 1 + ((B - 128) * A * 0.0001);                                //Lambda factor          [N/A]
    case 0x94: return (B - 128) * A * 0.25;                                        //Slip RPM               [N/A]
    case 0x95: return (B - 100) * A * 0.1;                                         //Temperature            [degC]
    case 0x96: return (256 * B + A) * 1 / 180;                                     //Mass flow              [g/s]
    case 0x97: return (B - 128) * A * 0.01;                                        //Idle correction        [KW]
    case 0x98: return A * B * 0.025;                                               //Air mass per stroke    [mg/h]
    case 0x99: return (B - 128) * A / 255;                                         //Injection quantity     [mg/h]
    case 0x9A: return B * A;                                                       //Angle                  [deg]
    case 0x9B: return (A * B * 0.01) - 90;                                         //Angle                  [deg]
    case 0x9C: return To16Bit(A, B);                                               //Distance               [cm]
    case 0x9D: return ToSigned16(A, B);                                              //Distance               [cm]
    case 0x9E: return To16Bit(A, B) * 0.01;                                        //Speed                  [km/h]
    case 0x9F: return ((B - 127) * 256 + A) * 0.1;                                 //Temperature            [degC]
    case 0xA1: return To16Bit(A, B);                                               //Binary bytes           [bin(A)bin(B)]
    case 0xA2: return A * B * 0.448;                                               //Angle                  [deg]
    case 0xA3: return B / 100.0 + A;                                               //Time                   [hh:mm]
    case 0xA4: return ((B <= 100) ? B : ((B > 100 && B <= 200) ? (B - 100) : B));  //Percentage             [%]
    case 0xA5: return ((B * 256) + A - 32768);                                     //Current                [mA]
    case 0xA6: return ToSigned16(A, B) * 2.5;                                        //Turn rate              [deg/s]
    case 0xA7: return B * A * 0.001;                                               //Resistance             [mOhm]
    case 0xA8: return (256 * A + B) * 0.01;                                        //Percentage             [%]
    case 0xA9: return B * A + 200;                                                 //Nernst voltage         [mV]
    case 0xAA: return B * A * 1 / 2560;                                            //Ammonia                [g]
    case 0xAB: return B * A * 1 / 2560;                                            //Nitrogen oxide         [g]
    case 0xAC: return B * A;                                                       //Nitrogen oxide         [mg/km]
    case 0xAD: return B * A * 0.1;                                                 //NOx mass flow          [mg/s]
    case 0xAE: return B * A * 0.01;                                                //Average DEF            [km]
    case 0xAF: return B * A * 0.005;                                               //DEF pressure           [bar]
    case 0xB0: return B * A * 0.1;                                                 //NOx concentration      [ppm]
    case 0xB1: return ((A - 128) * 256 + B);                                       //Torque                 [Nm]
    case 0xB2: return ((A - 128) * 256 + B) * ((double)6 / 51);                    //Turn rate              [deg/s]
    case 0xB3: return (A * 256 + B) * 10;                                          //Quantity               [N/A]
    case 0xB4: return B * A * 1 / 256;                                             //Mass flow              [kg/h]
    case 0xB5: return B * A * 0.001;                                               //Mass flow              [mg/s]
    default: return 0;                                                             //Other or undefined
  }
}

//0x3F - TEXT (converted to 5F by readGroup())                                      //ASCII long text        [text]
//0x5F - TEXT                                                                       //ASCII long text        [text]
//0x6C - UNKNOWN                                                                    //Environment            [N/A]
//0x6D - UNKNOWN                                                                    //N/A                    [N/A]
//0x6E - UNKNOWN                                                                    //Workshop               [N/A]
//0x76 - TEXT                                                                       //Hex bytes              [hex]
//case 0x7F: return                                                                 //Date                   [yyyy:mm:dd]
//     200000 + (B & 0x7F) * 100
//     + (((A & 0x07) << 1) | ((B & 0x80) >> 7))
//     + ((A & 0xF8) >> 3) * 0.01;
//0x8B - value mapped through table provided by module                              //RPM                    [/min]
//0x8C - value mapped through table provided by module                              //Temperature            [degC]
//0x8D - ASCII string from list provided by module                                  //Text from table        [text]
//0x93 - value mapped through table provided by module                              //Inclination            [%]
//0xA0 - variable units                                                             //N/A                    [variable]

void getPID(uint8_t group) {
  uint8_t pidBytes[3] = { 0x29, 0x00, 0x03 };
  pidBytes[1] = group;
  writeBlock(pidBytes, 3);
  int length = readBlock();

  if (resultBuffer[2] = 0xE7) {
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
  }
}

void readDTC() {
  writeBlock(readDTCs, 2);
  int length = readBlock();

  if (resultBuffer[2] == 0xFC) {
    int dtcStart = 3;
    int dtcBytesLength = length - 4;
    int dtcCount = dtcBytesLength / 3;

    for (int i = 0; i < dtcCount; i++) {
      int index = dtcStart + i * 3;

      uint8_t high = resultBuffer[index];
      uint8_t low = resultBuffer[index + 1];
      uint8_t detail = resultBuffer[index + 2];

      if (high == 0x00 && low == 0x00 && detail == 0x00) continue;  // 0x00 0x00 0x00 empty DTC

      uint16_t dtcCode = ((uint16_t)high << 8) | low;

      char dtcString[6];
      sprintf(dtcString, "%05u", dtcCode);

      Serial.print("DTC: ");
      Serial.print(dtcString);

      Serial.print("  Detail: 0x");
      Serial.println(detail, HEX);
    }
  }
}


  }
}

void readECUInfo() {
  int byteLength = 0;

  byteLength = readBlock();
  ecu_data_1 = extractAscii(resultBuffer, byteLength);

  writeBlock(ACKNOWLEDGE, 2);
  byteLength = readBlock();
  ecu_data_2 = extractAscii(resultBuffer, byteLength);

  writeBlock(ACKNOWLEDGE, 2);
  byteLength = readBlock();
  ecu_data_3 = extractAscii(resultBuffer, byteLength);

  writeBlock(ACKNOWLEDGE, 2);
  byteLength = readBlock();
  ecu_data_4 = extractAscii(resultBuffer, byteLength);

  Serial.println("ECU Data 1: " + ecu_data_1);
  Serial.println("ECU Data 2: " + ecu_data_2);
  Serial.println("ECU Data 3: " + ecu_data_3);
  Serial.println("ECU Data 4: " + ecu_data_4);
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

  return data;
}
