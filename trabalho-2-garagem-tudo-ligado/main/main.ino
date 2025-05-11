#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_PIN 9
#define STEP_A 2
#define STEP_B 4
#define STEP_C 5
#define STEP_D 6
#define JOY_X A1  // Pino do eixo X do joystick
#define BUTTON_PIN 8
#define BUZZER_PIN 3 // <--- Define o pino do buzzer

LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(2048, STEP_A, STEP_C, STEP_B, STEP_D);
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

const float TEMP_THRESHOLD = 28.0;
float temperatura = 0.0;

//Modos da ventoinha
enum Mode { AUTO, MANUAL };
Mode currentMode = AUTO;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  stepper.setSpeed(10); //Aumenta a velocidade dos Steps dados pelo motor
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Usa resistor pull-up interno
  pinMode(BUZZER_PIN, OUTPUT);       // <--- Inicializa o pino do buzzer
}

void loop() {
  readTemperature();
  handleJoystick();
  updateDisplay();
  if (currentMode == AUTO) {
    if (temperatura > TEMP_THRESHOLD) {
      tone(BUZZER_PIN, 2000);     // Liga o Buzzer
      stepper.step(2048);         // aumenta a quantidade dos Steps dados pelo motor
    } else {
      noTone(BUZZER_PIN);         // Desliga o buzzer
    }
  }
  else if (currentMode == MANUAL) {
    noTone(BUZZER_PIN);           // Força o Buzzer Desligado no modo Manual
    stepper.step(2048);           //Força a ligar mesmo que a temperatura seja baixa
  }
  delay(200);
}

void readTemperature() {
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);
  //Print na consola da temperatura
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
}

void handleJoystick() {
  int joyX = analogRead(JOY_X);
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW; // LOW quando pressionado

  if (buttonPressed) {
    currentMode = MANUAL;   //Botão que ativa o modo Manual
  } else if (joyX < 200) {
    currentMode = MANUAL;   // Joystick para a direita
  } else if (joyX > 800) {
    currentMode = AUTO;     // Joystick para a esquerda
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
