//----------------------------------ECU Requests---------------------------
uint8_t ECU_Data[2] = { 0x00, 0x03 };
uint8_t ACKNOWLEDGE[2] = { 0x09, 0x03 };

uint8_t LiveData[2] = { 0x12, 0x03 };  //0D 13 F4 00 49 00 00 5F CE E1 6C 64 FF 03
uint8_t LiveData1[3] = { 0x29, 0x01, 0x03 };
uint8_t LiveData2[3] = { 0x29, 0x02, 0x03 };
uint8_t LiveData3[3] = { 0x29, 0x03, 0x03 };
uint8_t LiveData4[3] = { 0x29, 0x04, 0x03 };
uint8_t LiveData5[3] = { 0x29, 0x05, 0x03 };
uint8_t LiveData6[3] = { 0x29, 0x06, 0x03 };
uint8_t LiveData7[3] = { 0x29, 0x07, 0x03 };
uint8_t LiveData8[3] = { 0x29, 0x08, 0x03 };
uint8_t LiveData9[3] = { 0x29, 0x09, 0x03 };
uint8_t LiveData10[3] = { 0x29, 0x0A, 0x03 };
uint8_t LiveData11[3] = { 0x29, 0x0B, 0x03 };
uint8_t LiveData12[3] = { 0x29, 0x0C, 0x03 };
uint8_t LiveData13[3] = { 0x29, 0x0D, 0x03 };
uint8_t LiveData14[3] = { 0x29, 0x0E, 0x03 };
uint8_t LiveData15[3] = { 0x29, 0x0F, 0x03 };
uint8_t LiveData16[3] = { 0x29, 0x10, 0x03 };
uint8_t LiveData17[3] = { 0x29, 0x11, 0x03 };
uint8_t LiveData18[3] = { 0x29, 0x12, 0x03 };
uint8_t LiveData19[3] = { 0x29, 0x13, 0x03 };

uint8_t readDTCs[2] = { 0x07, 0x03 };
uint8_t clearDTCs[2] = { 0x05, 0x03 };

uint8_t adjustment1[3] = { 0x28, 0x03, 0x03 };
uint8_t adjustment2[3] = { 0x28, 0x04, 0x03 };
uint8_t adjustment3[3] = { 0x28, 0x0B, 0x03 };

uint8_t controlParts[3] = { 0x04, 0x00, 0x03 };

uint8_t readRAM[2] = { 0x01, 0x03 };
uint8_t readROM[5] = { 0x03, 0xA9, 0xA9, 0xA9, 0x03 };
uint8_t readEEPROM[2] = { 0x19, 0x03 };

uint8_t endCommunication[2] = { 0x06, 0x03 };

//---------------------------------ECU Example Responses (AUDI A6 2.5L)----------------------------
uint8_t initKW1281[3] = { 0x55, 0x01, 0x8A };
uint8_t ACKNOWLEDGE_Response[2] = { 0x09, 0x03 };

// ECU Data
uint8_t ECU_Data1_Response[26] = { 0xF6, 0x34, 0x5A, 0x37, 0x39, 0x30, 0x37, 0x34, 0x30, 0x31, 0x42, 0x20, 0x20, 0x32, 0x2E, 0x35, 0x6C, 0x2F, 0x34, 0x56, 0x54, 0x45, 0x44, 0x43, 0x20, 0x03 };  // 4Z7907401B  2.5l/4VTEDC (ASCII)
uint8_t ECU_Data2_Response[8] = { 0x07, 0x03, 0xF6, 0x47, 0x30, 0x30, 0x30, 0x03 };                                                                                                               // G000 (ASCII)
uint8_t ECU_Data3_Response[12] = { 0x0B, 0x05, 0xF6, 0x53, 0x47, 0x20, 0x44, 0x30, 0x39, 0x20, 0x20, 0x03 };                                                                                      // SG D09 (ASCII)
uint8_t ECU_Data4_Response[9] = { 0x08, 0x07, 0xF6, 0x00, 0x07, 0xEE, 0x09, 0x15, 0x03 };

// Live Data (Length = 0F)
uint8_t LiveData1_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x27, 0x50, 0x00, 0x22, 0x3F, 0x80, 0x05, 0x0B, 0x52, 0x03 };   // Engine Speed, Injection amount, Quantity control valve, Coolant temp
uint8_t LiveData2_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x21, 0xFF, 0x00, 0x10, 0x5B, 0x00, 0x05, 0x0B, 0x52, 0x03 };   // Engine Speed, Accelerator pedal position, Operating state, Coolant temp
uint8_t LiveData3_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x31, 0x9D, 0x91, 0x31, 0x9D, 0xFF, 0x17, 0x65, 0xE0, 0x03 };   // Engine Speed, Air mass meter (calculated value), Air mass meter (actual value), EGR valve mark/space ratio
uint8_t LiveData4_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x1B, 0x14, 0xA8, 0x1B, 0x14, 0x80, 0x17, 0x65, 0x00, 0x03 };   // Engine Speed, Injection timing ajustment (calculated value), Injection timing ajustment (actual value), Valve, injection start mark/space ration
uint8_t LiveData5_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x27, 0x50, 0x00, 0x1B, 0x14, 0x80, 0x05, 0x0B, 0x52, 0x03 };   // Engine Speed,
uint8_t LiveData6_Response[14] = { 0xE7, 0x07, 0x76, 0x04, 0x10, 0x49, 0x49, 0x10, 0x3F, 0x00, 0x08, 0x0A, 0xFF, 0x03 };   //
uint8_t LiveData7_Response[14] = { 0xE7, 0x05, 0x09, 0x64, 0x05, 0x0B, 0x40, 0x05, 0x0B, 0x9B, 0x05, 0x0B, 0x52, 0x03 };   // Fuel Temp, Oil Temp, Intake Air Temp, Coolant Temp
uint8_t LiveData8_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x27, 0x50, 0x00, 0x27, 0x50, 0x00, 0x27, 0x50, 0x53, 0x03 };   // Engine Speed, Injection amount (desired), Injection amount engine load limiting, Injection amount air mass meter limiting
uint8_t LiveData9_Response[14] = { 0xE7, 0x27, 0x50, 0x00, 0x27, 0x63, 0x00, 0x27, 0x50, 0x00, 0x27, 0x63, 0xFF, 0x03 };   //
uint8_t LiveData10_Response[14] = { 0xE7, 0x31, 0x9D, 0xFF, 0x12, 0xFF, 0x60, 0x12, 0xFF, 0x60, 0x21, 0xFF, 0x00, 0x03 };  // Air mass meter, Atmospheric pressure, Load pressure, Accelerator pedal position
uint8_t LiveData11_Response[14] = { 0xE7, 0x01, 0x74, 0x00, 0x12, 0xFF, 0x44, 0x12, 0xFF, 0x60, 0x17, 0x65, 0x0C, 0x03 };  // Engine Speed, Load pressure (calculated value), Load pressure (actual value), Charging pressure valve mark/space ratio

// DTCs
uint8_t readDTC_Response[20] = { 0xFC, 0x05, 0x26, 0x25, 0x05, 0x26, 0x31, 0x03, 0x09, 0x1B, 0x02, 0x1E, 0x1E, 0x05, 0xA1, 0x1E, 0x02, 0x0B, 0x1E, 0x03 };

//---------------------------------ECU Example Responses (VW Golf 3 1.6L)----------------------------

// ECU Data
// uint8_t ECU_Data1_Response[14] = { 0xF6, 0x30, 0x33, 0x32, 0x39, 0x30, 0x36, 0x30, 0x32, 0x36, 0x41, 0x20, 0x20, 0x03 };  //032906026A
// uint8_t ECU_Data2_Response[14] = { 0xF6, 0x31, 0x2C, 0x36, 0x6C, 0x20, 0x52, 0x34, 0x20, 0x4D, 0x4F, 0x4E, 0x4F, 0x03 };  //1,6l R4 MONO
// uint8_t ECU_Data3_Response[13] = { 0xF6, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x44, 0x30, 0x31, 0x03 };        //D01
// uint8_t ECU_Data4_Response[9] = { 0xF6, 0x50, 0x4D, 0x43, 0x20, 0x30, 0x31, 0x00, 0x03 };                                 //PMC 01


// // Live Data (Length = 0F)
// uint8_t LiveData1_Response[14] = { 0xE7, 0x01, 0x7D, 0x00, 0x05, 0x09, 0x76, 0x0B, 0x27, 0x80, 0x10, 0xFF, 0x42, 0x03 };
// uint8_t LiveData2_Response[14] = { 0xE7, 0x01, 0x7D, 0x00, 0x0F, 0x03, 0x00, 0x06, 0x3F, 0xC3, 0x05, 0x09, 0x7A, 0x03 };
// uint8_t LiveData3_Response[14] = { 0xE7, 0x01, 0x7D, 0x00, 0x11, 0x20, 0x20, 0x03, 0xA7, 0x1E, 0x04, 0x64, 0x79, 0x03 };
// uint8_t LiveData4_Response[14] = { 0xE7, 0x01, 0x7D, 0x00, 0x11, 0x20, 0x20, 0x11, 0x20, 0x30, 0x10, 0xFF, 0x03, 0x03 };
// uint8_t LiveData5_Response[14] = { 0xE7, 0x01, 0x7D, 0x00, 0x02, 0xFF, 0x00, 0x0B, 0x27, 0x80, 0x14, 0x2B, 0x83, 0x03 };

// // DTCs
// uint8_t readDTC_Response[5] = { 0xFC, 0x02, 0x03, 0x03, 0x03 };
//-----------------------------------------------------------------------------------------------------

// 0x04 - Control Command
// 0x05 - Clear DTCs Command
// 0x06 - END Command
// 0x07 - Get DTCs Command
// 0x09 - ACK Command
// 0x28 - Adjustments Command
// 0x29 - Group Read Data Command
// 0x2B - Activate-Deactivate Command
// 0xE7 - Group Read Data Command Response
// 0xF5 - Control Command Response
// 0xF6 - ASCII Data Response
// 0xFC - Get DTCs Command Response

static const uint8_t KWP_ACKNOWLEDGE = 0x09;  // the module has no more data to send
static const uint8_t KWP_REFUSE = 0x0A;       // the module can not fulfill a request
static const uint8_t KWP_DISCONNECT = 0x06;   // the tester wants to disconnect from the module

static const uint8_t KWP_REQUEST_EXTRA_ID = 0x00;         // response: KWP_RECEIVE_ID_DATA (data available) / KWP_ACKNOWLEDGE (no data available)
static const uint8_t KWP_REQUEST_LOGIN = 0x2B;            // response: KWP_ACKNOWLEDGE (login successful) / KWP_REFUSE (login not successful)
static const uint8_t KWP_REQUEST_RECODE = 0x10;           // response: KWP_REQUEST_EXTRA_ID...
static const uint8_t KWP_REQUEST_FAULT_CODES = 0x07;      // response: KWP_RECEIVE_FAULT_CODES (ok) / KWP_REFUSE (fault codes not supported)
static const uint8_t KWP_REQUEST_CLEAR_FAULTS = 0x05;     // response: KWP_ACKNOWLEDGE (ok) / KWP_REFUSE (clearing fault codes not supported)
static const uint8_t KWP_REQUEST_ADAPTATION = 0x21;       // response: KWP_RECEIVE_ADAPTATION (ok) / KWP_REFUSE (invalid channel)
static const uint8_t KWP_REQUEST_ADAPTATION_TEST = 0x22;  // response: KWP_RECEIVE_ADAPTATION (ok) / KWP_REFUSE (invalid channel)
static const uint8_t KWP_REQUEST_ADAPTATION_SAVE = 0x2A;  // response: KWP_RECEIVE_ADAPTATION (ok) / KWP_REFUSE (invalid channel or value)
static const uint8_t KWP_REQUEST_GROUP_READING = 0x29;    // response: KWP_RECEIVE_GROUP_READING (ok) / KWP_ACKNOWLEDGE (empty group) / KWP_REFUSE (invalid group)
static const uint8_t KWP_REQUEST_GROUP_READING_0 = 0x12;  // response: KWP_RECEIVE_GROUP_READING (ok) / KWP_ACKNOWLEDGE (empty group) / KWP_REFUSE (invalid group)
static const uint8_t KWP_REQUEST_READ_RAM = 0x01;         // not implemented
static const uint8_t KWP_REQUEST_READ_ROM = 0x03;         // response: KWP_RECEIVE_ROM (ok) / KWP_REFUSE (reading ROM not supported or invalid parameters)
static const uint8_t KWP_REQUEST_READ_EEPROM = 0x19;      // not implemented
static const uint8_t KWP_REQUEST_OUTPUT_TEST = 0x04;      // response: KWP_RECEIVE_OUTPUT_TEST (ok) / KWP_REFUSE (output tests not supported)
static const uint8_t KWP_REQUEST_BASIC_SETTING = 0x28;    // response: KWP_RECEIVE_BASIC_SETTING (ok) / KWP_ACKNOWLEDGE (empty group) / KWP_REFUSE (invalid channel or not supported)
static const uint8_t KWP_REQUEST_BASIC_SETTING_0 = 0x11;  // response: KWP_RECEIVE_BASIC_SETTING (ok) / KWP_ACKNOWLEDGE (empty group) / KWP_REFUSE (invalid channel or not supported)

static const uint8_t KWP_RECEIVE_ID_DATA = 0xF6;        // request: connect/KWP_REQUEST_EXTRA_ID/KWP_REQUEST_RECODE
static const uint8_t KWP_RECEIVE_FAULT_CODES = 0xFC;    // request: KWP_REQUEST_FAULT_CODES
static const uint8_t KWP_RECEIVE_ADAPTATION = 0xE6;     // request: KWP_REQUEST_ADAPTATION/KWP_REQUEST_ADAPTATION_TEST/KWP_REQUEST_ADAPTATION_SAVE
static const uint8_t KWP_RECEIVE_GROUP_HEADER = 0x02;   // request: KWP_REQUEST_GROUP_READING
static const uint8_t KWP_RECEIVE_GROUP_READING = 0xE7;  // request: KWP_REQUEST_GROUP_READING
static const uint8_t KWP_RECEIVE_ROM = 0xFD;            // request: KWP_REQUEST_READ_ROM
static const uint8_t KWP_RECEIVE_OUTPUT_TEST = 0xF5;    // request: KWP_REQUEST_OUTPUT_TEST
static const uint8_t KWP_RECEIVE_BASIC_SETTING = 0xF4;  // request: KWP_REQUEST_BASIC_SETTING

//--------------------------------------------------------------------- Adjustment ---------------------------------------------------------------------
//EGR System
// 04 B0 28 03 03
// 0F CD E7 01 74 00 25 00 1A 31 9D FF 17 65 FF 03

//Injection Timing Ajustment
// 04 0C 28 04 03
// 0F 0D E7 01 74 00 25 00 86 1B 14 80 17 65 00 03

//Boost pressure regulation
// 04 50 28 0B 03
// 0F 51 E7 01 74 00 25 00 88 12 FF 60 17 65 00 03

//--------------------------------------------------------------------- Output Test ---------------------------------------------------------------------
//Glow pin relay Coil
// 04 C4 04 00 03
// 05 C5 F5 04 F2 03

//Glow time Lamp
// 04 12 04 00 03
// 05 13 F5 02 72 03

//Cooling Fan, after-cooling
// 04 5E 04 00 03
// 05 5F F5 06 96 03

//Power stage, low heat
// 04 D2 04 00 03
// 05 D3 F5 04 A9 03

//Power stage, high heat
// 04 C2 04 00 03
// 05 C3 F5 04 AA 03

// END
// 04 76 04 00 03
// 04 78 04 00 03

//--------------------------------------------------------------------- Programing ---------------------------------------------------------------------
// 08 78 2B 2C C7 00 00 BA 03       // Activate Cruise Control System
// 08 38 2B 3F 27 00 00 BA 03       // DeActivate Cruise Control System

//Code ECU (with PIN = 1234)
// 07 A0 10 09 A4 00 BA 03 // PIN = 1234    --  07 A0 10 08 AE 00 BA 03 // PIN = 1111
// 1B A1 F6 34 5A 37 39 30 37 34 30 31 42 20 20 32 2E 35 6C 2F 34 56 54 45 44 43 20 03
// 07 A3 F6 30 30 30 30 03
// 0B A5 F6 53 47 20 44 30 39 20 20 03
// 08 A7 F6 00 07 EE 09 15 03
