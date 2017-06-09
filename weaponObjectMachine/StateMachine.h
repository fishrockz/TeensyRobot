#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "Arduino.h"
#include <usb_serial.h>
#include <HardwareSerial.h>

class StateMachineClass
{
    public:
    int bar;

    float Sensor1;
    float Sensor2;
    void updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5); 
    int defaultStateTimeOutFunction(int input);
    StateMachineClass();

    void tickFunction();
    void debugFunction();
    void QuickdebugFunction();
    void EnableStateMachine();
    void externalRequest(int NewState);

    private:
    // usb_serial_class* printer;  
    // HardwareSerial* printerHW; 

    int happyMachine=-1;
    int DebugLevel=22;
    int debugHW=1;
    void setMachineState(int NewState);
    int currentState=0;
    int TransitionState=0;
    uint32_t TransitionStartMicros=0;
    int TelemetryMode=1;
    uint32_t telemMillis=0;
    uint32_t telemMillisRefreshRate=1000;
};

#endif
