#include <SPI.h>
#include "SdFat.h"
#include <PulsePosition.h>
// 32 KiB buffer.
const size_t BUF_DIM = 32768;

// 8 MiB file.
const uint32_t FILE_SIZE = 256UL*BUF_DIM;

const uint32_t PPMPin = 10;//only serten pins work, eg, 10 on teensey 3.1
const uint32_t boardLEDPin = 13;

const uint32_t ValveAPin = 3;
const uint32_t ValveBPin = 4;
const uint32_t ValveCPin = 5;
const uint32_t ValveDPin = 6;
const uint32_t ValveEPin = 7;




PulsePositionInput RadioIn;
//SdFatSdioEX sd;

File file;


int WeaponState;
// WeaponStates
// -2	 == Full DUMP
// -1	 == non inishalised (	Inishaliseing part A )
// 1		== InishaliseingA
// 2		== InishaliseingB

// 4		== Disamred
// 5		== Charging
// 6		== Armed
// 7		== Firing
// 8		== Free	Return
// 9		== Power Return
// 10	 == Buffer Vent

int NewComandState=-1;
int TransitionTo;
int TransitionFrom = -99; // -99 can be the past

unsigned long TransitionStart;// 70 miniutes till micros overflows
unsigned long TransitionTimer1;



int ValveAopeningTime=10000;
int ValveAclosingTime=10000;
int ValveBopeningTime=10000;
int ValveBclosingTime=10000;
int ValveCopeningTime=10000;
int ValveCclosingTime=10000;
int ValveDopeningTime=10000;
int ValveDclosingTime=10000;
int ValveEopeningTime=10000;
int ValveEclosingTime=10000;



void setup() {
	
//Safety shit first
//Safety shit first



pinMode(ValveAPin,OUTPUT); 
pinMode(ValveBPin,OUTPUT); 
pinMode(ValveCPin,OUTPUT); 
pinMode(ValveDPin,OUTPUT); 
pinMode(ValveEPin,OUTPUT); 

digitalWrite(ValveAPin,0); // this should be true anyway.
digitalWrite(ValveBPin,0);
digitalWrite(ValveCPin,0);
digitalWrite(ValveDPin,0);
digitalWrite(ValveEPin,1);	// it would be cool to make sure A is shut
// but we would be waiting for ages in micro turms.
// so if there is a dogy power line cousing constant resets i would like to atleast be pwming some power into valve E



pinMode(boardLEDPin,OUTPUT);

RadioIn.begin(PPMPin);


	WeaponState=-1;
	TransitionTo=-1;
}


int count;


void loop() {
//Serial.println("hi ");
	if (WeaponState>=4){
		int num = RadioIn.available();
		if (num == 8) {

			float valArm = RadioIn.read(5);
			if (valArm<1800){
				WeaponState=4;
				
			}else {// We Are Armed
				float valF = RadioIn.read(5);
				if (valF<1450){
				NewComandState=5;// not fireing so Ask to transitiont to Armed when availale
				}else if (valF<1800) {
					if (WeaponState==6){
						NewComandState=8;// Ask for single Firing/free return
						
					}else {
						int sytatx=0;
					}
				}

			}
		}
	}

	if (WeaponState==-1){
//we have just turned on! we probaly dont have any ppm data yet
// Setup should have made use safe but we dont want to do anything silly 


// transition to INIT/safe.
// we need to put some air in the recoil chamber so it can be reged down to the corect amount

		if (TransitionTo !=1){
			TransitionTo = 1;
			TransitionStart=micros();
			TransitionFrom= -1;
		
		} else if (micros()<TransitionStart+ValveEopeningTime+100000){
			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,1);
		} else if(micros()<TransitionStart+ValveEopeningTime+100000+ValveEclosingTime){
			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0);
	
		} else if (micros()>TransitionStart+ValveEopeningTime+100000+ValveEclosingTime){ 

			WeaponState=1;
		}
	} else if (WeaponState == 1){
//InishaliseingA == 1;
// setting the presser in the recoil chamber to presuered

	
 
	// this is a transient state
	// it always trnaisitont to 2 and starts to transistion strait way
		if (TransitionTo !=2){
			TransitionTo = 2;
			TransitionStart=micros();
			TransitionFrom= 1;

		} else if (micros()<TransitionStart+ValveCopeningTime){
			digitalWrite(ValveAPin,0); // Comand Open
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,1);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0); 
		
		} else if (micros()>TransitionStart+ValveCopeningTime){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0);

		
		} else if (micros()>TransitionStart+ValveCopeningTime+ValveCclosingTime){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!
	
			WeaponState=2;
		}	

	} else if (WeaponState == 2){
//InishaliseingA == 1;
// setting the presser in the recoil chamber to presuered

	
 
		// this is a transient state
		// it always trnaisitont to 2 and starts to transistion strait way
		if (TransitionTo !=3){
			TransitionTo = 3;
			TransitionStart=micros();
			TransitionFrom= 2;

		} else if (micros()<TransitionStart+ValveDopeningTime){
			digitalWrite(ValveAPin,0); // Comand Open
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,1);
			digitalWrite(ValveEPin,0); 
		
		} else if (micros()>TransitionStart+ValveDopeningTime){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0);

		
		} else if (micros()>TransitionStart+ValveDopeningTime+ValveDclosingTime){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!
	
			WeaponState=4;
		
		}

	} else if (WeaponState == 4){
		 //Disarmed! 
		 // This is a safe mode, so gose straight to open E
		 // there for close A and B if 
		if (TransitionTo==5){
			// if we are disarmed then we are disarmed others should close E first
			// we could transisitio out by closing E but then we would have left fully Safe state.
			WeaponState=5;
	
		}else{
			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,1);
			if (NewComandState==5){
				TransitionTo=5;
				TransitionStart=micros();
				TransitionFrom= 4;
		
			}
		}
	} else if (WeaponState == 5){
		 // this is charging,
		if (TransitionTo!=6){
			TransitionTo=6;//armed
			TransitionStart=micros();
			TransitionFrom= 5;
		}else if (micros()<TransitionStart+ValveDclosingTime){
			digitalWrite(ValveAPin,0); // Comand close everything
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0); 
		
		} else if (micros()<TransitionStart+ValveDclosingTime+ValveAopeningTime+100000){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			digitalWrite(ValveAPin,1); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0);

		} else if (micros()<TransitionStart+ValveDclosingTime+ValveAopeningTime+100000+ValveAclosingTime){
		
			digitalWrite(ValveAPin,0); 
			digitalWrite(ValveBPin,0);
			digitalWrite(ValveCPin,0);
			digitalWrite(ValveDPin,0);
			digitalWrite(ValveEPin,0);
		} else {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!
			// we are now armed and ready to fire!
			WeaponState=6;
		
		}
	} else if (WeaponState == 6){	
	
		
	
	}else{
		Serial.println("WeaponState error!!");
		Serial.print("WeaponState: ");Serial.print(WeaponState);
		Serial.println();
	
	}
}





