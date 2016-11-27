#include <SPI.h>
#include "SdFat.h"
#include <PulsePosition.h>
// 32 KiB buffer.
const size_t BUF_DIM = 32768;

// 8 MiB file.
const uint32_t FILE_SIZE = 256UL*BUF_DIM;


PulsePositionInput RadioIn;
SdFatSdioEX sd;

File file;



void setup() {
pinMode(13,OUTPUT);  
RadioIn.begin(10);
}



void loop() {





}

