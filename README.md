# light-ambient

const int LDR_PIN = 34; // Define the pin connected to the LDR
const int LED_PIN = 12; // Define the pin connected to the LED
void setup() {
pinMode(LDR_PIN, INPUT);
pinMode(LED_PIN, OUTPUT);
}
void loop() {
int sensorValue = analogRead(LDR_PIN); // Read the LDR value (0-4095)
int brightness = map(sensorValue, 0, 4095, 0, 255); // Map to LED brightness
(0-255);
analogWrite(LED_PIN, brightness); // Set LED brightness
delay(100); // Add a small delay for stability
}

