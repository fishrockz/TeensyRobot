#include <PulsePosition.h>


const uint32_t PPMPin = 10;//only certain pins work, eg, 10 on teensy 3.1
const uint32_t boardLEDPin = 13;

const uint32_t ValveAPin = 4;// chamber fill port
const uint32_t ValveBPin = 5;// main pilot exhaust
const uint32_t ValveCPin = 6;// buffer fill
const uint32_t ValveDPin = 7;// buffer exhaust
const uint32_t ValveEPin = 8;// chamber exhaust




PulsePositionInput RadioIn;
//SdFatSdioEX sd;

//File file;


int WeaponState=-1;
// WeaponStates
// -2	 	== Full dump
// -1	 	== non initialised ( initialising part A )
// 1		== initialising A
// 2		== initialising B

// 4		== Disarmed
// 5		== Charging
// 6		== Armed
// 7		== Firing
// 8		== Free	Return
// 9		== Power Return
// 10		== Buffer Vent

int NewCommandState=4;
int TransitionTo;
int TransitionFrom = -99; // -99 can be the past

unsigned long TransitionStart;// 70 minutes until micros overflows
unsigned long TransitionTimer1;
unsigned long RetractTimer1;
int retractState=0;

unsigned long HeartBeatTimer;
int HeartBeat = -1;

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

void Solenoid(int pin,int state){
	digitalWrite(pin, state!=1);
}
void setup() {

	//Safety shit first
	//Safety shit first

	Solenoid(ValveAPin,HIGH); // this should be true anyway.
	Solenoid(ValveBPin,HIGH);
	Solenoid(ValveCPin,HIGH);
	Solenoid(ValveDPin,HIGH);
	Solenoid(ValveEPin,LOW);

	pinMode(ValveAPin,OUTPUT); 
	pinMode(ValveBPin,OUTPUT); 
	pinMode(ValveCPin,OUTPUT); 
	pinMode(ValveDPin,OUTPUT); 
	pinMode(ValveEPin,OUTPUT); 

	// it would be cool to make sure A is shut
	// but we would be waiting for ages in micro terms.
	// so if there is a dodgy power line causing constant resets I would like to atleast be PWMing some power into valve E



	pinMode(boardLEDPin,OUTPUT);
	digitalWrite(boardLEDPin,LOW);
	RadioIn.begin(PPMPin);


	WeaponState=4;
	TransitionTo=4;
	NewCommandState=4;
}


int count;


void loop() {

//Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewCommandState);Serial.println();


	if (micros()> 100000){//dont do anything but be safe for the first 10th of a second
		int num = RadioIn.available();
		if (num == 8) {

			float valArm = RadioIn.read(5);
			//Serial.println(valArm);
			if (valArm<1800){
				WeaponState=4;
				NewCommandState=4;
				TransitionTo=4;
			}else {// We Are Armed
				float valF = RadioIn.read(6);
				//Serial.println(valF);
				if (valF<1450){
					NewCommandState=6;// not firing so Ask to transition to Armed when availale
				}else if (valF>1800) {
					if (WeaponState==6){
						NewCommandState=8;// Ask for single Firing/free return
            //NewCommandState=9;// Ask for single Firing/power return
						
					}else {
						int sytatx=0;
					}
				}

			}
		}
	}
//Serial.print("General: ");Serial.print(WeaponState);Serial.print(" ");Serial.print(TransitionTo);Serial.print(" ");Serial.print(NewCommandState);Serial.println();
	if (WeaponState==-1) {
		// start of the retract sequence
		// first vent

		int ExhaustTime = 300000;
		if (TransitionTo !=1) {
			TransitionTo = 1;
			TransitionStart=micros();
			TransitionFrom= -1;
			Serial.println("Start Init");
		} else if (micros()<TransitionStart+ValveEopeningTime+ExhaustTime) {
			Solenoid(ValveAPin,LOW); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,HIGH);
		} else if (micros()<TransitionStart+ValveEopeningTime+ExhaustTime+ValveEclosingTime) {
			Solenoid(ValveAPin,LOW); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,LOW);
	
		} else if (micros()>TransitionStart+ValveEopeningTime+ExhaustTime+ValveEclosingTime) { 

			WeaponState=1;
		}
	} else if (WeaponState == 1) {


		// second power retract sequence
		// fill the buffer volume with full pressure
 		int RetractFillTime = 5000000;
 		int SafetyCloseTime = 100000;
		if (TransitionTo != 2) {
			TransitionTo = 2;
			TransitionStart=micros();
			TransitionFrom= 1;
			Serial.println("Start Init 1");
		} else if (micros()<TransitionStart+ValveCopeningTime+RetractFillTime) {
			Solenoid(ValveAPin,LOW); // Command Open
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,HIGH);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,LOW); 

		} else if ((micros()>TransitionStart+ValveCopeningTime+RetractFillTime) and (micros()<TransitionStart+ValveCopeningTime+RetractFillTime+ValveCclosingTime+SafetyCloseTime) ){
			//now close the values.

			Solenoid(ValveAPin,LOW); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,LOW);

		
		} else  {

			WeaponState=2;
		}

	} else if (WeaponState == 2) {
	
		int RetractEmptyTime = 2000000;
		int SafetyCloseTime = 100000;
		// now we vent the buffer volume to 0.3 bar
		if (TransitionTo !=3) {
			TransitionTo = 3;
			TransitionStart=micros();
			TransitionFrom= 2;
			Serial.println("Start Init 2");
		} else if (micros()<TransitionStart+ValveDopeningTime+RetractEmptyTime) {
			Solenoid(ValveAPin,LOW); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,HIGH);
			Solenoid(ValveEPin,LOW); 
		
		} else if ((micros()>TransitionStart+ValveDopeningTime+RetractEmptyTime) and (micros()<TransitionStart+ValveDopeningTime+RetractEmptyTime+ValveDclosingTime+SafetyCloseTime)) {
			// this is basicaly only open long enough to open the value up a bit
			// Tune ME please!!

			// we want the value to always close by the end of this state

			Solenoid(ValveAPin,LOW); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,LOW);

		
		} else if (micros()>TransitionStart+ValveDopeningTime+ValveDclosingTime) {
			// at this point we have done an init, this might just be because of a command to init or because we are in the arming process
			if (NewCommandState==6){
				WeaponState=5;
			}else{
				WeaponState=4;
			}
		}

	} else if (WeaponState == 4) {
		 //Disarmed! 
		 // This is a safe mode, so goes straight to open E
		 // there for close A and B if
		 
		Solenoid(ValveAPin,LOW); 
		Solenoid(ValveBPin,LOW);
		Solenoid(ValveCPin,LOW);
		Solenoid(ValveDPin,HIGH);
		Solenoid(ValveEPin,HIGH);
		
		retractState=0;
			 
		if (micros()>HeartBeatTimer+1000000){
			HeartBeatTimer=micros();
			Serial.print("LED -- FLASH      ** ");
			Serial.print(NewCommandState);
			Serial.println(" Disarm Heart Beat");
			if (HeartBeat<1){
				HeartBeat=1;
				digitalWrite(boardLEDPin,1);
			}else{
				HeartBeat=0;
				digitalWrite(boardLEDPin,0);
			}
		}
		
		if (TransitionTo == -1) {
			// I dont want someone to be able to undo armed but leave the init switch up and then the robot to init straght away.
			if (NewCommandState != -1){
				TransitionTo=4;
			}else if (micros()>TransitionStart+100000){
				digitalWrite(boardLEDPin,0);
				WeaponState=-1;
			}
		} else {
			if (NewCommandState==6) {
				digitalWrite(boardLEDPin,0);
				WeaponState=-1;
				TransitionStart=micros();
				TransitionTo=-1;
			} else if ((NewCommandState==-1) and (TransitionTo!=-1)) {
				digitalWrite(boardLEDPin,0);
				TransitionTo=-1;
				TransitionStart=micros();
				TransitionFrom= 4;
			}
		}

	} else if (WeaponState == 5) {
		 // this is charging
		 int filltime=7000000;
		if (TransitionTo!=6) {
			TransitionTo=6;//armed
			TransitionStart=micros();
			TransitionFrom= 5;
			Serial.println("Start charging");
		}else if (micros()<TransitionStart+ValveEclosingTime) {
			// be could be open too
			Solenoid(ValveAPin,LOW); // Command close everything
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,LOW); 
		
		} else if (micros()<TransitionStart+ValveDclosingTime+ValveAopeningTime+filltime) {
			// Tune ME please!!

			Solenoid(ValveAPin,HIGH); 
			Solenoid(ValveBPin,LOW);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,HIGH);
			Solenoid(ValveEPin,LOW);

		} else {
			// we are now armed and ready to fire!
			WeaponState=6;
			Serial.println("Armed");
		
		}
	} else if (WeaponState == 6){
		//At this point we just sit and wait
		
		Solenoid(ValveAPin,HIGH); 
		Solenoid(ValveBPin,LOW);
		Solenoid(ValveCPin,LOW);
		Solenoid(ValveDPin,HIGH);
		Solenoid(ValveEPin,LOW);
		
		if (NewCommandState==8){//aim for a single simple fire.
			TransitionTo=7;// first stop is firing
			WeaponState=7;
			TransitionStart=micros();
			TransitionFrom= 6;
			Serial.println("Start Fire");
		} else if (NewCommandState==9){//aim for a single fire with power return.
			TransitionTo=7;// first stop is firing
			WeaponState=7;
			TransitionStart=micros();
			TransitionFrom= 6;
		}
	} else if (WeaponState == 7){
	// this is the fireing state, were only value B is open
	//big bang!!
		if ((TransitionTo != 8) and (TransitionTo != 9)){
			if (NewCommandState==8){//aim for a single simple fire.
				TransitionTo=8;// first stop is firing
				TransitionStart=micros();
				TransitionFrom= 7;
				
			} else if (NewCommandState==9){//aim for a single fire with power return.
				TransitionTo=9;// first stop is firing
				TransitionStart=micros();
				TransitionFrom= 7;
			}
	    	}//no need for a else if as may now be true
	    	// this value can be fettled to help with getting the retract right.
	    	int FiringTime=20000;
	    	if ((TransitionTo == 8) or (TransitionTo == 9)) {
	    		if (micros()<TransitionStart+ValveAclosingTime){
	    		
		    Solenoid(ValveAPin,LOW); 
				Solenoid(ValveBPin,LOW);
				Solenoid(ValveCPin,LOW);
				Solenoid(ValveDPin,LOW);
				Solenoid(ValveEPin,LOW);
	    		
	    		} else if (micros()<TransitionStart+ValveAclosingTime+ValveBopeningTime+FiringTime){
				Solenoid(ValveAPin,LOW); 
				Solenoid(ValveBPin,HIGH);
				Solenoid(ValveCPin,LOW);
				Solenoid(ValveDPin,LOW);
				Solenoid(ValveEPin,LOW);
				// all return states want these values open
	    		}else{
	    			WeaponState=TransitionTo;
	    		}
	    	}
	} else if (WeaponState == 8){
		// soft return
		// back to Charging so we can be armed again.
		
		int RetractTime = 300000;
		
		if (NewCommandState == 6) {
			if (TransitionTo != 5){
				TransitionTo = 5;
				TransitionStart=micros();
				TransitionFrom= 1;
			}else if (micros() > TransitionStart+RetractTime) {
				WeaponState = 5;
			}

		} else {
			Solenoid(ValveAPin,LOW);
			Solenoid(ValveBPin,HIGH);
			Solenoid(ValveCPin,LOW);
			Solenoid(ValveDPin,LOW);
			Solenoid(ValveEPin,HIGH);
		}
		
		
		
		
	} else if (WeaponState == 9){
		// power retract
		// back to Charging so we can be armed again.
		
		int RetractTime = 3000000;
		
		
		Solenoid(ValveAPin,LOW);
		Solenoid(ValveBPin,HIGH);
		Solenoid(ValveCPin,HIGH);
		Solenoid(ValveDPin,LOW);
		Solenoid(ValveEPin,HIGH);
		
		
		if (retractState==0){
			retractState=1;
			RetractTimer1=micros();
		} 
		
		if (micros()>RetractTimer1+RetractTime ){
			if (NewCommandState == 6) {
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
		Solenoid(ValveAPin,LOW); 
		Solenoid(ValveBPin,LOW);
		Solenoid(ValveCPin,LOW);
		Solenoid(ValveDPin,HIGH);
		Solenoid(ValveEPin,HIGH)	;
	}
}





