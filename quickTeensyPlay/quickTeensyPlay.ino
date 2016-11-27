#include <SPI.h>
#include "SdFat.h"
SdFatSdioEX sd;
//SdFat sd;

// Log file.
SdFile file;


// 32 KiB buffer.
const size_t BUF_DIM = 32768; //2^15
uint8_t buf[BUF_DIM];  // uint8_t is one byte?? i think.


void setup() {
pinMode(13,OUTPUT);  


//  do {
//  delay(10);
//  } while (Serial.available() && Serial.read());

  while (!Serial) {
  }



  if (!sd.begin()) {
     sd.initErrorHalt("SdFatSdioEX begin() failed");
  }
  
  if (!file.open("TeensyDemo.bin", O_RDWR )) {
    sd.errorHalt("open failed");
  }

}
size_t nb = 512;

unsigned long lasttimer1=0;
unsigned long timer1Intervial=100000;
unsigned long lasttimerSonar=0;
unsigned long timerSonarIntervial=100000;
unsigned long lasttimerFlash=0;
unsigned long timerFlashIntervial=100000;
bool flashstate=0;
int sensorValue =0;
void loop() {
  unsigned long timer=micros();

  if (timer>lasttimerFlash+timerFlashIntervial){
    lasttimerFlash=micros();
    
    
    if(flashstate==1){
      flashstate=0;
      timerFlashIntervial=255-sensorValue;
      if (timerFlashIntervial>0){
        digitalWrite(13,1);
      }
    }else{
      flashstate=1;
      timerFlashIntervial=sensorValue;
      digitalWrite(13,0);
    }
  
  }



  //    for (uint8_t i )
//
  //    if (nb != file.write(buf, nb)) {
  //      errorHalt("write failed");
  //    }
  if (timer>lasttimer1+timer1Intervial){
    lasttimer1=micros();
    sensorValue=0;
    for (int iii = 0; iii < 20; iii++){
      sensorValue += analogRead(A0)/4.;
    }
    sensorValue=sensorValue/20;
    Serial.println(sensorValue);
  }
  if (timer>lasttimerSonar+timerSonarIntervial){
    lasttimerSonar=micros();
    int SonarValue=0;
    for (int iii = 0; iii < 20; iii++){
      SonarValue += analogRead(A1);
    }
    SonarValue=SonarValue/20;
    Serial.print("Sonar: ");
    Serial.println(SonarValue);
  }
  
}
