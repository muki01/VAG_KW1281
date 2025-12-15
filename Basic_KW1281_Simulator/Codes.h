//---------------------------------ECU Example Responses (AUDI A6 2.5L)----------------------------
uint8_t initKW1281_Response[3] = { 0x55, 0x01, 0x8A };
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
