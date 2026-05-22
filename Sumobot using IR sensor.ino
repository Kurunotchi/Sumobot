/**
 * @file Sumobot using IR sensor.ino
 * @brief Autonomous Sumo Robot code using a single IR sensor for ring edge detection.
 * @author Kurunotchi
 * @date 2026-05-22 (Refactored)
 * 
 * Hardware Connections (Arduino Nano):
 * - IR Line Sensor: Pin A0 (Digital Input)
 * - L298N Motor Driver Inputs:
 *   - IN1: Pin 5 (Left Motor Forward)
 *   - IN2: Pin 6 (Left Motor Backward)
 *   - IN3: Pin 7 (Right Motor Forward)
 *   - IN4: Pin 8 (Right Motor Backward)
 * 
 * Note: ENA and ENB jumpers on the L298N driver should be connected to run motors at full speed.
 */

// --- Pin Definitions ---
const int PIN_IR_SENSOR = A0; ///< Analog pin used as digital input for line sensor

// L298N Motor Driver Control Pins
const int PIN_MOTOR_IN1 = 5; ///< Left Motor Control 1
const int PIN_MOTOR_IN2 = 6; ///< Left Motor Control 2
const int PIN_MOTOR_IN3 = 7; ///< Right Motor Control 1
const int PIN_MOTOR_IN4 = 8; ///< Right Motor Control 2

// --- Sensor State Constants ---
const int LINE_DETECTED = HIGH;   ///< Sensor reads HIGH on the black ring border
const int SURFACE_WHITE = LOW;    ///< Sensor reads LOW on the white inside surface

// --- Configuration Constants ---
const unsigned long ACTION_DELAY_MS = 800; ///< Delay for turn and reverse maneuvers
const int MAX_LINE_COUNT = 6;              ///< Reset count after this many line detections

// --- Global Variables ---
int darklineCount = 0;   ///< Tracks consecutive line detections to alternate turn directions
int previousValue = SURFACE_WHITE; ///< Stores the previous sensor reading for edge detection

// --- Function Prototypes ---
void moveForward();
void turnRight();
void turnLeft();
void stopMotors();

/**
 * @brief Initialize pins and serial communication.
 */
void setup() {
  // Initialize motor control pins as outputs
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_MOTOR_IN3, OUTPUT);
  pinMode(PIN_MOTOR_IN4, OUTPUT);
  
  // Initialize IR sensor pin as input
  pinMode(PIN_IR_SENSOR, INPUT);
  
  // Initialize serial monitor for debugging
  Serial.begin(9600);
  Serial.println(F("Sumobot IR Sensor Initialized. Ready for battle!"));
}

/**
 * @brief Main execution loop.
 */
void loop() {
  // Read current status of the IR sensor
  int currentValue = digitalRead(PIN_IR_SENSOR);
  
  // Log the IR sensor state to the serial console
  Serial.print(F("IR Sensor Value: "));
  Serial.println(currentValue == LINE_DETECTED ? F("BLACK LINE") : F("WHITE SURFACE"));

  // Check for transition from White Surface to Black Line (Rising Edge)
  if (currentValue == LINE_DETECTED && previousValue == SURFACE_WHITE) {
    darklineCount++;
    Serial.print(F("Black Line Detected! Count: "));
    Serial.println(darklineCount);

    // Alternate directions based on detection count
    if (darklineCount >= 1 && darklineCount <= 3) {
      turnLeft();
    } else if (darklineCount >= 4 && darklineCount <= 6) {
      turnRight();
    }
    
    // Reset counter once we complete a full cycle of turns
    if (darklineCount >= MAX_LINE_COUNT) {
      darklineCount = 0;
      Serial.println(F("Resetting Dark Line Count."));
    }
  }

  // Save current value for the next loop iteration
  previousValue = currentValue;

  // Move forward if still inside the ring
  if (currentValue == SURFACE_WHITE) {
    moveForward();
  }
}

/**
 * @brief Drive the robot straight forward.
 */
void moveForward() {
  Serial.println(F("Moving Forward..."));
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}

/**
 * @brief Move backwards and turn right to recover from the ring edge.
 */
void turnRight() {
  Serial.println(F("Action: Recovering (Backwards + Turn Right)"));

  // Step 1: Move backwards away from the border
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(ACTION_DELAY_MS);

  // Step 2: Spin clockwise (right motor backward, left motor forward)
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(ACTION_DELAY_MS);
}

/**
 * @brief Move backwards and turn left to recover from the ring edge.
 */
void turnLeft() {
  Serial.println(F("Action: Recovering (Backwards + Turn Left)"));

  // Step 1: Move backwards away from the border
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(ACTION_DELAY_MS);

  // Step 2: Spin counter-clockwise (left motor backward, right motor forward)
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(ACTION_DELAY_MS);
}

/**
 * @brief Stop both motors immediately.
 */
void stopMotors() {
  Serial.println(F("Stopping motors"));
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}
