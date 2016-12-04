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

// 4		== Disamred
// 5		== Charging
// 6		== Armed
// 7		== Firing
// 8		== Free	Return
// 9		== Power Return
// 10		== Buffer Vent

int NewComandState=4;
int TransitionTo;
int TransitionFrom = -99; // -99 can be the past

unsigned long TransitionStart;// 70 miniutes till micros overflows
unsigned long TransitionTimer1;
unsigned long retractTimer1;
int retractState=0;

unsigned long HartBeatTimer;
int HaertBeat = -1;

int ValveAopeningTime=100000;
int ValveAclosingTime=100000;
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
						NewComandState=9;// Ask for single Firing/free return
						
					}else {
						int sytatx=0;
					}
				}

			}
		}
	}
//Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewComandState);Serial.println();
	if (WeaponState==-1) {
		// start of the retraction sequence
		// first vent

		int ExhaustTime = 100000;
		if (TransitionTo !=1) {
			TransitionTo = 1;
			TransitionStart=micros();
			TransitionFrom= -1;
			Serial.println("Start Init");
		} else if (micros()<TransitionStart+ValveEopeningTime+ExhaustTime) {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,1);
		} else if (micros()<TransitionStart+ValveEopeningTime+ExhaustTime+ValveEclosingTime) {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);
	
		} else if (micros()>TransitionStart+ValveEopeningTime+ExhaustTime+ValveEclosingTime) { 

			WeaponState=1;
		}
	} else if (WeaponState == 1) {


		// second retraction sequence
		// fill the retraction chamber with full pressure
 		int retractfilltime = 1000000;
 		int saftyCloseTime = 100000;
		if (TransitionTo != 2) {
			TransitionTo = 2;
			TransitionStart=micros();
			TransitionFrom= 1;
			Serial.println("Start Init 1");
		} else if (micros()<TransitionStart+ValveCopeningTime+retractfilltime) {
			Seloind(ValveAPin,LOW); // Comand Open
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,HIGH);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW); 

		} else if ((micros()>TransitionStart+ValveCopeningTime+retractfilltime) and (micros()<TransitionStart+ValveCopeningTime+retractfilltime+ValveCclosingTime+saftyCloseTime) ){
			//now close the values.

			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);

		
		} else  {

			WeaponState=2;
		}

	} else if (WeaponState == 2) {
	
		int retractemptytime = 2000000;
		int saftyCloseTime = 100000;
		// now we vent the retraction chamber to 0.3 bar
		if (TransitionTo !=3) {
			TransitionTo = 3;
			TransitionStart=micros();
			TransitionFrom= 2;
			Serial.println("Start Init 2");
		} else if (micros()<TransitionStart+ValveDopeningTime+retractemptytime) {
			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,HIGH);
			Seloind(ValveEPin,LOW); 
		
		} else if ((micros()>TransitionStart+ValveDopeningTime+retractemptytime) and (micros()<TransitionStart+ValveDopeningTime+retractemptytime+ValveDclosingTime+saftyCloseTime)) {
			// this is baicly only open long enught to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			Seloind(ValveAPin,LOW); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);

		
		} else if (micros()>TransitionStart+ValveDopeningTime+ValveDclosingTime) {
			// at this point we have done a init, this might just be because of a comand to init or because we are in the arming process
			if (NewComandState==6){
				WeaponState=5;
			}else{
				WeaponState=4;
			}
		}

	} else if (WeaponState == 4) {
		 //Disarmed! 
		 // This is a safe mode, so gose straight to open E
		 // there for close A and B if
		 
		Seloind(ValveAPin,LOW); 
		Seloind(ValveBPin,LOW);
		Seloind(ValveCPin,LOW);
		Seloind(ValveDPin,HIGH);
		Seloind(ValveEPin,HIGH);
		
		retractState=0;
			 
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
		
		if (TransitionTo == -1) {
			// I dont want some one to beable to undo armed but leave the init switch up and then the robot to init straght away.
			if (NewComandState != -1){
				TransitionTo=4;
			}else if (micros()>TransitionStart+100000){
				digitalWrite(boardLEDPin,0);
				WeaponState=-1;
			}
		} else {
			if (NewComandState==6) {
				digitalWrite(boardLEDPin,0);
				WeaponState=-1;
				TransitionStart=micros();
				TransitionTo=-1;
			} else if ((NewComandState==-1) and (TransitionTo!=-1)) {
				digitalWrite(boardLEDPin,0);
				TransitionTo=-1;
				TransitionStart=micros();
				TransitionFrom= 4;
			}
		}

	} else if (WeaponState == 5) {
		 // this is charging,
		 int filltime=1000000;
		if (TransitionTo!=6) {
			TransitionTo=6;//armed
			TransitionStart=micros();
			TransitionFrom= 5;
			Serial.println("Start charging");
		}else if (micros()<TransitionStart+ValveEclosingTime) {
			// be could be open too
			Seloind(ValveAPin,LOW); // Comand close everything
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW); 
		
		} else if (micros()<TransitionStart+ValveDclosingTime+ValveAopeningTime+filltime) {
			// Tune ME please!!

			Seloind(ValveAPin,HIGH); 
			Seloind(ValveBPin,LOW);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,LOW);

		} else {
			// we are now armed and ready to fire!
			WeaponState=6;
			Serial.println("Armed");
		
		}
	} else if (WeaponState == 6){
		//At this point we just sit and wait
		
		Seloind(ValveAPin,HIGH); 
		Seloind(ValveBPin,LOW);
		Seloind(ValveCPin,LOW);
		Seloind(ValveDPin,LOW);
		Seloind(ValveEPin,LOW);
		
		if (NewComandState==8){//aim for a single simple fire.
			TransitionTo=7;// first stop is fireing
			WeaponState=7;
			TransitionStart=micros();
			TransitionFrom= 6;
			Serial.println("Start Fire");
		} else if (NewComandState==9){//aim for a single fire with power return.
			TransitionTo=7;// first stop is fireing
			WeaponState=7;
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
	    	
	    	int fireingtime=100000;
	    	if ((TransitionTo == 8) or (TransitionTo == 9)) {
	    		if (micros()<TransitionStart+ValveAclosingTime){
	    		
		    		Seloind(ValveAPin,LOW); 
				Seloind(ValveBPin,LOW);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
	    		
	    		} else if (micros()<TransitionStart+ValveAclosingTime+ValveBopeningTime+fireingtime){
				Seloind(ValveAPin,LOW); 
				Seloind(ValveBPin,HIGH);
				Seloind(ValveCPin,LOW);
				Seloind(ValveDPin,LOW);
				Seloind(ValveEPin,LOW);
				// all return states want these values open
	    		}else{
	    			WeaponState=TransitionTo;
	    		}
	    	}
	} else if (WeaponState == 8){
		// soft return
		// back to Charging so we can be armed again.
		
		int RetractTime = 1000000;
		
		if (NewComandState == 6) {
			if (TransitionTo != 5){
				TransitionTo = 5;
				TransitionStart=micros();
				TransitionFrom= 1;
			}else if (micros() > TransitionStart+RetractTime) {
				WeaponState = 5;
			}

		} else {
			Seloind(ValveAPin,LOW);
			Seloind(ValveBPin,HIGH);
			Seloind(ValveCPin,LOW);
			Seloind(ValveDPin,LOW);
			Seloind(ValveEPin,HIGH);
		}
		
		
		
		
	} else if (WeaponState == 9){
		// power return	
		// back to Charging so we can be armed again.
		
		int RetractTime = 5000000;
		
		
		Seloind(ValveAPin,LOW);
		Seloind(ValveBPin,HIGH);
		Seloind(ValveCPin,HIGH);
		Seloind(ValveDPin,LOW);
		Seloind(ValveEPin,HIGH);
		
		
		if (retractState==0){
			retractState=1;
			retractTimer1=micros();
		} 
		
		if (micros()>RetractTime ){
			if (NewComandState == 6) {
				retractState = 0;
				WeaponState = 5;
				TransitionTo=5;
				
			}

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





