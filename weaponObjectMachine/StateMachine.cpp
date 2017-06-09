#include "StateMachine.h"

#include "SerialPrint.h"

//Valve Stuff
static const int N_VALVES=6;

// ValveAPin = 3;  // chamber fill port (NC)
// ValveBPin = 4;  // main pilot exhaust (NC)
// ValveCPin = 5;  // buffer fill (NO)
// ValveDPin = 6;  // buffer exhaust(NC)
// ValveEPin = 9;  // chamber exhaust(NC)
// ValveA2Pin = 3; // high flow chamber fill port (NO)

#ifdef WillsBoard
    /* wills pin out */
    /* set line 17 in arduino-1.8.2/hardware/teensy/avr/platform.txt to recipe.cpp.o.pattern="{compiler.path}{build.toolchain}{build.command.g++}" -c {build.flags.optimize} {build.flags.common} {build.flags.dep} {build.flags.cpp} {build.flags.cpu} {build.flags.defs} -DARDUINO={runtime.ide.version} -DF_CPU={build.fcpu} -D{build.usbtype} -DLAYOUT_{build.keylayout} -DWillsBoard {includes} "{source_file}" -o "{object_file} "*/
    const int ValvePins[N_VALVES] = {
    /*Valve A*/  23,
    /*Valve B*/  22,
    /*Valve C*/  21,
    /*Valve D*/  5,
    /*Valve E*/  6,
    /*Valve A2*/  37
    // /*Aux1 */    36
    // /*Aux1 */    38
    };
#else
    /* Doug's pinout*/
    const int ValvePins[N_VALVES] = {
    /*Valve A*/  4,
    /*Valve B*/  5,
    /*Valve C*/  6,
    /*Valve D*/  7,
    /*Valve E*/  8,
    /*Valve A2*/ 9,
    };
#endif

const int reversvalue [N_VALVES] = {
/*Valve A*/  0,
/*Valve B*/  0,
/*Valve C*/  1,
/*Valve D*/  0,
/*Valve E*/  0,
/*Valve A2*/ 1,

};

//state stuff
const int numberofStates=9;

const int ValveState[numberofStates][N_VALVES] = {
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


const char *StateNames[numberofStates] = {
    "Safe State",
    "Retract/fill Buffer",
    "Expand Buffer",
    "Rest",
    "Arming",
    "Ready to Fire",
    "Waiting to Fire",
    "Fire",
    "Fired"
};

enum States {
    STATE_SAFE = 0,
    STATE_RETRACT = 1,
    STATE_EXPAND = 2,
    STATE_REST = 3,
    STATE_ARMING = 4,
    STATE_READY_TO_FIRE = 5,
    STATE_WAITING_TO_FIRE = 6,
    STATE_FIRE = 7,
    STATE_FIRED = 8,
};

const int StateLeadinTimes[numberofStates] = {
/* Safe State */              10000,
/* Retract/fill Buffer */     10000,
/* Retract wait */            10000,
/* Rest */                    10000,
/* Arming */                  10000,
/* Ready to Fire */           10000,
/* Waiting to Fire */         10000,
/* Fire */                    10000,
/* Fired */                   10000,
};

const int StateMinTimes[numberofStates] = {
/* Safe State */                 10000,
/* Retract/fill Buffer */       500000, // 0.5 or 1 seconds??
/* Retract wait */             3000000, // 4 seconds of expansion
/* Rest */                       10000,
/* Arming */                     10000,
/* Ready to Fire */              10000,
/* Waiting to Fire */            10000,
/* Fire */                      500000, // 0.5 to 1 second
/* Fired */                    1000000,
};

const int StateAutoTransitionTimes[numberofStates] = {
/* Safe State */              10000,
/* Retract/fill Buffer */     10000,
/* Retract wait */            10000,
/* Rest */                    10000,
/* Arming */                3000000, // 5 seconds
/* Ready to Fire */           10000, // 7 seconds
/* Waiting to Fire */         10000,
/* Fire */                    10000,
/* Fired */                   10000,
};



const int AutoTransitionTo[numberofStates] = {
/* 0 - Safe State */             -1,
/* 1 - Retract/fill Buffer */     2,
/* 2 - Retract wait */           -1,
/* 3 - Rest */                   -1,
/* 4 - Arming */                  5,
/* 5 - Ready to Fire */           6,
/* 6 - Waiting to Fire */        -1,
/* 7 - Fire */                    8,
/* 8 - Fired */                   1,
};



//State Machine

int StateMachineClass::defaultStateTimeOutFunction(int input) {
    debug_println("I'm StateMachineClass::defaultStateTimeOutFunction");
    //StateMinTimes[currentState]+StateLeadinTimes[currentState]
    unsigned long currentTime = micros();


    if (TransitionStartMicros + StateLeadinTimes[currentState] + StateMinTimes[currentState] + StateAutoTransitionTimes[currentState] < currentTime ) {
        return 0; // past auto transition
    } else if (TransitionStartMicros + StateLeadinTimes[currentState] + StateMinTimes[currentState] < currentTime) {
        return 1; // past min
    } else {
        return -1;// in transion, do not change
    }
}

typedef int (StateMachineClass::*IntFunctionWithOneParameter) (int a);
const IntFunctionWithOneParameter ContinueState [numberofStates] = {
/* 0 -- Safe State */   &StateMachineClass::defaultStateTimeOutFunction,
/* 1 -- Retract P */    &StateMachineClass::defaultStateTimeOutFunction,
/* 2 -- Retract E */    &StateMachineClass::defaultStateTimeOutFunction,
/* 3 -- RestFull */     &StateMachineClass::defaultStateTimeOutFunction,
/* 4 -- Arming */       &StateMachineClass::defaultStateTimeOutFunction,
/* 5 -- ready */        &StateMachineClass::defaultStateTimeOutFunction,
/* 6 -- waiting */      &StateMachineClass::defaultStateTimeOutFunction,
/* 7 -- Fire */         &StateMachineClass::defaultStateTimeOutFunction,
/* 8 -- Fired */        &StateMachineClass::defaultStateTimeOutFunction,
};


StateMachineClass::StateMachineClass() {
}

void StateMachineClass::EnableStateMachine( ) {
    for (int valveII = 0; valveII < N_VALVES; valveII++) {
        //digitalWrite(ValvePins[valveII],ValveState[NewState][valveII]);
        pinMode(ValvePins[valveII],OUTPUT);
    }
//for( int valveII = 0; valveII < N_VALVES; valveII += 1 ) {
//
//    }
}

void StateMachineClass::QuickdebugFunction(void) {
    telemetry_print("<TelemPacket><State=%d></ TelemPacket>", currentState);
}

void StateMachineClass::debugFunction(void) {

    telemetry_print("StateMachineClass::debugFunction"
                    "<TelemPacket>"
                    "<State=%d>"
                    "<TransitionState=%d>"
                    "<happyMachine=%d>"
                    "<Sensor1=%f>"
                    "<Sensor2=%f>",
                    currentState, TransitionState, happyMachine, Sensor1, Sensor2);
    for (int valveII=0; valveII<N_VALVES; valveII++) {
        telemetry_print("<ValveII-%d=%d>", valveII, ValveState[currentState][valveII]);
    }
    telemetry_print("</ TelemPacket>\n");
}

float readPressureSensor(int pin){

    int val = analogRead(2);
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
            unsigned tmpmillis=millis();
            if ( (tmpmillis >telemMillisRefreshRate) and ( telemMillis < tmpmillis-telemMillisRefreshRate)){

                debug_println("StateMachineClass::tickFunction Telemetry mode code goes here");

                Sensor1=readPressureSensor(2);
                Sensor2=readPressureSensor(3);

                debugFunction();

                telemMillis=millis();
            }
        }
    }

    if(TransitionState!=0){
        int tmpContInfo=(this->*ContinueState[currentState])(0);
    //    printer->print("StateMachineClass::tickFunction transition test ");
    //    printer->println(tmpContInfo);

        if (tmpContInfo==0)
        {
            //if (TransitionState<1){
                debug_println("StateMachineClass::tickFunction now at State %d %s",
                              currentState, StateNames[currentState]);
            //}
            TransitionState=0;

            if (AutoTransitionTo[currentState]>-1){
                int gotostate=AutoTransitionTo[currentState];

                debug_println("StateMachineClass::tickFunction going to State %d %s",
                              gotostate, StateNames[gotostate]);

                setMachineState(gotostate);
            }
        }else if (tmpContInfo==1){
            if (TransitionState!=1){
                debug_println("StateMachineClass::tickFunction passed minimum time in state: %d %s",
                              currentState, StateNames[currentState]);
            }

            TransitionState=1;
        }
    }
}


void StateMachineClass::externalRequest( int NewState ) {
    if (NewState==0) {
        setMachineState ( NewState );
    } else {
        // this is worth considering, maybe via some kind of merge of updateSwitches
    }
}

void StateMachineClass::setMachineState( int NewState ) {
    int PinVal;

    for (int valveII=0; valveII<N_VALVES; valveII++) {
        //digitalWrite(valveII,ValveState[NewState][valveII]);
        PinVal=ValveState[NewState][valveII];
        if (reversvalue[valveII]==1) {
            if (PinVal==1) {PinVal=0;}else{PinVal=1;}
        }
        digitalWrite(ValvePins[valveII],PinVal);
    }

    if (NewState != currentState) {
        // debug_println("StateMachineClass::setMachineState going to %d %s",
        //               NetState, StateNames[NewState]);
        QuickdebugFunction();

        currentState=NewState;
        TransitionState=-1;
        TransitionStartMicros=micros();
    }
}



void StateMachineClass::updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5) {
    //debug_println("StateMachineClass");


    //switch1 0: safe            1: rest           2:
    //switch2 0: rest            1: single fire    2: multi fire
    //switch3 0: short retract   1: long retract   2: retract now
    //switch4 0: open loopA      1: open loopB     2: closed
    //switch5 0: rest            1:                2: fire

    if (switch1==0) {
        setMachineState(0);
        if (happyMachine!=1 and switch1==0 and switch2==0 and (switch3==0 or switch3==1) and switch5==0){
            debug_println("now I'm happy! :)");
            setMachineState(STATE_SAFE);
            happyMachine=1;
        }
    } else if(TransitionState<0) {
        //debug_println("mid Transition");

    } else if (happyMachine!=1) {
        debug_println("not happy!");
    } else if (switch1>0) {
    // normal active mode
    //4568
        if (currentState==STATE_SAFE or currentState==STATE_EXPAND) {
            // go to rest from safe.
            if (switch2==0 and (switch3==0 or switch3==1) and switch5==0){
                // switch 4 is which control mode we are in so it doesn't matter.
                // so does pot
                // so does switch3=0 or 1 but switch3=2 does do something so don't let that through
                debug_println("we can go to state 3 (Rest) now");
                setMachineState(STATE_REST);
            } else {
                if (currentState==STATE_SAFE) {
                    debug_println("not going to rest due to other switches");
                } else if (currentState==STATE_EXPAND) {
                    if (switch2==2 and (switch3==0 or switch3==1) and switch5==0) {
                        //if switch3==1 then the extra time in two is in the min time so we don't get here until the extended min time has been used.
                        setMachineState(4);

                    }
                }
            }
        } else if (currentState==STATE_ARMING or currentState==STATE_READY_TO_FIRE or currentState==STATE_WAITING_TO_FIRE or currentState==STATE_FIRED) {
            // if in state 2 (expand) or 8(fired) but less than min time then we will be in TransitionState and not get here
            if (switch2==0) {
                setMachineState(STATE_REST);
            } else if (switch2>0) {

                if (currentState==STATE_READY_TO_FIRE or currentState==STATE_WAITING_TO_FIRE) {
                    if (switch5==2){
                        setMachineState(STATE_FIRE);
                    }
                }
            }
        } else if (currentState==STATE_REST and TransitionState>-1) {
        // at rest
            if (switch3==2 and switch2==0 ) {
                // active retract from rest

                setMachineState(STATE_RETRACT);
            } else if ( switch3 < 2 and switch5==0 and switch2==1) {

                setMachineState(STATE_ARMING);
            }


        } else {
            //setMachineState(STATE_SAFE);
        }
    }
    if (switch1==2){
        TelemetryMode=1;
        //debug_println("TelemetryMode=1;");
    }

}
