#include "StateMachine.h"



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


const char *StateNames[numberofStates] = { "Safe State", "Retract/fill Buffer", "Expand Buffer","Rest","Arming","Ready to Fire","Waiting to Fire","Fire","Fired"}; 

const int StateLeadinTimes[numberofStates] = { 
/* Safe State */	100000, 
/* Retract/fill Buffer */	10000, 
/* Rest */	10000, 
/* charge */	10000, 
/* Armed */	10000, 
/* Fire */	10000,
/* Fired */	10000,
/* Fire */	10000,
/* Fired */	10000,
};
const int StateMinTimes[numberofStates] = { 
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


int StateMachineClass::defaultStateTimeOutFunction(int input){
printer->println("I'm StateMachineClass::defaultStateTimeOutFunction");

 if (TransitionStartMicros + StateMinTimes[currentState]+StateLeadinTimes[currentState] > micros() ){
return -1;// send me to safe!
}else{

return currentState;
}
}

typedef int (StateMachineClass::*IntFunctionWithOneParameter) (int a);
const IntFunctionWithOneParameter ContinueState [numberofStates] = { 
/* 0 -- Safe State */	&StateMachineClass::defaultStateTimeOutFunction,
/* 1 -- Retract P */	&StateMachineClass::defaultStateTimeOutFunction,
/* 2 -- Retract E */	&StateMachineClass::defaultStateTimeOutFunction,
/* 3 -- RestFull */	&StateMachineClass::defaultStateTimeOutFunction,
/* 4 -- Arming */	&StateMachineClass::defaultStateTimeOutFunction,
/* 5 -- ready */	&StateMachineClass::defaultStateTimeOutFunction,
/* 6 -- waiting */	&StateMachineClass::defaultStateTimeOutFunction,
/* 7 -- Fire */		&StateMachineClass::defaultStateTimeOutFunction,
/* 8 -- Fired */	&StateMachineClass::defaultStateTimeOutFunction
};




StateMachineClass::StateMachineClass( usb_serial_class &print ) { 
      printer = &print; //operate on the adress of print

      
}  



void StateMachineClass::tickFunction(void ){
	
	if (TelemiteryMode==1){
		//printer->println("StateMachineClass::tickFunction Telemity mode code gose hear");
		// telemitry mode, bit like a debug mode, so spit sutuff out a few times a second
		int tmpmillis=millis();
		if ( (tmpmillis >telemMillisRefreshRate) and ( telemMillis < tmpmillis-telemMillisRefreshRate)){
			
			printer->println("StateMachineClass::tickFunction Telemity mode code gose hear");
			telemMillis=millis();
		}
	}
	
	if(TransitionState!=0){
		int tmpContInfo=(this->*ContinueState[currentState])(0);
		if (tmpContInfo>-1)
		{
		TransitionState=0;
		printer->println("StateMachineClass::tickFunction now at State");
		}
	}
}

void StateMachineClass::setMachineState( int NewState ) { 
  
  
  	
	for ( int valveII;valveII< numberOfValues;valveII++){
		digitalWrite(valveII,ValueState[NewState][valveII]);
	}
	
	if (NewState!=currentState){
		printer->print("StateMachineClass::setMachineState going to");
		printer->println(NewState);
	}
	currentState=NewState;
	TransitionState=1;
	TransitionStartMicros=micros();
	
}  




void StateMachineClass::updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5){
	//printer->println("StateMachineClass");
	
	
	
	if (switch1==0){
		setMachineState(0);
		
	}else if(TransitionState!=0){
		printer->println("mid Transition");
	}else if (switch1==1){
	// normal active mode
		if (currentState==0){
			// go to rest from safe.
			if (switch2==0 and (switch3==0 or switch3==1) and switch5==0){
				// switch 4 is which control mode we are in so it dosnet mater.
				// so dose pot
				// so dose switch3=0 or 1 but switch3=2 dose somthing so dont let that through
				printer->println("we can go to state 3 now");
				setMachineState(3);
			}else{
				printer->println("not arming due to other switches");
			}
		}else if (currentState==3 and TransitionState ==0){
		// at rest
			if (switch3==2 and switch2==0 ){
				// active retract from rest
				
				setMachineState(1);
			}else if ( switch3 < 2 and switch5==0 and switch2==1){
			
				setMachineState(4);
			}
		}else if (currentState==3){	
			
		}else{
			//setMachineState(0);
		}
	
	}else if (switch1==2){
		TelemiteryMode=1;
		//printer->println("elemiteryMode=1;");
	}

}
