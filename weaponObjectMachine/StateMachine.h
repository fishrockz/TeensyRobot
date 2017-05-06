
#include "Arduino.h"
#include <usb_serial.h>

class StateMachineClass
{
public:

  int bar;
  void updateSwitches(int switch1, int switch2, int switch3, int switch4, int switch5); 
 int defaultStateTimeOutFunction(int input);
  StateMachineClass( usb_serial_class &print );
void tickFunction();
void  debugFunction();
  private:
    usb_serial_class* printer;  
  int DebugLevel=0;
  void setMachineState(int NewState);
  int currentState=0;
  int TransitionState=0;
  unsigned long TransitionStartMicros=0;
  int TelemiteryMode=0;
  unsigned long telemMillis=0;
  unsigned long telemMillisRefreshRate=1000;
};
