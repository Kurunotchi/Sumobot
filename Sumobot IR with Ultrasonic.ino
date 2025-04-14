// Sumobot code by Kurunotchi
/* 20/03/2025 */

// Components
// Arduino Nano
// Motor driver - L298n
// DC geared motor
// Battery - 18650
// IR Sensor
// Switch
// Ultrasonic sensor for opponent detection

#define darkline A5

int trigpin = 5;
int echopin = 6;

int in1 = 7;
int in2 = 8;
int in3 = 9;
int in4 = 10;

int ena = 2;
int enb = 3;

int darklineCount = 0;
int previousValue = 0;

void ultrasonic();
void turnRight();
void turnLeft();
void moveForward();
void moveBackwards();
void stopMotors();
void pushOpponent(int speed);

void setup() {
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  
  pinMode(darkline, INPUT);
  Serial.begin(9600);
  Serial.println("Sumobot Initialized.");
}

void loop() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  long duration = pulseIn(echopin, HIGH);
  long distance = (duration * 0.0343) / 2;

  int value = digitalRead(darkline);
  Serial.print("IR Sensor Value: ");
  Serial.println(value);
  
  if (value == 1 && previousValue == 0) {
    darklineCount++;
    Serial.print("Dark Line Count: ");
    Serial.println(darklineCount);
    delay(200);

    if (darklineCount == 1 || darklineCount == 2 || darklineCount == 3) {
      Serial.println("Turning Left due to Dark Line.");
      turnLeft();
    } else if (darklineCount == 4 || darklineCount == 5 || darklineCount == 6) {
      Serial.println("Turning Right due to Dark Line.");
      turnRight();
    }
    if (darklineCount == 6) {
      darklineCount = 0;
      Serial.println("Resetting Dark Line Count.");
    }
  }
  previousValue = value;

  if (value == 0 && distance > 15) {
    moveForward();
  }
  if (distance < 15) {
    Serial.print("Opponent detected at distance: ");
    Serial.println(distance);
    pushOpponent(255);
  }
}

void moveForward() {
  Serial.println("Moving Forward");
  analogWrite(ena, 150);  
  analogWrite(enb, 150);  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void moveBackwards() {
  Serial.println("Moving Backwards");
  analogWrite(ena, 255); 
  analogWrite(enb, 255);  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight() {
  Serial.println("Turning Right");
  analogWrite(ena, 255); 
  analogWrite(enb, 255);  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800);
}

void turnLeft() {
  Serial.println("Turning Left");
  analogWrite(ena, 255);  
  analogWrite(enb, 255);  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(800);
}

void pushOpponent(int speed) {
  Serial.println("Pushing opponent");
  analogWrite(ena, speed);  
  analogWrite(enb, speed);  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopMotors() {
  Serial.println("Stopping motors");
  analogWrite(ena, 0); 
  analogWrite(enb, 0);  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
