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
/* Retract/fill Buffer */	100000, 
/* Rest */	100000, 
/* charge */	100000, 
/* Armed */	100000, 
/* Fire */	100000,
/* Fired */	100000,
/* Fire */	100000,
/* Fired */	100000,
};



StateMachineClass::StateMachineClass( usb_serial_class &print ) { 
      printer = &print; //operate on the adress of print

      
}  



void StateMachineClass::setMachineState( int NewState ) { 
  
  
  	
	for ( int valveII;valveII< numberOfValues,valveII++;){
		digitalWrite(valveII,ValueState[NewState][valveII]);
	}
	currentState=NewState;
	if (NewState!=currentState){
		printer->print("StateMachineClass::setMachineState");
		printer->println(NewState);
	}
}  




void StateMachineClass::updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5){
	printer->println("StateMachineClass");
	
	
	
	if (switch1==0){
		setMachineState(0);
	}
	
	
	
	
	
	
	if (DebugLevel>10){printer->print(switch1);printer->print(" ");printer->print(switch2);printer->print(" ");printer->print(switch3);printer->print(" ");printer->print(switch4);printer->print(" ");printer->println(switch5);}
	

}
