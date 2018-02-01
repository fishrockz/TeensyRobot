#include "StateMachine.h"

#include <Servo.h> 

//Value Stuff
const int numberOfValues=6;
// ValveAPin = 3;  // chamber fill port (NC)
// ValveBPin = 4;  // main pilot exhaust (NC)
// ValveCPin = 5;  // buffer fill (NO)
// ValveDPin = 6;  // buffer exhaust(NC)
// ValveEPin = 9;  // chamber exhaust(NC)
// ValveA2Pin = 3; // high flow chamber fill port (NO)

#ifdef WillsBoard
	/* wills pin out */
	/* set line 17 in arduino-1.8.2/hardware/teensy/avr/platform.txt to recipe.cpp.o.pattern="{compiler.path}{build.toolchain}{build.command.g++}" -c {build.flags.optimize} {build.flags.common} {build.flags.dep} {build.flags.cpp} {build.flags.cpu} {build.flags.defs} -DARDUINO={runtime.ide.version} -DF_CPU={build.fcpu} -D{build.usbtype} -DLAYOUT_{build.keylayout} -DWillsBoard {includes} "{source_file}" -o "{object_file} "*/
	const int ValvePins[numberOfValues] = {
	/*Valve A*/  23,
	/*Valve B*/  22,
	/*Valve C*/  21,
	/*Valve D*/  5,
	/*Valve E*/  6,
	/*Valve A2*/  37,
	// /*Aux1 */    36
	// /*Aux1 */    38
	};
#else 
	/* Doug's pinout*/
	const int ValvePins[numberOfValues] = {
	/*Valve A*/  14,  //4
	/*Valve B*/  15, //5
	/*Valve C*/  6,
	/*Valve D*/  7,
	/*Valve E*/  8,
	/*Valve A2*/ 9,
	};
#endif

const int reversvalue [numberOfValues] = { 
/*Valve A*/  0,
/*Valve B*/  0,
/*Valve C*/  1,
/*Valve D*/  0,
/*Valve E*/  0,
/*Valve A2*/ 1,

};


const boolean servopin[numberOfValues] = { 
/*Valve A*/  1,
/*Valve B*/  1,
/*Valve C*/  0,
/*Valve D*/  0,
/*Valve E*/  0,
/*Valve A2*/ 0,

};



const int ServoStart [numberOfValues] = { 
/*Valve A*/  0,
/*Valve B*/  0,
/*Valve C*/  10,
/*Valve D*/  0,
/*Valve E*/  0,
/*Valve A2*/ 10,

};



const int ServoEnd [numberOfValues] = { 
/*Valve A*/  0,
/*Valve B*/  0,
/*Valve C*/  120,
/*Valve D*/  0,
/*Valve E*/  0,
/*Valve A2*/ 120,

};




Servo * servoObj[numberOfValues]={0};//please dont change.

//state stuff
const int numberofStates=9;

const int ValueState[numberofStates][numberOfValues] = {
/* 0 -- Safe State */ 
{0, 0, 0, 1, 1, 0}, 

/* 1 -- Retract/fill Buffer */
{0, 0, 1, 0, 1, 0}, 

/* 2 -- Expand Buffer */
{0, 0, 0, 0, 1, 0}, 

/* 3 -- Rest */
{0, 0, 0, 0, 1, 0}, 

/* 4 -- Arming */
{1, 0, 0, 1, 0, 1}, 

/* 5 -- Ready to Fire */
{1, 0, 0, 0, 0, 1},

/* 6 -- Waiting to Fire */
{0, 0, 0, 0, 0, 0}, 

/* 7 -- Fire */
{0, 1, 0, 0, 0, 0},

/* 8 -- Fired */
{0, 1, 0, 0, 1, 0},
};


const char *StateNames[numberofStates] = { "Safe State", "Retract/fill Buffer", "Expand Buffer","Rest","Arming","Ready to Fire","Waiting to Fire","Fire","Fired"}; 

const int StateLeadinTimes[numberofStates] = { 
/* Safe State */		        10000, 
/* Retract/fill Buffer */   10000, 
/* Retract wait */	        10000, 
/* Rest */			            10000, 
/* Arming */			          10000, 
/* Ready to Fire */		      10000,
/* Waiting to Fire */	  	  10000,
/* Fire */			            10000,
/* Fired */			            10000,
};

const int StateMinTimes[numberofStates] = { 
/* Safe State */            10000, 
/* Retract/fill Buffer */  400000, // 0.4seconds
/* Retract wait */         800000, // 0.8 seconds of expansion
/* Rest */                  10000, 
/* Arming */                10000, 
/* Ready to Fire */         10000,
/* Waiting to Fire */       10000,
/* Fire */                 800000, // waits 0.8 sec before venting chamber after firing?
/* Fired */               1000000, // 1s venting duration
};

const int StateAutoTransitionTimes[numberofStates] = { 
/* Safe State */            10000, 
/* Retract/fill Buffer */   10000, 
/* Retract wait */          10000, 
/* Rest */                  10000, 
/* Arming */              2000000, // 2 seconds fill
/* Ready to Fire */         10000, // 0.01 seconds
/* Waiting to Fire */       10000,
/* Fire */                  10000,
/* Fired */                 10000,
};



const int AutoTransitionTo[numberofStates] = { 
/* 0 - Safe State */		      -1, 
/* 1 - Retract/fill Buffer */	 2, 
/* 2 - Retract wait */		    -1, 
/* 3 - Rest */		          	-1, 
/* 4 - Arming */		           5, 
/* 5 - Ready to Fire */		     6,
/* 6 - Waiting to Fire */    	-1,
/* 7 - Fire */			           8,
/* 8 - Fired */			           1,
};



//State Machine

int StateMachineClass::defaultStateTimeOutFunction(int input){
//printer->println("I'm StateMachineClass::defaultStateTimeOutFunction");
	//StateMinTimes[currentState]+StateLeadinTimes[currentState]
	unsigned long currentTime = micros();
	
	
	if (	TransitionStartMicros + StateLeadinTimes[currentState] + StateMinTimes[currentState] + StateAutoTransitionTimes[currentState] < currentTime ){
		return 0;// past auto transition
	
	} else if (		TransitionStartMicros + StateLeadinTimes[currentState] + StateMinTimes[currentState] < currentTime ){
		return 1; // past min
	} else {
		return -1;// in transion, do not change
	}
}

typedef int (StateMachineClass::*IntFunctionWithOneParameter) (int a);
const IntFunctionWithOneParameter ContinueState [numberofStates] = { 
/* 0 -- Safe State */	&StateMachineClass::defaultStateTimeOutFunction,
/* 1 -- Retract P */	&StateMachineClass::defaultStateTimeOutFunction,
/* 2 -- Retract E */	&StateMachineClass::defaultStateTimeOutFunction,
/* 3 -- RestFull */	  &StateMachineClass::defaultStateTimeOutFunction,
/* 4 -- Arming */	    &StateMachineClass::defaultStateTimeOutFunction,
/* 5 -- ready */	    &StateMachineClass::defaultStateTimeOutFunction,
/* 6 -- waiting */	  &StateMachineClass::defaultStateTimeOutFunction,
/* 7 -- Fire */		    &StateMachineClass::defaultStateTimeOutFunction,
/* 8 -- Fired */	    &StateMachineClass::defaultStateTimeOutFunction,
};



/*
StateMachineClass::StateMachineClass( usb_serial_class &print ) { 
      printer = &print; //operate on the address of print
      
}  
*/

StateMachineClass::StateMachineClass( usb_serial_class &print,HardwareSerial &print2 ) { 
      printer = &print; //operate on the address of print
printerHW = &print2;
      
}

void StateMachineClass::EnableStateMachine( ) { 
  
  	
	for ( int valveII;valveII< numberOfValues;valveII++){
		//digitalWrite(ValvePins[valveII],ValueState[NewState][valveII]);
		if (servopin[valveII] == 0 ){
			pinMode(ValvePins[valveII],OUTPUT);
/*			servopin is const array so we never need to do this.
 *      if (servoObj[valveII]){
 *				servoObj[valveII]->detach();
 *			}*/
	 	}else{
			if (servoObj[valveII]){
			    servoObj[valveII]->attach(ValvePins[valveII]);
			}else{
					Servo * tmpPointer = new Servo;
					servoObj[valveII] = tmpPointer;
					servoObj[valveII]->attach(ValvePins[valveII]);
			}
	 	}
	}
//for( int valveII = 0; valveII < numberOfValues; valveII += 1 ) {
//		);
//	}
	
}  
/*
void StateMachineClass::debugFunction(void ){

	printer->print("StateMachineClass::debugFunction");
	printer->print("<TelemPacket>");
	printer->print("<State=");
	printer->print(currentState);
	printer->print(">");
	printer->print("<TransitionState=");
	printer->print(TransitionState);
	printer->print(">");
	printer->print("<happyMachine=");
	printer->print(happyMachine);
	printer->print(">");
	printer->print("<Sensor1=");
	printer->print(Sensor1);
	printer->print(">");
	printer->print("<Sensor2=");
	printer->print(Sensor2);
	printer->print(">");

	for ( int valveII;valveII< numberOfValues;valveII++){
		printer->print("<ValveII-");
		printer->print(valveII);
		printer->print("=");
		printer->print(ValueState[currentState][valveII]);
		printer->print(">");

	} 

	printer->println("</ TelemPacket>");
}
*/
void StateMachineClass::QuickdebugFunction(void ){
	printerHW->print("<TelemPacket><State=");
	printerHW->print(currentState);
	printerHW->println("></ TelemPacket>");
	}
void StateMachineClass::debugFunction(void ){

	//if (debugHW==1){
		printerHW->print("StateMachineClass::debugFunction");
		printerHW->print("<TelemPacket>");
		printerHW->print("<State=");
		printerHW->print(currentState);
		printerHW->print(">");
		printerHW->println("</ TelemPacket>");
		
		
		
//		printerHW->print("<TransitionState=");
//		printerHW->print(TransitionState);
//		printerHW->print(">");
//		printerHW->print("<happyMachine=");
//		printerHW->print(happyMachine);
//		printerHW->print(">");

		printerHW->print("<TelemPacket>");
		printerHW->print("<Sensor1=");
		printerHW->print(Sensor1);
		printerHW->println(">");
		printerHW->print("<Sensor2=");
		printerHW->print(Sensor2);
		printerHW->println(">");
		printerHW->println("</ TelemPacket>");
		
		printerHW->print("<TelemPacket>");
		for ( int valveII;valveII< numberOfValues;valveII++){
			printerHW->print("<ValveII-");
			printerHW->print(valveII);
			printerHW->print("=");
			printerHW->print(ValueState[currentState][valveII]);
			printerHW->println(">");
		
		} 


		printerHW->println("</ TelemPacket>");
	
	//}else{
		printer->print("StateMachineClass::debugFunction");
		printer->print("<TelemPacket>");
		printer->print("<State=");
		printer->print(currentState);
		printer->print(">");
		printer->print("<TransitionState=");
		printer->print(TransitionState);
		printer->print(">");
		printer->print("<happyMachine=");
		printer->print(happyMachine);
		printer->print(">");
		printer->print("<Sensor1=");
		printer->print(Sensor1);
		printer->print(">");
		printer->print("<Sensor2=");
		printer->print(Sensor2);
		printer->print(">");

		for ( int valveII;valveII< numberOfValues;valveII++){
			printer->print("<ValveII-");
			printer->print(valveII);
			printer->print("=");
			printer->print(ValueState[currentState][valveII]);
			printer->print(">");
		
		} 

		printerHW->println("</ TelemPacket>");
	
	
	//}
}

void StateMachineClass::debugFunction(HardwareSerial MySerial ){

	MySerial.print("StateMachineClass::debugFunction");
	MySerial.print("<TelemPacket>");
	MySerial.print("<State=");
	MySerial.print(currentState);
	MySerial.print(">");
	MySerial.print("<TransitionState=");
	MySerial.print(TransitionState);
	MySerial.print(">");
	MySerial.print("<happyMachine=");
	MySerial.print(happyMachine);
	MySerial.print(">");
	MySerial.print("<Sensor1=");
	MySerial.print(Sensor1);
	MySerial.print(">");
	MySerial.print("<Sensor2=");
	MySerial.print(Sensor2);
	MySerial.print(">");

	for ( int valveII;valveII< numberOfValues;valveII++){
		MySerial.print("<ValveII-");
		MySerial.print(valveII);
		MySerial.print("=");
		MySerial.print(ValueState[currentState][valveII]);
		MySerial.print(">");
		
	} 

	MySerial.println("</ TelemPacket>");
}


float readPressureSensor(int pin){

	int val = analogRead(pin);
	// 5 122
	// 2.5 96
	// minimum seen: 20
	float voltage = (float)val / 1024 * 3.3;
	float pressure = voltage * 3.2 / 1 /10 * 60;
	return pressure;

}
void StateMachineClass::tickFunction(void ){
	
	if (TelemetryMode==1){
		//printer->println("StateMachineClass::tickFunction Telemetry mode code goes here");
		// Telemetry mode. A bit like a debug mode, so spit stuff out a few times a second.
		
		// don't do this on the time critical states, ie, 1 and 7
		if (currentState==0 or currentState==2 or currentState==3 or currentState==4 or currentState==5 or currentState==6 or currentState==8 or currentState==9){
			int tmpmillis=millis();
			if ( (tmpmillis >telemMillisRefreshRate) and ( telemMillis < tmpmillis-telemMillisRefreshRate)){
			
				printer->println("StateMachineClass::tickFunction Telemetry mode code goes here");
												
				//Sensor1=readPressureSensor(3);// these are not analog pins!! did i mean A3?
				//Sensor2=readPressureSensor(2);// these are not analog pins!! did i mean A2?
						
				debugFunction();
			
				telemMillis=millis();
			}
		}
	}
	
	if(TransitionState!=0){
		int tmpContInfo=(this->*ContinueState[currentState])(0);
	//	printer->print("StateMachineClass::tickFunction transition test ");
	//	printer->println(tmpContInfo);
		
		if (tmpContInfo==0)
		{
			//if (TransitionState<1){
				printer->print("StateMachineClass::tickFunction now at State ");
				printer->print(currentState);
				printer->print(" ");
				printer->println(StateNames[currentState]);
			//}
			TransitionState=0;
			
			if (AutoTransitionTo[currentState]>-1){
				int gotostate=AutoTransitionTo[currentState];
					
				printer->print("StateMachineClass::tickFunction going to State ");
				printer->print(gotostate);
				printer->print(" ");
				printer->println(StateNames[gotostate]);
			
				setMachineState(gotostate);
			}
		}else if (tmpContInfo==1){
			if (TransitionState!=1){
				printer->print("StateMachineClass::tickFunction passed minimum time in state: ");
				printer->print(currentState);
				printer->print(" ");
				printer->println(StateNames[currentState]);
			
			}
			
			TransitionState=1;
		}
	}
}


void StateMachineClass::externalRequest( int NewState ) { 
	if (NewState==0){
		setMachineState ( NewState );
	}else{
		// this is worth considering, maybe via some kind of merge of updateSwitches
	}
}

void StateMachineClass::setMachineState( int NewState ) { 
 	 int PinVal;
  
  	
	for ( int valveII;valveII< numberOfValues;valveII++){
		//digitalWrite(valveII,ValueState[NewState][valveII]);
		PinVal=ValueState[NewState][valveII];
		if (reversvalue[valveII]==1){
			if (PinVal==1) {PinVal=0;}else{PinVal=1;}
		}
		if (servopin[valveII] == 0 ){
			digitalWrite(ValvePins[valveII],PinVal);
		}else{
			servoObj[valveII]->write(ServoStart[valveII]*( !PinVal )+  ServoEnd[valveII]*( PinVal ));
		}
	}
	
	if (NewState!=currentState){
		/*printer->print("StateMachineClass::setMachineState going to ");
		printer->print(NewState);
		printer->print(" ");
		printer->println(StateNames[NewState]);	*/
		QuickdebugFunction();	
		
		currentState=NewState;
		TransitionState=-1;
		TransitionStartMicros=micros();
	}
	
	
}  



void StateMachineClass::updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5){
	//printer->println("StateMachineClass");
	
	
	//switch1 0: safe            1: rest           2:
	//switch2 0: rest            1: single fire    2: multi fire
	//switch3 0: short retract   1: long retract   2: retract now
	//switch4 0: open loopA      1: open loopB     2: closed
	//switch5 0: rest            1:                2: fire
	
	if (switch1==0){
		setMachineState(0);
		if (happyMachine!=1 and switch1==0 and switch2==0 and (switch3==0 or switch3==1) and switch5==0){
			printer->println("now I'm happy! :)");
			setMachineState(0);
			happyMachine=1;
		}
	}else if(TransitionState<0){
		//printer->println("mid Transition");
		
	}else if (happyMachine!=1){
		printer->println("not happy!");
		
	}else if (switch1>0){
	// normal active mode
	//4568
		if (currentState==0 or currentState==2 ){
			// go to rest from safe.
			if (switch2==0 and (switch3==0 or switch3==1) and switch5==0){
				// switch 4 is which control mode we are in so it doesn't matter.
				// so does pot
				// so does switch3=0 or 1 but switch3=2 does do something so don't let that through
				printer->println("we can go to state 3 now");
				setMachineState(3);
			}else{
				if (currentState==0){
					printer->println("not going to rest due to other switches");
				}else if (currentState==2){
					if (switch2==2 and (switch3==0 or switch3==1) and switch5==0){
						//if switch3==1 then the extra time in two is in the min time so we don't get here until the extended min time has been used.
						setMachineState(4);
					
					}
				}
			}
		}else if (currentState==4 or currentState==5 or currentState==6 or currentState==8){
			// if in state 2 or 8 but less than min time then we will be in TransitionState and not get here
			if (switch2==0){
				setMachineState(3);
			}else if (switch2>0){
				
				if (currentState==5 or currentState==6){ 
					if (switch5==2){
						setMachineState(7);
					}
								
				
				}
			}
			
		}else if (currentState==3 and TransitionState >-1){
		// at rest
			if (switch3==2 and switch2==0 ){
				// active retract from rest
				
				setMachineState(1);
			}else if ( switch3 < 2 and switch5==0 and switch2==1){
			
				setMachineState(4);
			}
					
			
		}else{
			//setMachineState(0);
		}		

    
	}	
	if (switch1==2){
		TelemetryMode=1;
		//printer->println("TelemetryMode=1;");
	}

}
