#include <FastLED.h>
#include "Codes.h"

#define K_Serial Serial1
#define K_line_RX 10
#define K_line_TX 11
int WRITE_DELAY = 5;             // Delay between each byte of the transmitted data (5ms - 20ms)
int DATA_REQUEST_INTERVAL = 50;  // Time to wait before sending a new request after receiving a response (55ms - 5000ms)
int DATA_TIMEOUT = 1000;         // Time to wait before sending a new request after receiving a response (500ms - 5000ms)
#define DEBUG_Serial

#ifdef DEBUG_Serial
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#define debugPrintHex(x) printHex(x);
#else
#define debugPrint(x) ((void)0)
#define debugPrintln(x) ((void)0)
#define debugPrintHex(x) ((void)0)
#endif

#define LED_PIN 21
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

int MODE = 1;
bool connectionStatus = false;

byte resultBuffer[620];

int zeroCount = 0;
void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  ledRed();

  Serial.begin(250000);
  setSerial(true, 9600);
}

void loop() {
  if (MODE == 0) {
    //read5baud();
    readRawData();
  } else if (MODE == 1) {
    KW1281();
  } else if (MODE == 2) {
    KW1281_Simulator();
  }
}