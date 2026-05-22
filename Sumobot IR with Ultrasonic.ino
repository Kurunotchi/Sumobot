/**
 * @file Sumobot IR with Ultrasonic.ino
 * @brief Autonomous Sumo Robot code using an IR sensor for ring edge detection
 *        and an Ultrasonic sensor for opponent detection and tracking.
 * @author Kurunotchi
 * @date 2026-05-22 (Refactored)
 * 
 * Hardware Connections (Arduino Nano):
 * - IR Line Sensor: Pin A0 (Digital Input)
 * - Ultrasonic Sensor:
 *   - Trig Pin: Pin 5
 *   - Echo Pin: Pin 6
 * - L298N Motor Driver Control Pins:
 *   - IN1: Pin 7 (Left Motor Control 1)
 *   - IN2: Pin 8 (Left Motor Control 2)
 *   - IN3: Pin 9 (Right Motor Control 1)
 *   - IN4: Pin 10 (Right Motor Control 2)
 *   - ENA: Pin 2 (Left Motor Speed Enable, PWM)
 *   - ENB: Pin 3 (Right Motor Speed Enable, PWM)
 */

// --- Pin Definitions ---
const int PIN_IR_SENSOR = A0;   ///< Analog pin used as digital input for line sensor
const int PIN_US_TRIG   = 5;    ///< Ultrasonic Trigger Pin
const int PIN_US_ECHO   = 6;    ///< Ultrasonic Echo Pin

// L298N Motor Control Pins
const int PIN_MOTOR_IN1 = 7;    ///< Left Motor Input 1
const int PIN_MOTOR_IN2 = 8;    ///< Left Motor Input 2
const int PIN_MOTOR_IN3 = 9;    ///< Right Motor Input 1
const int PIN_MOTOR_IN4 = 10;   ///< Right Motor Input 2
const int PIN_MOTOR_ENA = 2;    ///< Left Motor Speed (PWM)
const int PIN_MOTOR_ENB = 3;    ///< Right Motor Speed (PWM)

// --- Sensor State Constants ---
const int LINE_DETECTED = HIGH;   ///< Sensor reads HIGH on the black ring border
const int SURFACE_WHITE = LOW;    ///< Sensor reads LOW on the white inside surface

// --- Speed Configurations (0 to 255) ---
const int SPEED_CRUISE  = 150;    ///< Motor speed when searching/cruising
const int SPEED_TURN    = 255;    ///< Motor speed during turn recovery
const int SPEED_ATTACK  = 255;    ///< Motor speed during opponent charge

// --- Tactical Configurations ---
const int OPPONENT_THRESHOLD_CM = 15;   ///< Detection threshold for opponent (in cm)
const unsigned long ACTION_DELAY_MS = 800; ///< Recovery turn and backing duration (in ms)
const int MAX_LINE_COUNT = 6;            ///< Reset count after this many line detections
const bool BACKUP_BEFORE_PUSH = false;   ///< Set to true to back up before charging (legacy behavior)

// --- Global Variables ---
int darklineCount = 0;                  ///< Tracks consecutive line detections to alternate turn directions
int previousValue = SURFACE_WHITE;       ///< Stores the previous sensor reading for edge detection

// --- Function Prototypes ---
void moveForward();
void moveBackwards();
void turnRight();
void turnLeft();
void pushOpponent(int speed);
void stopMotors();
long readDistanceCm();

/**
 * @brief Initialize pins, serial communication, and setup start-up feedback.
 */
void setup() {
  // Ultrasonic Pin Configuration
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);

  // Motor Driver Control Pin Configuration
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_MOTOR_IN3, OUTPUT);
  pinMode(PIN_MOTOR_IN4, OUTPUT);
  
  pinMode(PIN_MOTOR_ENA, OUTPUT);
  pinMode(PIN_MOTOR_ENB, OUTPUT);
  
  // Line Sensor Pin Configuration
  pinMode(PIN_IR_SENSOR, INPUT);
  
  // Debug Serial Setup
  Serial.begin(9600);
  Serial.println(F("Sumobot IR & Ultrasonic Initialized. Ready for battle!"));
}

/**
 * @brief Main execution loop containing the sumobot state logic.
 */
void loop() {
  // Read sensors
  int lineState = digitalRead(PIN_IR_SENSOR);
  long distance = readDistanceCm();

  // Print line status to serial interface
  Serial.print(F("IR Sensor Value: "));
  Serial.print(lineState);
  Serial.print(F(" ("));
  Serial.print(lineState == LINE_DETECTED ? F("BLACK LINE") : F("WHITE SURFACE"));
  Serial.println(F(")"));

  // Check for White-to-Black border transition (Rising Edge)
  if (lineState == LINE_DETECTED && previousValue == SURFACE_WHITE) {
    darklineCount++;
    Serial.print(F("Black Line Detected! Count: "));
    Serial.println(darklineCount);

    // Alternate recovery directions based on detection count
    if (darklineCount >= 1 && darklineCount <= 3) {
      Serial.println(F("Recovering: Turning Left..."));
      turnLeft();
    } else if (darklineCount >= 4 && darklineCount <= 6) {
      Serial.println(F("Recovering: Turning Right..."));
      turnRight();
    }
    
    // Reset counter once we complete a full cycle of turns
    if (darklineCount >= MAX_LINE_COUNT) {
      darklineCount = 0;
      Serial.println(F("Resetting Dark Line Count."));
    }
    
    // Save line state and exit early to avoid executing movements on stale distance reading
    previousValue = LINE_DETECTED;
    return;
  }

  // Update previous state value
  previousValue = lineState;

  // If safe inside the ring, proceed with search or attack
  if (lineState == SURFACE_WHITE) {
    if (distance > 0 && distance < OPPONENT_THRESHOLD_CM) {
      Serial.print(F("Opponent detected at: "));
      Serial.print(distance);
      Serial.println(F(" cm! Charging!"));
      pushOpponent(SPEED_ATTACK);
    } else {
      moveForward();
    }
  } else {
    // Safety fallback: if we are still resting on the black line, halt to prevent driving out
    Serial.println(F("Warning: Stuck on line. Stopping motors."));
    stopMotors();
  }
}

/**
 * @brief Measure the distance to the closest object in front of the robot.
 * @return Distance in centimeters, or 999 if no object is within range (timeout).
 */
long readDistanceCm() {
  // Send a clean 10-microsecond HIGH pulse to trigger the sensor
  digitalWrite(PIN_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  // Read the return echo pulse. Timeout in 30,000 microseconds (~5 meters max)
  long duration = pulseIn(PIN_US_ECHO, HIGH, 30000);

  // duration is 0 if no pulse is received (timeout)
  if (duration == 0) {
    return 999; 
  }

  // Calculate distance: speed of sound is 343 m/s or 0.0343 cm/us
  long distanceCm = (duration * 0.0343) / 2;
  return distanceCm;
}

/**
 * @brief Drive the robot straight forward at cruise speed.
 */
void moveForward() {
  Serial.println(F("Moving Forward"));
  analogWrite(PIN_MOTOR_ENA, SPEED_CRUISE);  
  analogWrite(PIN_MOTOR_ENB, SPEED_CRUISE);  
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
}

/**
 * @brief Drive both motors in reverse at full speed.
 */
void moveBackwards() {
  Serial.println(F("Moving Backwards"));
  analogWrite(PIN_MOTOR_ENA, SPEED_TURN); 
  analogWrite(PIN_MOTOR_ENB, SPEED_TURN);  
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}

/**
 * @brief Back up and pivot right.
 */
void turnRight() {
  // Step 1: Move backwards away from the border
  moveBackwards();
  delay(ACTION_DELAY_MS);
  
  // Step 2: Pivot right (left motor forward, right motor backward)
  Serial.println(F("Pivoting Right"));
  analogWrite(PIN_MOTOR_ENA, SPEED_TURN); 
  analogWrite(PIN_MOTOR_ENB, SPEED_TURN);  
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(ACTION_DELAY_MS);
}

/**
 * @brief Back up and pivot left.
 */
void turnLeft() {
  // Step 1: Move backwards away from the border
  moveBackwards();
  delay(ACTION_DELAY_MS);
  
  // Step 2: Pivot left (left motor backward, right motor forward)
  Serial.println(F("Pivoting Left"));
  analogWrite(PIN_MOTOR_ENA, SPEED_TURN);  
  analogWrite(PIN_MOTOR_ENB, SPEED_TURN);  
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(ACTION_DELAY_MS);
}

/**
 * @brief Engage and push the opponent.
 * @param speed Speed level (0 - 255) to drive the motors.
 */
void pushOpponent(int speed) {
  if (BACKUP_BEFORE_PUSH) {
    // Legacy wind-up maneuver
    Serial.println(F("Pushing Opponent: Backing up first..."));
    moveBackwards();
    delay(ACTION_DELAY_MS);
  }

  // Charge forward at specified speed
  Serial.println(F("Pushing Opponent: Charging forward!"));
  analogWrite(PIN_MOTOR_ENA, speed);  
  analogWrite(PIN_MOTOR_ENB, speed);  
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
}

/**
 * @brief Stop both motors immediately.
 */
void stopMotors() {
  Serial.println(F("Stopping motors"));
  analogWrite(PIN_MOTOR_ENA, 0); 
  analogWrite(PIN_MOTOR_ENB, 0);  
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}
