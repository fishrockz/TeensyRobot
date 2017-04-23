
#include <PulsePosition.h>


const int DebugLevel=1;

const uint32_t PPMPin = 10;//only certain pins work, eg, 10 on teensy 3.1
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

float valArm		= 0.0;
float valMode		= 0.0;
float valRetract	= 0.0;
float valFire		= 0.0;
int ValidPPMdata= 0;

unsigned long TimeEnteredState=0;

int Operational=-1;
// -1 -- turn on dont go staright to On
// 0 --  off
// 1 --  on

int OppsMode=0;
// 0 -- restfull
// 1 -- Single
// 2 -- Multi

int loopMode=0;
// 0 -- standard
// 1 -- slow
// 2 -- closed with fast time out
// 3 -- closed with slow option

int Action=0;
// 0 -- not doing stuff
// 1 -- retracting/ getting ready for rest or to arm
// 2 -- getting ready to fire
// 3 -- firing process


int ComandFiring=0;
// 0 --  not firing
// 1 -- firing

int ComandRetracting=0;
// 0 --  not firing
// 1 -- firing


//
//
//

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


int defaultSateFunction(int input){

return 0;// send me to safe!

}



int SafeSateFunction(int input){

	if ((Operational==1)
	and (ComandFiring==0)
	and (ComandRetracting==0)){
		Action=4;
		return 1;
	}else{
		return 0;// send me to safe!
	}
}





int RetractSateFunctionPresure(int input){

	if (Action==4 or Action==5){// getting readdy for rested, usually after leaving safe mode
		if (micros() > TimeEnteredState + 100000){
			// in rest full mode then give me a second of retraction
			return 2;// send me to Expand!
		}else{

			return -1;// -1 means dont change me
		}

	}else {//if (action==3)

		if (micros() > TimeEnteredState + 300000){
			// in rest full mode then give me a second of retraction
			return 2;// send me to restfull!
		}else{

			return -1;// -1 means dont change me
		}

	}

	
}
int RetractSateFunctionExpand(int input){
	if (Action==4){
		if (micros() > TimeEnteredState + 1000000){
			// in rest full mode then give me a second of retraction
			return 3;// send me to restfull!
		}else{

			return -1;// -1 means dont change me
		}
	}else if (Action==5){
		if (micros() > TimeEnteredState + 1000000){
			// in rest full mode then give me a second of retraction
			return 8;// send me to restfull!
		}else{
			return -1;// -1 means dont change me
		}	

	}else {//if (action==3)
		if (micros() > TimeEnteredState + 1000000){
			// in rest full mode then give me a second of retraction
			return 3;// send me to restfull!
		}else{
			return -1;// -1 means dont change me
		}

	}
}




int RestFullSateFunction(int input){

	if (Operational==1){
	
		if (OppsMode==0){
	
			if (ComandRetracting==1){

				Action=3;
				return 1;
			}else{
				return -1;
			}
		}else{
			Serial.println("go arm the weapon :)");
			Action=1;
			return 4;
		
		}
	}else{
		return 0;// send me to safe!
	}
}

int ArmingSateFunction(int input){
	if (OppsMode==0){
		return 3;
	}else{
		if (micros() > TimeEnteredState + 100000){
			return 5;// send me to restfull!
		}else{
			return -1;// -1 means dont change me
		}
	}
}

int ArmedSateFunction(int input){
	if (OppsMode==0){
		return 3;
	}else{
		if (ComandFiring==1){
			return 7;
		}else if (micros() > TimeEnteredState + 100000){
			return 6;// send me to restfull!
		}else{
			return -1;// -1 means dont change me
		}
	}
}

int ArmedWaitingSateFunction(int input){
	if (OppsMode==0){
		return 3;
	}else{
		if (ComandFiring==1){
			return 7;
		}else{
			return -1;// -1 means dont change me
		}
	}
}


int FireSateFunction(int input){

	if (micros() > TimeEnteredState + 100000){
		return 8;// send me to fired!
	}else{
		return -1;// -1 means dont change me
	}
	
} 

int FiredSateFunction(int input){

	if (OppsMode==0){
		return 3;
	}else if (OppsMode==2){
		if (micros() > TimeEnteredState + 100000){
			Action = 4;
			return 1;
		}else{
			return -1;// -1 means dont change me
		}
	}else{
		if (Action == 5){
			if (ComandRetracting==1){
				Action = 5;
				return 1;
			}else{
				return -1;
			}
		}else{
			Action = 5;
			return 1;
		}
	}
} 

typedef int (*IntFunctionWithOneParameter) (int a);
const IntFunctionWithOneParameter ContinueState [numberofStates] = { 
/* 0 -- Safe State */	SafeSateFunction,
/* 1 -- Retract P */	RetractSateFunctionPresure,
/* 2 -- Retract E */	RetractSateFunctionExpand,
/* 3 -- RestFull */	RestFullSateFunction,
/* 4 -- Arming */	ArmingSateFunction,
/* 5 -- ready */	ArmedSateFunction,
/* 6 -- waiting */	ArmedWaitingSateFunction,
/* 7 -- Fire */		FireSateFunction,
/* 8 -- Fired */	FiredSateFunction
};




void Solenoid(int ValveIndex,int state){
	digitalWrite(ValvePins[ValveIndex], state!= reversvalue[ValveIndex]);
}


void SetValuesByState(int state){
	if (DebugLevel>5){Serial.println(StateNames[state]);}
 
	for( int valveII = 0; valveII < numberOfValues; valveII += 1 ) {
		if (DebugLevel>20){Serial.print(valveII);}
		Solenoid(valveII,ValueState[state][valveII]);
		
	}
}



PulsePositionInput RadioIn;


void setup() {
	
	
	
	SetValuesByState(0);
	
	for( int valveII = 0; valveII < numberOfValues; valveII += 1 ) {
		pinMode(ValvePins[valveII],OUTPUT);
	}
	
	
	RadioIn.begin(PPMPin);
}


int CurrentState=0;
int LastState=0;
void proceduralStateMacheine(){

	
	if (micros()<TimeEnteredState+StateLeadinTimes[CurrentState]){
		for ( int valveII = 0; valveII < numberOfValues; valveII += 1 ) {
			if (ValueState[LastState][valveII]>ValueState[LastState][valveII]){
				Solenoid(valveII,0);
			}
		}
	
	}else{
	
		SetValuesByState(CurrentState);
	
		int newState = ContinueState[CurrentState](0);
	
		if (newState>=0) {
			if (DebugLevel>0){
				Serial.print("Was: ");
				Serial.print(StateNames[CurrentState]);
				Serial.print(", Now: ");
				Serial.println(StateNames[newState]);
			}
			TimeEnteredState=micros();
			LastState=CurrentState;
			CurrentState=newState;
		}
	
	
		if (CurrentState==0){SetValuesByState(CurrentState);}

	}
}



void loop(){

	
	
	int num = RadioIn.available();
	if (num == 8) {

		valArm = RadioIn.read(5);
		valMode = RadioIn.read(6);
		valRetract = RadioIn.read(7);
		valFire = RadioIn.read(8);
		// we should validate the read values hear, if out side of 950 to 2050 then rasie error.
		
		ValidPPMdata=1;
		if (DebugLevel>10){Serial.println(valArm);}
	}
	
	
	
	if ((valArm< 1800.0) or (valArm> 2100.0)){
		CurrentState=0;
		Operational=0;
	}else{
	//radio time out
	
	//max time on
		
		if ((Operational >-1) and (valMode<1250) and (valFire<1250) and (valRetract<1250) ) {
			Operational=1;
		}
	
	}
	
	
	
	
	
	if (Operational>0){
	
		//Operational=1;
		// 0 --  off
		// 1 --  on

		if (valMode<1250){
			// 0 -- restfull
			OppsMode=0;
		} else if (valMode<1750){
			// 1 -- Single
			OppsMode=1;
			//Serial.println("mode 1");
		}else{
			// 2 -- Multi
			OppsMode=2;
		}
	
	
		
		loopMode=0;
		// 0 -- standard
		// 1 -- slow
		// 2 -- closed with fast time out
		// 3 -- closed with slow option
		
		
	//	int Action=0;
	//	// 0 -- not doing stuff
	//	// 1 -- getting ready to fire
	//	// 2 -- firing process
	//      // 3 -- retracting on comand in restfull
	//	// 4 -- retracting after fire back to restfull/refire
	//	// 5 -- retracting after fire back to fired
		
		
		if (valFire<1750){
			ComandFiring=0;
			// 0 --  not firing
		}else{
			ComandFiring=1;
			// 1 -- firing
		}
		
		if (valRetract<1750){
			ComandRetracting=0;
			// 0 --  not retracting
		}else{
			ComandRetracting=1;
			// 1 -- retracting
		
		}
		
		
	
		proceduralStateMacheine();
	
	
	}else{
		CurrentState=0;
		Operational=0;
		SetValuesByState(0);
	}

	

	
	
}
