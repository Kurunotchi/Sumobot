//Sumobot code by Kurunotchi
/* 20/03/2025 */

//Components

//Arduino nano
//Motor driver - L298n
//Dc geared motor
//Battery - 18650
//IR Sensor
//Switch
//Ultrasonic sensor if gusto niyo ng object or opponent detection

const int darkline = A0;

// Motor pins
int in1 = 5;
int in2 = 6;
int in3 = 7;
int in4 = 8;

int darklineCount = 0;
int previousValue = 0;

void turnRight();
void turnLeft();
void moveForward();
void rotate();

void setup() {
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  pinMode(darkline, INPUT);
  Serial.begin(9600);
}

void loop() {
  int value = digitalRead(darkline);
  Serial.print("IR Sensor Value: ");
  Serial.println(value);

  // Detect darkline
  if (value == 1 && previousValue == 0) {
    darklineCount++;
    Serial.print("Dark Line Count: ");
    Serial.println(darklineCount);

    if (darklineCount == 1 || darklineCount == 2 || darklineCount == 3) {
      turnLeft();
    } else if (darklineCount == 4 || darklineCount == 5 || darklineCount == 6) {
      turnRight();
    } 
    if (darklineCount == 6) {
      darklineCount = 0;
    }
  }

  previousValue = value;

  // Move forward if no dark line
  if (value == 0) {
    moveForward();
  }
}

void moveForward() {
  Serial.println("Moving Forward");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void turnRight() {
  Serial.println("Turning Right");

  // Reverse
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(800);

  // Turn right
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(800); // kayo na bahala mag adjust
}

void turnLeft() {
  Serial.println("Turning Left");

  // Reverse
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(800); // kayo na bahala mag adjust

  // Turn left
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(800); // kayo na bahala mag adjust
}

void rotate() {
  Serial.println("Revolve");

  // Reverse
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(200); // kayo na bahala mag adjust

  // Turn left
  analogWrite(in1, 100);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(800); // kayo na bahala mag adjust
}
