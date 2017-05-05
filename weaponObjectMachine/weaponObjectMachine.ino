//Teensy libarys
#include <PulsePosition.h>

//Custom Robot Libarys
#include "weaponObjectMachine.h" 

const int DebugLevel=20;

const uint32_t PPMPin = 10;//only certain pins work, eg, 9 and 10 on teensy 3.1 and teensy 3.5
const uint32_t boardLEDPin = 13;


const int numberOfValues=5;
// ValveAPin = 3;// chamber fill port
// ValveBPin = 4;// main pilot exhaust
// ValveCPin = 5;// buffer fill
// ValveDPin = 6;// buffer exhaust
// ValveEPin = 9;// chamber exhaust

const int ValvePins[numberOfValues] = {
/*Valve A*/  3,
/*Valve B*/  4,
/*Valve C*/  5,
/*Valve D*/  6,
/*Valve E*/  9

};


const int reversvalue [numberOfValues] = { 
/*Valve A*/  0,
/*Valve B*/  0,
/*Valve C*/  0,
/*Valve D*/  0,
/*Valve E*/  0,
};


const int numberofStates=9;

const int ValueState[numberofStates][numberOfValues] = {
/* 0 -- Safe State */ 
{0, 0, 0, 1, 1}, 

/* 1 -- Retract/fill Buffer */
{0, 0, 1, 0, 1}, 

/* 2 -- Expand Buffer */
{0, 0, 0, 0, 1}, 

/* 3 -- Rest */
{0, 0, 0, 0, 1}, 

/* 4 -- Arming */
{1, 0, 0, 1, 0}, 

/* 5 -- Ready to Fire */
{1, 0, 0, 0, 0},

/* 6 -- Waiting to Fire */
{0, 0, 0, 0, 0}, 

/* 7 -- Fire */
{0, 1, 0, 0, 0},

/* 8 -- Fired */
{0, 0, 0, 0, 1},
};


char *StateNames[numberofStates] = { "Safe State", "Retract/fill Buffer", "Expand Buffer","Rest","Arming","Ready to Fire","Waiting to Fire","Fire","Fired"}; 

const int StateLeadinTimes[numberofStates] = { 
/* Safe State */	100000, 
/* Retract/fill Buffer */	100000, 
/* Rest */	100000, 
/* charge */	100000, 
/* Armed */	100000, 
/* Fire */	100000,
/* Fired */	100000,
/* Fire */	100000,
/* Fired */	100000,
};




StateMachineClass theWeapon;
PulsePositionInput RadioIn;
void setup() {
  // put your setup code here, to run once:
	RadioIn.begin(9);
	
}



int ValidPPMdata=-1;
int valRadio1,valRadio2,valRadio3,valRadio4;
int switch1,switch2,switch3,switch4,switch5;






int pot1;


void twoSwitchesFromChan(int chanel,int &switchA,int &switchB) {
	

	chanel-=1000;
	if (chanel<325){
		switchA=0;
	}else if (chanel<700) {
		switchA=1;
	} else {
		switchA=2;
	}
	
	chanel-=switchA*300;
	
		
	if (chanel<1400){
		switchB=0;
	}else if (chanel<1600) {
		switchB=1;
	}else{
		switchB=2;
	}	



}


void loop() {
  // put your main code here, to run repeatedly:
	//theWeapon.foo();
	
	int num = RadioIn.available();
	if (num == 8) {

		valRadio1 = RadioIn.read(5);
		valRadio2 = RadioIn.read(6);
		valRadio3 = RadioIn.read(7);
		valRadio4 = RadioIn.read(8);
		// we should validate the read values hear, if out side of 950 to 2050 then rasie error.
		
		ValidPPMdata=1;
		//if (DebugLevel>10){Serial.println(valArm);}
		
		
		
		
		
		
		if (DebugLevel>10){Serial.print(valRadio1);Serial.print(" ");Serial.print(valRadio2);Serial.print(" ");Serial.print(valRadio3);Serial.print(" ");Serial.println(valRadio4);}
	

		twoSwitchesFromChan(valRadio2,switch1,switch2);
		twoSwitchesFromChan(valRadio3,switch3,switch4);
		
		
		
		if (DebugLevel>10){Serial.print(pot1);Serial.print(" ");Serial.print(switch1);Serial.print(" ");Serial.print(switch2);Serial.print(" ");Serial.print(switch3);Serial.print(" ");Serial.println(switch4);}
	}
	
	
	
	
	
	
}
