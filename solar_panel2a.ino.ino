#include <Servo.h>  // Include the Servo library

// Pin definitions
const int voltagePin = A0;       // Voltage sensor pin
const int dustPin = A1;          // Dust sensor pin
const int rainPin = 2;           // Rain sensor pin (digital)
const int tempPin = A2;          // Temperature sensor pin
const int buzzerPin = 10;        // Buzzer pin for alert
const int servoPin = 9;          // Servo motor control pin

// Thresholds
const int thresholdVoltage = 20; // Threshold voltage in volts
const int dustThreshold = 1000;  // Dust threshold value
const int highTempThreshold = 60 ; // High temperature threshold in °C

Servo panelServo;                // Create a Servo object

void setup() {
  Serial.begin(9600);            // Start serial communication
  pinMode(rainPin, INPUT);       // Rain sensor as input
  pinMode(buzzerPin, OUTPUT);    // Buzzer pin as output
  panelServo.attach(servoPin);   // Attach servo to control pin

  panelServo.write(0);           // Start with the panel uncovered (position 0 degrees)
}

void loop() {
  int dustValue = analogRead(dustPin);    // Read dust sensor
  int rainStatus = digitalRead(rainPin);  // Read rain sensor
  int tempValue = analogRead(tempPin);    // Read temperature sensor
  int voltageValue = analogRead(voltagePin);  // Read voltage sensor
  
  // Map the voltage value to a voltage level (assuming a max of 25V from sensor)
  float voltage = map(voltageValue, 0, 1023, 0, 25);

  // Check thresholds and send messages
  if (voltage > thresholdVoltage) {
    triggerAlert("High Voltage Alert!");
  }

  if (dustValue > dustThreshold) {
    triggerAlert("High Dust Level Alert!");
    coverPanel();  // Cover the panel when dust level is high
  } else if (rainStatus == LOW) {  // LOW means rain detected
    triggerAlert("Rain Detected Alert!");
    coverPanel();  // Cover the panel when rain is detected
  } else {
    uncoverPanel();  // Uncover the panel when conditions are normal
  }

  float temperature = (tempValue / 1023.0) * 500;  // Convert to temperature in °C
  if (temperature > highTempThreshold) {
    triggerAlert("High Temperature Alert!");
  }

  // Send sensor data to NodeMCU
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print(" V, ");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, ");

  Serial.print("Rain Status: ");
  Serial.println(rainStatus == LOW ? "Raining" : "Dry");

  delay(1000);  // Delay for the next cycle
}

// Function to trigger an alert with the buzzer
void triggerAlert(String message) {
  Serial.println("ALERT: " + message);  // Send alert to NodeMCU
  for (int i = 0; i < 3; i++) {    // Buzzer alert
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }
}

// Function to cover the panel with the servo motor
void coverPanel() {
  panelServo.write(90);  // Move the servo to 90 degrees to cover the panel
  delay(500);            // Delay to ensure the servo completes the movement
}

// Function to uncover the panel with the servo motor
void uncoverPanel() {
  panelServo.write(0);   // Move the servo back to 0 degrees to uncover the panel
  delay(500);            // Delay to ensure the servo completes the movement
}
