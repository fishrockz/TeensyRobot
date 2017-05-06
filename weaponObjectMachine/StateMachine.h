
#include "Arduino.h"
#include <usb_serial.h>

class StateMachineClass
{
public:
  void foo();
  int bar;
  void updateSwitches(int switch1, int switch2, int switch3, int switch4); 
  StateMachineClass( usb_serial_class &print );

  
  private:
    usb_serial_class* printer;  
  int DebugLevel=22;
  void setMachineState(int NewState);
  int currentState;
};
