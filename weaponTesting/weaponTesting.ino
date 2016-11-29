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


int WeaponState=-1;
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

unsigned long HartBeatTimer;
int HaertBeat = -1;

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
	Seloind(boardLEDPin,LOW);
	RadioIn.begin(PPMPin);


	WeaponState=-1;
	TransitionTo=-1;
}


int count;


void loop() {

Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewComandState);Serial.println();


	if (WeaponState>=4){
		int num = RadioIn.available();
		if (num == 8) {

			float valArm = RadioIn.read(5);
			Serial.println(valArm);
			if (valArm<1800){
				WeaponState=4;
				NewComandState=4;
				TransitionTo=4;
			}else {// We Are Armed
				float valF = RadioIn.read(6);
				Serial.println(valF);
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
Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewComandState);Serial.println();
	if (WeaponState==-1) {
//we have just turned on! we probaly dont have any ppm data yet
// Setup should have made use safe but we dont want to do anything silly 


// transition to INIT/safe.
// we need to put some air in the recoil chamber so it can be reged down to the corect amount
		Serial.print("hi -1");Serial.print("hi -1");Serial.println();
		if (TransitionTo !=1) {
			TransitionTo = 1;
			TransitionStart=micros();
			TransitionFrom= -1;
		
		} else if (micros()<TransitionStart+ValveEopeningTime+100000) {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,1);
		} else if (micros()<TransitionStart+ValveEopeningTime+100000+ValveEclosingTime) {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);
	
		} else if (micros()>TransitionStart+ValveEopeningTime+100000+ValveEclosingTime) { 

			WeaponState=1;
		}
	} else if (WeaponState == 1) {
//InishaliseingA == 1;
// setting the presser in the recoil chamber to presuered

		Serial.print("hi 1");Serial.println(TransitionStart);
 
	// this is a transient state
	// it always trnaisitont to 2 and starts to transistion strait way
		if (TransitionTo != 2) {
			Serial.print("hi a");
			TransitionTo = 2;
			TransitionStart=micros();
			TransitionFrom= 1;

		} else if (micros()<TransitionStart+ValveCopeningTime) {
			Seloind(ValveAPin,LOW); // Comand Open
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,HIGH);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW); 
			Serial.print("hi b");
		} else if ((micros()>TransitionStart+ValveCopeningTime) and (micros()<TransitionStart+ValveCopeningTime+ValveCclosingTime) ){
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);
			Serial.print("hi c");
		
		} else  {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!
			Serial.print("hi d");
			WeaponState=2;
		}

	} else if (WeaponState == 2) {
//InishaliseingA == 1;
// setting the presser in the recoil chamber to presuered
		Serial.println("hi 2");
	
 
		// this is a transient state
		// it always trnaisitont to 2 and starts to transistion strait way
		if (TransitionTo !=3) {
			TransitionTo = 3;
			TransitionStart=micros();
			TransitionFrom= 2;

		} else if (micros()<TransitionStart+ValveDopeningTime) {
			Seloind(ValveAPin,LOW); // Comand Open
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,HIGH);
			Seloind(ValveEPin,LOW); 
		
		} else if ((micros()>TransitionStart+ValveDopeningTime) and (micros()<TransitionStart+ValveDopeningTime+ValveDclosingTime)) {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);

		
		} else if (micros()>TransitionStart+ValveDopeningTime+ValveDclosingTime) {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!
	
			WeaponState=4;
		
		}

	} else if (WeaponState == 4) {
		 //Disarmed! 
		 // This is a safe mode, so gose straight to open E
		 // there for close A and B if
		 
		if (micros()>HartBeatTimer+1000000){
			HartBeatTimer=micros();
			Serial.print("LED -- FLASH      **");
			if (HaertBeat<1){
				HaertBeat=1;
				Seloind(boardLEDPin,1);
			}else{
				HaertBeat=0;
				Seloind(boardLEDPin,0);
			}
		}
		
		if (TransitionTo==5) {
			// if we are disarmed then we are disarmed others should close E first
			// we could transisitio out by closing E but then we would have left fully Safe state.
			WeaponState=5;
	
		} else {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,HIGH);
			Seloind(ValveEPin,HIGH);
			if (NewComandState==5) {
				TransitionTo=5;
				TransitionStart=micros();
				TransitionFrom= 4;
		
			}
		}
		Seloind(boardLEDPin,0);
	} else if (WeaponState == 5) {
		 // this is charging,
		if (TransitionTo!=6) {
			TransitionTo=6;//armed
			TransitionStart=micros();
			TransitionFrom= 5;
		}else if (micros()<TransitionStart+ValveEclosingTime) {
			Seloind(ValveAPin,LOW); // Comand close everything
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW); 
		
		} else if (micros()<TransitionStart+ValveDclosingTime+ValveAopeningTime+100000) {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			Seloind(ValveAPin,HIGH); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);

		} else if (micros()<TransitionStart+ValveEclosingTime+ValveAopeningTime+100000+ValveAclosingTime) {
		
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);
		} else {
			// we are now armed and ready to fire!
			WeaponState=6;
		
		}
	} else if (WeaponState == 6){	
		if (NewComandState==8){//aim for a single simple fire.
			TransitionTo=7;// first stop is fireing
			TransitionStart=micros();
			TransitionFrom= 6;
		} else if (NewComandState==9){//aim for a single fire with power return.
			TransitionTo=7;// first stop is fireing
			TransitionStart=micros();
			TransitionFrom= 6;
		}
	} else if (WeaponState == 7){
	
	//big bang!!
		if ((TransitionTo != 8) and (TransitionTo != 9)){
			if (NewComandState==8){//aim for a single simple fire.
				TransitionTo=8;// first stop is fireing
				TransitionStart=micros();
				TransitionFrom= 7;
			} else if (NewComandState==9){//aim for a single fire with power return.
				TransitionTo=9;// first stop is fireing
				TransitionStart=micros();
				TransitionFrom= 7;
			}
	    	}//no need for a else if as may now be true
	    	if ((TransitionTo == 8) or (TransitionTo == 9)) {
	    		if (micros()<TransitionStart+ValveBopeningTime+1000){
	    		
				Seloind(ValveAPin,LOW); 
				Seloind(ValveBPin,HIGH);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
	    		}else if (micros()<TransitionStart+ValveBopeningTime+1000+ValveBclosingTime){
	    		
				Seloind(ValveAPin,LOW );
				Seloind(ValveBPin,LOW);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
	    		}else{
	    			WeaponState=TransitionTo;
	    		}
	    	}
	} else if (WeaponState == 8){
		// soft return
		// back to Charging so we can be armed again.
		
		
		
	} else if (WeaponState == 9){  
		// power return	
		// back to Charging so we can be armed again.
		
		
		
	} else {
		Serial.println("WeaponState error!!");
		Serial.print("WeaponState: ");Serial.print(WeaponState);
		Serial.println();
	
	}
}





