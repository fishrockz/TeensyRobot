const int numberOfValues = 8;
/* wills purple */
const int ValvePins[numberOfValues] = {
/*Valve A*/  23,
/*Valve B*/  22,
/*Valve C*/  21,
/*Valve D*/  5,
/*Valve E*/  6,
/*Valve A2*/  37,
 /*Aux1 */    36,
 /*Aux1 */    38,
};
int LEDpin =13;
void setup() {
  // put your setup code here, to run once:


	for ( int valveII;valveII< numberOfValues;valveII++){
		pinMode(valveII,OUTPUT); 
	}
	pinMode(LEDpin,OUTPUT); 
	

	for ( int valveII;valveII< numberOfValues;valveII++){
		digitalWrite(valveII,HIGH); 
	}
	digitalWrite(LEDpin,HIGH); 

}

void loop() {
  // put your main code here, to run repeatedly:


	for ( int valveII;valveII< numberOfValues;valveII++){
		digitalWrite(valveII,HIGH); 
	}
	digitalWrite(LEDpin,HIGH); 
	delay(1000);

	for ( int valveII;valveII< numberOfValues;valveII++){
		digitalWrite(valveII,LOW); 
	}
	digitalWrite(LEDpin,LOW); 
	delay(1000);
}
