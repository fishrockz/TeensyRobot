//#include <SPI.h>
//#include "SdFat.h"
#include <PulsePosition.h>
// 32 KiB buffer.
//const size_t BUF_DIM = 32768;

// 8 MiB file.
//const uint32_t FILE_SIZE = 256UL*BUF_DIM;

const uint32_t PPMPin = 10;//only serten pins work, eg, 10 on teensey 3.1
const uint32_t boardLEDPin = 13;

const uint32_t ValveAPin = 4;// main fill port
const uint32_t ValveBPin = 5;// main exsorsed / fire
const uint32_t ValveCPin = 6;// retract fill
const uint32_t ValveDPin = 7;//retranct vent/exhaust
const uint32_t ValveEPin = 8;// side exsorsed.




PulsePositionInput RadioIn;
//SdFatSdioEX sd;

//File file;


int WeaponState=-1;
// WeaponStates
// -2	 	== Full DUMP
// -1	 	== non inishalised (	Inishaliseing part A )
// 1		== InishaliseingA
// 2		== InishaliseingB
//
// 4		== Disamred
// 
// 6		== retract
// 7		== extend
// 8		
// 9		
// 10		

int NewComandState=4;
int TransitionTo;
int TransitionFrom = -99; // -99 can be the past

unsigned long TransitionStart;// 70 miniutes till micros overflows
unsigned long TransitionTimer1;

unsigned long HartBeatTimer;
int HaertBeat = -1;

int ValveAopeningTime=100000;
int ValveAclosingTime=500000;
int ValveBopeningTime=100000;
int ValveBclosingTime=100000;
int ValveCopeningTime=100000;
int ValveCclosingTime=100000;
int ValveDopeningTime=100000;
int ValveDclosingTime=100000;
int ValveEopeningTime=100000;
int ValveEclosingTime=100000;

void Seloind(int pin,int state){
	digitalWrite(pin, state!=1);
}
void setup() {

	//Safety shit first
	//Safety shit first

	Seloind(ValveAPin,HIGH); // this should be true anyway.
	Seloind(ValveBPin,HIGH);
	Seloind(ValveCPin,HIGH);
	Seloind(ValveDPin,HIGH);
	Seloind(ValveEPin,LOW);

	pinMode(ValveAPin,OUTPUT); 
	pinMode(ValveBPin,OUTPUT); 
	pinMode(ValveCPin,OUTPUT); 
	pinMode(ValveDPin,OUTPUT); 
	pinMode(ValveEPin,OUTPUT); 

	// it would be cool to make sure A is shut
	// but we would be waiting for ages in micro turms.
	// so if there is a dogy power line cousing constant resets i would like to atleast be pwming some power into valve E



	pinMode(boardLEDPin,OUTPUT);
	digitalWrite(boardLEDPin,LOW);
	RadioIn.begin(PPMPin);


	WeaponState=4;
	TransitionTo=4;
	NewComandState=4;
}


int count;


void loop() {

//Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewComandState);Serial.println();


	if (micros()> 100000){//dont do anything but be safe for the first 10th of a second
		int num = RadioIn.available();
		if (num == 8) {

			float valArm = RadioIn.read(5);
			//Serial.println(valArm);
			if (valArm<1800){
				WeaponState=4;
				NewComandState=4;
				TransitionTo=4;
			}else {// We Are Armed
				float valF = RadioIn.read(6);
				//Serial.println(valF);
				if (valF<1450){
					NewComandState=6;// not fireing so Ask to transitiont to Armed when availale
				}else if (valF>1800) {
					if (WeaponState==6){
						NewComandState=7;// Ask for single Firing/free return
						
					}else {
						int sytatx=0;// dont go straight to firing from disarmed
					}
				}

			}
		}
	}
//Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewComandState);Serial.println();


	 if (WeaponState == 4) {
		 //Disarmed! 
		 // This is a safe mode, so gose straight to open E
		 // there for close A and B if
		 
		Seloind(ValveAPin,LOW); 
		Seloind(ValveBPin,LOW);
		Seloind(ValveCPin,LOW);
		Seloind(ValveDPin,HIGH);
		Seloind(ValveEPin,HIGH);
			 
		if (micros()>HartBeatTimer+1000000){
			HartBeatTimer=micros();
			Serial.print("LED -- FLASH      ** ");
			Serial.print(NewComandState);
			Serial.println(" Dis Arm Haert Beat");
			if (HaertBeat<1){
				HaertBeat=1;
				digitalWrite(boardLEDPin,1);
			}else{
				HaertBeat=0;
				digitalWrite(boardLEDPin,0);
			}
		}
		
		
		if (NewComandState==6) {
			digitalWrite(boardLEDPin,0);
			WeaponState=6;
			TransitionStart=micros();
			TransitionTo=6;
		}
		

	} else if (WeaponState == 6){
		//At this point we just sit and wait

		int shutingtime=250000;
		if (NewComandState==7){//aim for a single simple fire.
			if (TransitionTo!=7){
				TransitionTo=7;// first stop is fireing
				TransitionStart=micros();
				TransitionFrom= 6;
				Serial.println("Start Fire");
			}else if (TransitionStart<shutingtime){
				Seloind(ValveAPin,LOW); 
				Seloind(ValveBPin,LOW);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
			} else {
				WeaponState=7;
			}	

		} else {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,HIGH);
			Seloind(ValveDPin,HIGH);
			Seloind(ValveEPin,LOW);
			TransitionTo=6;
		}
	} else if (WeaponState == 7){
	
//At this point we just sit and wait

		int shutingtime=250000;
		if (NewComandState==6){//aim for a single simple fire.
			if (TransitionTo!=6){
				TransitionTo=7;// first stop is fireing
				TransitionStart=micros();
				TransitionFrom= 6;
				Serial.println("Start Fire");
			} else if (TransitionStart<shutingtime){
				Seloind(ValveAPin,LOW); 
				Seloind(ValveBPin,LOW);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
			} else {
				WeaponState=6;
			}	

		} else {
			Seloind(ValveAPin,HIGH); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);
			TransitionTo=7;
		}
		
	} else {
		Serial.println("WeaponState error!!");
		Serial.print("WeaponState: ");Serial.print(WeaponState);
		Serial.println();
		
		// default to disarmed (4)
		Seloind(ValveAPin,LOW); 
		Seloind(ValveBPin,LOW);
		Seloind(ValveCPin,LOW);
		Seloind(ValveDPin,HIGH);
		Seloind(ValveEPin,HIGH)	;
	}
}





