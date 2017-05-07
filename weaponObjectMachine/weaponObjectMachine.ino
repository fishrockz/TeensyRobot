//Teensy libarys
#include <PulsePosition.h>

//Custom Robot Libarys
#include "weaponObjectMachine.h" 

const int DebugLevel=00;

const uint32_t PPMPin = 10;//only certain pins work, eg, 9 and 10 on teensy 3.1 and teensy 3.5
const uint32_t boardLEDPin = 13;




StateMachineClass theWeapon(Serial);
PulsePositionInput RadioIn;
void setup() {
  // put your setup code here, to run once:
	RadioIn.begin(PPMPin);
	theWeapon.EnableStateMachine();
	theWeapon.externalRequest(0);
	
}


int ValidPPMdata=-1;
int valRadio1,valRadio2,valRadio3,valRadio4;
int switch1,switch2,switch3,switch4,switch5;






int pot1;


void oneSwitchFromChan(int chanel,int &switchA){

	chanel-=1000;
	if (chanel<325){
		switchA=0;
	}else if (chanel<700) {
		switchA=1;
	} else {
		switchA=2;
	}

}

void twoSwitchesFromChan(int chanel,int &switchA,int &switchB) {
	

	chanel-=1000;
	if (chanel<325){
		switchA=0;
	}else if (chanel<700) {
		switchA=1;
	} else {
		switchA=2;
	}
	
	chanel-=switchA*350;
		
	if (chanel<100){
		switchB=0;
	}else if (chanel<250) {
		switchB=1;
	}else{
		switchB=2;
	}	


}


void loop() {
  // put your main code here, to run repeatedly:
	//theWeapon.foo();
	
	theWeapon.tickFunction();
	
	
	
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
	
		oneSwitchFromChan(valRadio4,switch5);
		twoSwitchesFromChan(valRadio2,switch1,switch2);
		twoSwitchesFromChan(valRadio3,switch3,switch4);
		
		if (DebugLevel>10){Serial.print(switch1);Serial.print(" ");Serial.print(switch2);Serial.print(" ");Serial.print(switch3);Serial.print(" ");Serial.print(switch4);Serial.print(" ");Serial.println(switch5);}
		
		theWeapon.updateSwitches(switch1,switch2,switch3,switch4,switch5);
		
		
		if (DebugLevel>10){Serial.print(pot1);Serial.print(" ");Serial.print(switch1);Serial.print(" ");Serial.print(switch2);Serial.print(" ");Serial.print(switch3);Serial.print(" ");Serial.println(switch4);}
	}
	
	
	
	
	
	
	
	
	
	
	
	
}
