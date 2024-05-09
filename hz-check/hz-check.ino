#include <Arduino.h>
#include "heltec.h" // Include Heltec ESP32 library

const int LDRPin = 7; // LDR connected to GPIO 34
unsigned long startTime = millis(); // Start time for calibration
const long calibrationPeriod = 5000; // 15 seconds for calibration
bool isCalibrated = false; // Flag for calibration completion
int minValue = 4095; // Maximum possible for ESP32 ADC
int maxValue = 0; // Minimum possible for ESP32 ADC
int lightThreshold; // Threshold for light detection
int lastState = LOW; // Last state recorded
unsigned long lastBlinkTime = 0; // Last time a blink was detected
float frequencySum = 0; // Sum of frequencies measured
int frequencyCount = 0; // Count of frequency measurements
unsigned long lastUpdateTime = 0; // Last time the frequency was updated
const long updateInterval = 10000; // 10 seconds for average frequency calculation
float lastFrequency = 0; // Last measured frequency
float averageFrequency = 0;

void setup() {
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/, false /*PABOOST Enable*/, 868E6 /*long BAND*/);
  pinMode(LDRPin, INPUT);
}

void loop() {
  if (!isCalibrated) {
    calibrateSensor();
    displayStatus("Calibrating...", analogRead(LDRPin), lightThreshold, lastFrequency, calculateAverageFrequency());
    return; // Wait until calibration is complete
  } else {
    detectBlinkAndUpdateFrequency();
    // Every 10 seconds, calculate and display the average frequency
    if (millis() - lastUpdateTime >= updateInterval) {
      averageFrequency = calculateAverageFrequency();
      frequencySum = 0; // Reset the sum of frequencies
      frequencyCount = 0; // Reset the count of frequencies
      lastUpdateTime = millis(); // Update the last update time
    }
    displayStatus("Detecting...", analogRead(LDRPin), lightThreshold, lastFrequency, averageFrequency);
  }
}

void calibrateSensor() {
  unsigned long currentTime = millis();
  if (currentTime - startTime < calibrationPeriod) {
    int lightValue = analogRead(LDRPin);
    if (lightValue < minValue) {
      minValue = lightValue;
    }
    if (lightValue > maxValue) {
      maxValue = lightValue;
    }
  } else {
    lightThreshold = (minValue + maxValue) / 2; // Calculate the threshold
    isCalibrated = true;
  }
}

void detectBlinkAndUpdateFrequency() {
  int lightValue = analogRead(LDRPin);
  int currentState = (lightValue < lightThreshold) ? HIGH : LOW;
  if (lastState == LOW && currentState == HIGH) { // On rising edge
    unsigned long currentTime = millis();
    if (lastBlinkTime > 0) {
      unsigned long period = currentTime - lastBlinkTime;
      lastFrequency = 1000.0 / period; // Calculate frequency in Hz
      frequencySum += lastFrequency; // Add to the sum of frequencies
      frequencyCount++; // Increment the count of frequencies
    }
    lastBlinkTime = currentTime; // Update last blink time
  }
  lastState = currentState; // Update last state
}

float calculateAverageFrequency() {
  if (frequencyCount == 0) return 0; // Avoid division by zero
  return frequencySum / frequencyCount; // Calculate average frequency
}

void displayStatus(String mode, int currentLight, int threshold, float frequency, float averageFrequency) {
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, mode);
  Heltec.display->drawString(0, 15, "Light: " + String(currentLight));
  Heltec.display->drawString(0, 30, "Threshold: " + String(threshold));
  Heltec.display->drawString(0, 45, "Freq: " + String(frequency, 2) + " Hz");
  Heltec.display->drawString(70, 45, "Avg: " + String(averageFrequency, 2) + " Hz");
  Heltec.display->display();
}
