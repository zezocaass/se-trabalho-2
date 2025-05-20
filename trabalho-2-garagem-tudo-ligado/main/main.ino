#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#define DS18B20_PIN 9
#define STEP_A 2
#define STEP_B 4
#define STEP_C 5
#define STEP_D 6
#define JOY_X A1
#define BUTTON_PIN 8
#define BUZZER_PIN 3

#define TRIG_PIN 11
#define ECHO_PIN 12
#define SERVO_PIN 10
#define SERVO_OPEN 90
#define SERVO_CLOSED 0
const int DISTANCE_THRESHOLD = 10; // distância para abrir portão

// Encoder
#define ENCODER_CLK 7
#define ENCODER_DT  A2

LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(2048, STEP_A, STEP_C, STEP_B, STEP_D);
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
Servo gateServo;

const float TEMP_THRESHOLD = 24.0;
float temperatura = 0.0;

enum Mode { AUTO, MANUAL };
Mode currentMode = AUTO;

int currentGatePos = SERVO_CLOSED;

int lastEncoderCLK = HIGH;
unsigned long manualOverrideUntil = 0;
const unsigned long overrideDuration = 5000; // 5 segundos

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  stepper.setSpeed(10);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(currentGatePos);

  // Encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  lastEncoderCLK = digitalRead(ENCODER_CLK);
}

void loop() {
  readTemperature();
  handleJoystick();
  updateDisplay();

  handleEncoder();

  if (millis() > manualOverrideUntil) {
    controlarPortaoPorProximidade();
  }

  if (currentMode == AUTO) {
    if (temperatura > TEMP_THRESHOLD) {
      tone(BUZZER_PIN, 2000);
      stepper.step(2048);
    } else {
      noTone(BUZZER_PIN);
    }
  } else if (currentMode == MANUAL) {
    noTone(BUZZER_PIN);
    stepper.step(2048);
  }
  delay(50);
}

void handleEncoder() {
  static int lastCLK = HIGH;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 3; 

  int currentCLK = digitalRead(ENCODER_CLK);

  if (lastCLK == HIGH && currentCLK == LOW) {
    if ((millis() - lastDebounceTime) > debounceDelay) {
      int dtValue = digitalRead(ENCODER_DT);
      if (dtValue == HIGH && currentGatePos != SERVO_OPEN) {
        abrirPortao();
        manualOverrideUntil = millis() + overrideDuration;
        Serial.println("Portão aberto manualmente via encoder");
      } else if (dtValue == LOW && currentGatePos != SERVO_CLOSED) {
        fecharPortao();
        manualOverrideUntil = millis() + overrideDuration;
        Serial.println("Portão fechado manualmente via encoder");
      }
      lastDebounceTime = millis();
    }
  }
  lastCLK = currentCLK;
}


void controlarPortaoPorProximidade() {
  long distance = readDistance();
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < DISTANCE_THRESHOLD && currentGatePos != SERVO_OPEN) {
    abrirPortao();
  } else if (distance >= DISTANCE_THRESHOLD && currentGatePos != SERVO_CLOSED) {
    fecharPortao();
  }
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  long distance = duration * 0.034 / 2;
  return distance;
}

void abrirPortao() {
  for (int pos = currentGatePos; pos <= SERVO_OPEN; pos++) {
    gateServo.write(pos);
    delay(10);
  }
  currentGatePos = SERVO_OPEN;
}

void fecharPortao() {
  for (int pos = currentGatePos; pos >= SERVO_CLOSED; pos--) {
    gateServo.write(pos);
    delay(10);
  }
  currentGatePos = SERVO_CLOSED;
}

void readTemperature() {
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
}

void handleJoystick() {
  int joyX = analogRead(JOY_X);
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  if (buttonPressed) {
    currentMode = MANUAL;
  } else if (joyX < 200) {
    currentMode = MANUAL;
  } else if (joyX > 800) {
    currentMode = AUTO;
  }
}

void updateDisplay() {
  lcd.setCursor(0,0);
  if (currentMode == AUTO) {
    lcd.print("Modo: Auto     ");
  } else {
    lcd.print("Modo: Manual   ");
  }
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1);
  lcd.print(" C     ");
}
