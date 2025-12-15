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

byte checksum8_XOR(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum ^= data[i];  // XOR operation
  }
  return checksum;
}

byte checksum8_Modulo256(const byte data[], int length) {
  unsigned int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += data[i];
  }
  return (byte)(sum % 256);  // or (byte)sum; because byte automatically wraps around at 256
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


bool compareData(const uint8_t* dataArray, uint8_t length) {
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


uint16_t To16Bit(uint8_t msb, uint8_t lsb) {
  return ((uint16_t)msb << 8) | lsb;
}

int8_t ToSigned8(uint8_t value) {
  return (int8_t)value;  // 0..255 -> -128..127
}

int16_t ToSigned16(uint8_t msb, uint8_t lsb) {
  return (int16_t)((msb << 8) | lsb);
}

String extractAscii(const uint8_t* buffer, int length) {
  // header = first 3 byte (usually: length, blockCounter, command)
  // terminator = last byte (0x03)
  // ASCII data = from 3rd byte to one before the last byte

  String s = "";
  for (int i = 3; i < length - 1; i++) {
    uint8_t c = buffer[i];
    if (c >= 0x20 && c <= 0x7E) {  //  ASCII filtering
      s += (char)c;
    }
  }
  return s;
}



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
    case 0x20: return ToSigned8(B);                                                //N/A                    [N/A]
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
    case 0x3A: return ToSigned8(B) * 1.023;                                        //Misfires               [/s]
    case 0x3B: return To16Bit(A, B) * 1 / 32768;                                   //Segment correction     [N/A]
    case 0x3C: return To16Bit(A, B) * 1 / 100;                                     //Time resolution        [s]
    case 0x3D: return (B - 128) / A;                                               //Difference             [N/A]
    case 0x3E: return B * A * 256 / 1000;                                          //Time                   [s]
    case 0x40: return B + A;                                                       //Resistance             [Ohm]
    case 0x41: return (B - 127) * A * 0.01;                                        //Distance               [mm]
    case 0x42: return B * A / 512;                                                 //Voltage                [V]
    case 0x43: return ToSigned16(A, B) * 2.5;                                      //Steering angle         [deg]
    case 0x44: return ToSigned16(A, B) * 0.1358;                                   //Turn rate              [deg/s]
    case 0x45: return ToSigned16(A, B) * 0.3255;                                   //Pressure               [bar]
    case 0x46: return ToSigned16(A, B) * 0.192;                                    //Lat. acceleration      [m/s^2]
    case 0x47: return B * A;                                                       //Distance               [cm]
    case 0x48: return (A * 255 + B * (211 - A)) / 4080;                            //Voltage                [V]
    case 0x49: return A * B * 0.01;                                                //Resistance             [Ohm]
    case 0x4A: return 0.1 * A * B;                                                 //Time                   [months]
    case 0x4B: return A * 256 + B;                                                 //Fault code             [N/A]
    case 0x4C: return A * 255 + B;                                                 //Resistance             [kOhm]
    case 0x4D: return (255 * A + B * 60) / 4080;                                   //Voltage                [V]
    case 0x4E: return ToSigned8(B) * 1.819;                                        //Misfires               [/s]
    case 0x4F: return B;                                                           //Channel number         [N/A]
    case 0x50: return To16Bit(A, B) / 100;                                         //Resistance             [kOhm]
    case 0x51: return ToSigned16(A, B) * 0.04375;                                  //Steering angle         [deg]
    case 0x52: return ToSigned16(A, B) * 0.00981;                                  //Lat. acceleration      [m/s^2]
    case 0x53: return ToSigned16(A, B) * 0.01;                                     //Pressure               [bar]
    case 0x54: return ToSigned16(A, B) * 0.0973;                                   //Long. acceleration     [m/s^2]
    case 0x55: return ToSigned16(A, B) * 0.002865;                                 //Turn rate              [deg/s]
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
    case 0x63: return ToSigned16(A, B);                                            //N/A                    [N/A]
    case 0x64: return A * B * 0.1;                                                 //Pressure               [bar]
    case 0x65: return B * A * 0.001;                                               //Fuel level factor      [l/mm]
    case 0x66: return B * A * 0.1;                                                 //Fuel level             [mm]
    case 0x67: return A + B * 0.05;                                                //Voltage                [V]
    case 0x68: return (B - 128) * A * 0.2;                                         //Volume                 [ml]
    case 0x69: return (B - 128) * A * 0.01;                                        //Distance               [m]
    case 0x6A: return (B - 128) * A * 0.1;                                         //Speed                  [km/h]
    case 0x6B: return ToSigned16(A, B);                                            //Hex bytes              [hex(A)hex(B)]
    case 0x6F: return 0x6F0000 | (A << 8) | B;                                     //Mileage                [km]
    case 0x70: return (B - 128) * A * 0.001;                                       //Angle                  [deg]
    case 0x71: return (B - 128) * A * 0.01;                                        //N/A                    [N/A]
    case 0x72: return (B - 128) * A;                                               //Altitude               [m]
    case 0x73: return ToSigned16(A, B);                                            //Power                  [W]
    case 0x74: return ToSigned16(A, B);                                            //RPM                    [/min]
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
    case 0x9D: return ToSigned16(A, B);                                            //Distance               [cm]
    case 0x9E: return To16Bit(A, B) * 0.01;                                        //Speed                  [km/h]
    case 0x9F: return ((B - 127) * 256 + A) * 0.1;                                 //Temperature            [degC]
    case 0xA1: return To16Bit(A, B);                                               //Binary bytes           [bin(A)bin(B)]
    case 0xA2: return A * B * 0.448;                                               //Angle                  [deg]
    case 0xA3: return B / 100.0 + A;                                               //Time                   [hh:mm]
    case 0xA4: return ((B <= 100) ? B : ((B > 100 && B <= 200) ? (B - 100) : B));  //Percentage             [%]
    case 0xA5: return ((B * 256) + A - 32768);                                     //Current                [mA]
    case 0xA6: return ToSigned16(A, B) * 2.5;                                      //Turn rate              [deg/s]
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
