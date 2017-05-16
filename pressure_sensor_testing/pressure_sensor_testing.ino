
void setup() {
    Serial.begin(115200);
}

//Press1: analog in 3
//Press2: analog in 2

void loop() {

    int val = analogRead(2);
    // 5 122
    // 2.5 96
    // minimum seen: 20
    float voltage = (float)val / 1024 * 3.3;
    float pressure = voltage * 3.2 / 1 /10 * 60;
    Serial.print("analog 0 is: ");
    Serial.print(val);
    Serial.print(" ");
    Serial.println(pressure);
    delay(250);
}
