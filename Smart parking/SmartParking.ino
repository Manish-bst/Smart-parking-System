#include <Servo.h>
#include <LiquidCrystal.h>

// Pin configuration
const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 6;

// IR Sensor pins
const int irSensor1Pin = 2;
const int irSensor2Pin = 3;

// LED pins
const int greenLed1Pin = 4;
const int redLed1Pin = 5;
const int greenLed2Pin = 7;
const int redLed2Pin = 8;

// LCD pins
const int lcdRsPin = 12;
const int lcdEnPin = 11;
const int lcdD4Pin = A0;
const int lcdD5Pin = A1;
const int lcdD6Pin = A2;
const int lcdD7Pin = A3;

// Set up the servo motor
Servo myServo;

// Set up the LCD
LiquidCrystal lcd(lcdRsPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

// Variables for ultrasonic sensor
long duration;
int distance;

// Variables for IR sensors
int irSensor1State = 0;
int irSensor2State = 0;
int irSensor1LastState = HIGH;  // Last state of IR sensor 1 (initially no vehicle)
int irSensor2LastState = HIGH;  // Last state of IR sensor 2 (initially no vehicle)

// Variables for slots
int totalSlots = 2;
int availableSlots = totalSlots;

void setup() {
  // Initialize the servo motor
  myServo.attach(servoPin);
  
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize IR sensor pins
  pinMode(irSensor1Pin, INPUT);
  pinMode(irSensor2Pin, INPUT);
  
  // Initialize LED pins
  pinMode(greenLed1Pin, OUTPUT);
  pinMode(redLed1Pin, OUTPUT);
  pinMode(greenLed2Pin, OUTPUT);
  pinMode(redLed2Pin, OUTPUT);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Smart Parking System");
  lcd.setCursor(0, 1);
  lcd.print("Slots Engaged: ");
  lcd.print(availableSlots);
  
  // Start serial communication for debugging
  Serial.begin(9600);
  
  // Initially set the servo to closed position (0 degrees)
  myServo.write(0);
}

void loop() {
  // Send a pulse to trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the pulse from the echo pin
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in cm
  distance = duration * 0.034 / 2;
  
  // Print distance to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // If distance is less than a threshold (e.g., 17 cm) and available slots are greater than 0, open the gate (servo motor)
  if (distance < 17 && availableSlots > 0) {
    // Open the gate (servo at 90 degrees)
    myServo.write(90);
  } else {
    // Close the gate (servo at 0 degrees)
    myServo.write(0);
  }

  // IR Sensor 1 logic: if detected (LOW), turn on green LED, else red LED
  irSensor1State = digitalRead(irSensor1Pin);
  if (irSensor1State == LOW && irSensor1LastState == HIGH) {
    // A vehicle just entered the parking slot
    if (availableSlots > 0) {
      availableSlots--;
    }
    digitalWrite(greenLed1Pin, HIGH);  // Turn on green LED
    digitalWrite(redLed1Pin, LOW);     // Turn off red LED
  } else if (irSensor1State == HIGH && irSensor1LastState == LOW) {
    // A vehicle just exited the parking slot
    if (availableSlots < totalSlots) {
      availableSlots++;
    }
    digitalWrite(greenLed1Pin, LOW);   // Turn off green LED
    digitalWrite(redLed1Pin, HIGH);    // Turn on red LED
  }
  irSensor1LastState = irSensor1State; // Update the last state

  // IR Sensor 2 logic: if detected (LOW), turn on green LED, else red LED
irSensor2State = digitalRead(irSensor2Pin);
if (irSensor2State == LOW && irSensor2LastState == HIGH) {
  // A vehicle just entered the parking slot
  if (availableSlots > 0) {
    availableSlots--;
  }
  digitalWrite(greenLed2Pin, HIGH);  // Turn on green LED
  digitalWrite(redLed2Pin, LOW);     // Turn off red LED
} else if (irSensor2State == HIGH && irSensor2LastState == LOW) {
  // A vehicle just exited the parking slot
  if (availableSlots < totalSlots) {
    availableSlots++;
  }
  digitalWrite(greenLed2Pin, LOW);   // Turn off green LED
  digitalWrite(redLed2Pin, HIGH);    // Turn on red LED
}
irSensor2LastState = irSensor2State; // Update the last state
  // Ensure availableSlots is within valid range
  if (availableSlots < 0) {
    availableSlots = 0;
  } else if (availableSlots > totalSlots) {
    availableSlots = totalSlots;
  }

  // Update the LCD display with the current number of available slots
  lcd.setCursor(0, 1);
  lcd.print("Slots Available: ");
  lcd.print(availableSlots);
  lcd.print("   "); // Clear any remaining characters

  // Optional: Add a safety check to prevent the servo from moving too quickly
  if (distance < 17 && myServo.read() == 0 && availableSlots > 0) {
    myServo.write(90);
  } else if (distance >= 17 && myServo.read() == 90) {
    myServo.write(0);
  }

  // Delay before the next reading
  delay(500);
}
