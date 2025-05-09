#include <Servo.h>

#define TRIG_PIN 11
#define ECHO_PIN 12
#define SERVO_PIN 10

const int DISTANCE_THRESHOLD = 30; // cm para abrir portão

Servo gateServo;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Portão fechado inicialmente
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  long distance = readDistance();
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < DISTANCE_THRESHOLD) {
    gateServo.write(90); // Abre portão
  } else {
    gateServo.write(0);  // Fecha portão
  }
  delay(200);
}
