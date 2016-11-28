#include <SPI.h>
#include "SdFat.h"
#include <PulsePosition.h>
// 32 KiB buffer.
const size_t BUF_DIM = 32768;

// 8 MiB file.
const uint32_t FILE_SIZE = 256UL*BUF_DIM;

const uint32_t PPMPin = 10;
const uint32_t boardLEDPin = 13;

const uint32_t ValveAPin = 13;
const uint32_t ValveBPin = 13;
const uint32_t ValveCPin = 13;
const uint32_t ValveDPin = 13;
const uint32_t ValveEPin = 13;




PulsePositionInput RadioIn;
SdFatSdioEX sd;

File file;



void setup() {
pinMode(boardLEDPin,OUTPUT);  
RadioIn.begin(PPMPin);

pinMode(ValveAPin,OUTPUT); 
pinMode(ValveBPin,OUTPUT); 
pinMode(ValveCPin,OUTPUT); 
pinMode(ValveDPin,OUTPUT); 
pinMode(ValveEPin,OUTPUT); 

}



void loop() {





}

