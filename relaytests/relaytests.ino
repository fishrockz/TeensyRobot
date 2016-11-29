void setup() {
  // put your setup code here, to run once:

for (int iii=3;iii<10;iii++){
pinMode(iii,OUTPUT);
}
}

void loop() {
  // put your main code here, to run repeatedly:



for (int iii=3;iii<10;iii++){
digitalWrite(iii,1);
}
delay(1000);
for (int iii=3;iii<10;iii++){
digitalWrite(iii,0);
}
 delay(1000);

}
