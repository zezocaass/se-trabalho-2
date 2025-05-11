#include <Servo.h>

#define ENCODER_CLK 7
#define ENCODER_DT  A2
#define SERVO_PIN 10

#define SERVO_OPEN 90
#define SERVO_CLOSED 0

Servo gateServo;
int lastEncoderCLK = 0;

void setup() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  gateServo.attach(SERVO_PIN);
  gateServo.write(SERVO_CLOSED); 
  lastEncoderCLK = digitalRead(ENCODER_CLK);
}

void loop() {
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastEncoderCLK && currentCLK == LOW) {
    int dtValue = digitalRead(ENCODER_DT);
    if (dtValue == HIGH) {
      // Gira para um lado: abre totalmente
      gateServo.write(SERVO_OPEN);
    } else {
      // Gira para o outro lado: fecha totalmente
      gateServo.write(SERVO_CLOSED);
    }
    delay(200);
  }
  lastEncoderCLK = currentCLK;
}
