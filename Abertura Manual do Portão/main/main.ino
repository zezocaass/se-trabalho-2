#include <Servo.h>

#define ENCODER_CLK 7
#define ENCODER_DT  A2
#define SERVO_PIN 10

#define SERVO_OPEN 90
#define SERVO_CLOSED 0

Servo gateServo;
int lastEncoderCLK = 0;
int currentPos = SERVO_CLOSED;

void setup() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  gateServo.attach(SERVO_PIN);
  gateServo.write(currentPos);
  lastEncoderCLK = digitalRead(ENCODER_CLK);
}

void loop() {
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastEncoderCLK && currentCLK == LOW) {
    int dtValue = digitalRead(ENCODER_DT);
    if (dtValue == HIGH) {
      abrirCancela();
    } else {
      fecharCancela();
    }
    delay(200); // Debounce
  }
  lastEncoderCLK = currentCLK;
}

void abrirCancela() {
  for (int pos = currentPos; pos <= SERVO_OPEN; pos++) {
    gateServo.write(pos);
    delay(10); //Aumentar o delay para que o servo abra mais lentamente
  }
  currentPos = SERVO_OPEN;
}

void fecharCancela() {
  for (int pos = currentPos; pos >= SERVO_CLOSED; pos--) {
    gateServo.write(pos);
    delay(10); //Aumentar o delay para que o servo feche mais lentamente
  }
  currentPos = SERVO_CLOSED;
}
