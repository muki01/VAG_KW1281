uint8_t initKW1281[3] = { 0x55, 0x01, 0x8A };
uint8_t ACT[2] = { 0x09, 0x03 };
uint8_t ACT_Response[2] = { 0x09, 0x03 };
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
uint8_t ECU_Data[2] = { 0x00, 0x03 };


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

// --------------------------------------------------------------------- After Connection Established -------------------------------------------------
// 1B 01 F6 34 5A 37 39 30 37 34 30 31 42 20 20 32 2E 35 6C 2F 34 56 54 45 44 43 20 03          // 4Z7907401B  2.5l/4VTEDC (ASCII)
// 07 03 F6 47 30 30 30 03                                                                      // G000 (ASCII)
// 0B 05 F6 53 47 20 44 30 39 20 20 03                                                          // SG D09 (ASCII)
// 08 07 F6 00 07 EE 09 15 03                                                                   //

//--------------------------------------------------------------------- Live Data ---------------------------------------------------------------------
// 04 40 29 01 03
// 0F 2B E7 01 74 00 27 50 00 22 3F 80 05 0B 52 03
// 0F 2D E7 01 74 00 21 FF 00 10 5B 00 05 0B 52 03
// 0F 2F E7 01 74 00 31 9D 91 31 9D FF 17 65 E0 03
// 0F 1D E7 01 74 00 1B 14 A8 1B 14 80 17 65 00 03
// 0F 1F E7 01 74 00 27 50 00 1B 14 80 05 0B 52 03
// 0F 33 E7 07 76 04 10 49 49 10 3F 00 08 0A FF 03
// 0F 35 E7 05 09 64 05 0B 40 05 0B 9B 05 0B 52 03
// 0F 37 E7 01 74 00 27 50 00 27 50 00 27 50 53 03
// 0F 39 E7 27 50 00 27 63 00 27 50 00 27 63 FF 03
// 0F 3B E7 31 9D FF 12 FF 60 12 FF 60 21 FF 00 03
// 0F 3D E7 01 74 00 12 FF 44 12 FF 60 17 65 0C 03


//--------------------------------------------------------------------- DTCs ---------------------------------------------------------------------
//Read DTCs
// 03 40 07 03
// 15 69 FC 05 26 25 05 26 31 03 09 1B 02 1E 1E 05 A1 1E 02 0B 1E 03

//Clear DTCs
// 03 2C 05 03
// 15 69 FC 05 26 25 05 26 31 03 09 1B 02 1E 1E 05 A1 1E 02 0B 1E 03

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

//--------------------------------------------------------------------- Tic Tic ---------------------------------------------------------------------
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




// 03 04 06 03                  // END Communication