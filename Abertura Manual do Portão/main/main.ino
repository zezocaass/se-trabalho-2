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

// Variáveis do encoder
int lastEncoderCLK = HIGH;
int currentPos = SERVO_CLOSED;

// Variáveis de temporização
unsigned long manualOverrideUntil = 0;
const unsigned long overrideDuration = 5000; // 5 segundos de override

void setup() {
  Serial.begin(9600);
  
  // Configura encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  
  // Configura servo
  gateServo.attach(SERVO_PIN);
  gateServo.write(currentPos);
  
  // Configura sensor ultrassónico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // 1. Verifica encoder para controlo manual
  handleEncoder();
  
  // 2. Se não estiver em override manual, verifica sensor
  if (millis() > manualOverrideUntil) {
    handleUltrasonic();
  }
  
  delay(50); // Delay geral para estabilidade
}

void handleEncoder() {
  int newCLK = digitalRead(ENCODER_CLK);
  
  // Deteta mudança no encoder
  if (newCLK != lastEncoderCLK && newCLK == LOW) {
    int dtState = digitalRead(ENCODER_DT);
    
    if (dtState == HIGH) {
      abrirCancela();
    } else {
      fecharCancela();
    }
    
    manualOverrideUntil = millis() + overrideDuration;
    Serial.println("Modo Manual Ativo!");
  }
  lastEncoderCLK = newCLK;
}

void handleUltrasonic() {
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

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void abrirCancela() {
  for (int pos = currentPos; pos <= SERVO_OPEN; pos++) {
    gateServo.write(pos);
    delay(15);
  }
  currentPos = SERVO_OPEN;
}

void fecharCancela() {
  for (int pos = currentPos; pos >= SERVO_CLOSED; pos--) {
    gateServo.write(pos);
    delay(15);
  }
  currentPos = SERVO_CLOSED;
}
