
const uint32_t ValveAPin = 3;
const uint32_t ValveBPin = 4;
const uint32_t ValveCPin = 5;
const uint32_t ValveDPin = 6;
const uint32_t ValveEPin = 7;

void setup() {
  // put your setup code here, to run once:
if (0==1){
	digitalWrite(ValveAPin,HIGH); // this should be true anyway.
	digitalWrite(ValveBPin,HIGH);
	digitalWrite(ValveCPin,HIGH);
	digitalWrite(ValveDPin,HIGH);
	digitalWrite(ValveEPin,HIGH);
}else if (1==0){
	digitalWrite(ValveAPin,LOW); // this should be true anyway.
	digitalWrite(ValveBPin,LOW);
	digitalWrite(ValveCPin,LOW);
	digitalWrite(ValveDPin,LOW);
	digitalWrite(ValveEPin,LOW);
}


	pinMode(ValveAPin,OUTPUT); 
	pinMode(ValveBPin,OUTPUT); 
	pinMode(ValveCPin,OUTPUT); 
	pinMode(ValveDPin,OUTPUT); 
	pinMode(ValveEPin,OUTPUT); 
delay(100);
	digitalWrite(ValveAPin,HIGH); // this should be true anyway.
	digitalWrite(ValveBPin,HIGH);
	digitalWrite(ValveCPin,HIGH);
	digitalWrite(ValveDPin,HIGH);
	digitalWrite(ValveEPin,HIGH);


}

void loop() {
  // put your main code here, to run repeatedly:

}
