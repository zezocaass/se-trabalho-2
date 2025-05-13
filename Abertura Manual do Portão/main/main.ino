#include <Servo.h>

#define ENCODER_CLK 7
#define ENCODER_DT  A2
#define SERVO_PIN 10

#define TRIG_PIN 11
#define ECHO_PIN 12

#define SERVO_OPEN 90
#define SERVO_CLOSED 0
const int DISTANCE_THRESHOLD = 30; // cm para abrir portão

Servo gateServo;
int lastEncoderCLK = 0;
int currentPos = SERVO_CLOSED;

// Variável para bloquear o ultrassónico após uso manual
unsigned long manualOverrideUntil = 0;
const unsigned long overrideDuration = 5000; // 5 segundos

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  gateServo.attach(SERVO_PIN);
  gateServo.write(currentPos);
  lastEncoderCLK = digitalRead(ENCODER_CLK);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
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
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastEncoderCLK && currentCLK == LOW) {
    int dtValue = digitalRead(ENCODER_DT);
    if (dtValue == HIGH) {
      abrirCancela();
    } else {
      fecharCancela();
    }
    manualOverrideUntil = millis() + overrideDuration; // Bloqueia ultrassónico por 5 segundos para evitar que feche sozinho
    delay(200); // Debounce
  }
  lastEncoderCLK = currentCLK;

  if (millis() > manualOverrideUntil) {
    long distance = readDistance();
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > 0 && distance < DISTANCE_THRESHOLD && currentPos != SERVO_OPEN) {
      abrirCancela();
    } else if (distance >= DISTANCE_THRESHOLD && currentPos != SERVO_CLOSED) {
      fecharCancela();
    }
  }

  delay(100); // Pequeno delay para estabilidade
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
