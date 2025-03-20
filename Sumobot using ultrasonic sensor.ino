int trigpin = 2;
int echopin = 3;

int in1 = 4;
int in2 = 5;
int in3 = 6;
int in4 = 7;

void setup() {
    //Ultrasonic sensor
    pinMode(trigpin, OUTPUT); //trig 
    pinMode(echopin, INPUT); //echo
        //Motor driver
    pinMode(in1, OUTPUT); //left forward
    pinMode(in2, OUTPUT); //right forward
    pinMode(in3, OUTPUT); //left reverse  
    pinMode(in4, OUTPUT); //right reverse

    Serial.begin(9600);
}
void loop(){
    digitalWrite(trigpin, HIGH);
    
    digitalWrite(trigpin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigpin, HIGH);
    delayMicroseconds(10);

    digitalWrite(echopin, LOW);

    long distance;
    int duration;

    duration = pulseIn(echopin, HIGH);
    distance = (duration*0.0343)/2;

    if (distance > 50){
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    }else if (distance < 30){
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    }
    Serial.print("Distance: ");
    Serial.println(distance);
    delay(1000);
}
