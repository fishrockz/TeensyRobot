#include "StateMachine.h"




void StateMachineClass::foo()
{
}


StateMachineClass::StateMachineClass( usb_serial_class &print ) { 
      printer = &print; //operate on the adress of print
}  


void StateMachineClass::updateSwitches(int switch1, int switch2, int switch3, int switch4){
	printer->println("StateMachineClass");
	
	
	if (DebugLevel>10){printer->print(switch1);printer->print(" ");printer->print(switch2);printer->print(" ");printer->print(switch3);printer->print(" ");printer->println(switch4);}
	

}
