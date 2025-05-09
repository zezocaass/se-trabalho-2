#include <Servo.h>

#define PUSH_BUTTON 8
#define SERVO_PIN 10

Servo gateServo;

void setup() {
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Portão fechado inicialmente
}

void loop() {
  if (digitalRead(PUSH_BUTTON) == LOW) {
    gateServo.write(90); // Abre portão
  } else {
    gateServo.write(0);  // Fecha portão
  }
  delay(100);
}
