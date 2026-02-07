#include <Arduino.h>

// On the Pico 2020, the onboard LED is GP25
#define LED_PIN 25

void setup() {
  // Initialize the pin and serial communication
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Turn LED on
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Pico Heartbeat: ON");
  delay(500); // Wait 0.5 seconds

  // Turn LED off
  digitalWrite(LED_PIN, LOW);
  Serial.println("Pico Heartbeat: OFF");
  delay(500); // Wait 0.5 seconds
}