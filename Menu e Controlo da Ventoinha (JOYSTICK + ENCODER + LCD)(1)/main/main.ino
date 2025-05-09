#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Stepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define JOY_X A1
#define ENCODER_CLK 7
#define ENCODER_DT A2

#define STEP_A 2
#define STEP_B 4
#define STEP_C 5
#define STEP_D 6

#define DS18B20_PIN 9

LiquidCrystal_I2C lcd(0x27, 16, 2);
Encoder myEnc(ENCODER_CLK, ENCODER_DT);
Stepper stepper(2048, STEP_A, STEP_C, STEP_B, STEP_D);
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

enum Menu { MENU_TEMP, MENU_SPEED };
Menu currentMenu = MENU_TEMP;

int fanSpeed = 0;           
float lastEncPos = 0;       // posição anterior do encoder
float temperatura = 0.0;

unsigned long lastTempRead = 0;
const unsigned long tempInterval = 1000; // 1 segundo

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  myEnc.write(0);
  stepper.setSpeed(fanSpeed);
}

void loop() {
  readTemperature();
  handleJoystick();
  updateDisplay();
  handleFanControl();
  delay(100);
}

// ========================== Funções ==========================

void readTemperature() {
  if (millis() - lastTempRead >= tempInterval) {
    sensors.requestTemperatures();
    temperatura = sensors.getTempCByIndex(0);
    lastTempRead = millis();
  }
}

void handleJoystick() {
  int joyX = analogRead(JOY_X);
  if (joyX > 800) currentMenu = MENU_SPEED;
  else if (joyX < 200) currentMenu = MENU_TEMP;
}

void updateDisplay() {
  if (currentMenu == MENU_TEMP) {
    lcd.setCursor(0,0);
    lcd.print("Temperatura:");
    lcd.setCursor(0,1);
    lcd.print(temperatura, 1);
    lcd.print(" C");
  } else if (currentMenu == MENU_SPEED) {
    handleEncoder(); // só ativa o encoder aqui!
    lcd.setCursor(0,0);
    lcd.print("Velocidade:");
    lcd.setCursor(0,1);
    lcd.print(fanSpeed);
    lcd.print(" RPM");
  }
}

void handleEncoder() {
  float encPos = myEnc.read() / 4.0;
  float delta = encPos - lastEncPos;
  static float stepAccumulator = 0;

  if (delta != 0) {
    stepAccumulator += delta;
    lastEncPos = encPos;

    while (stepAccumulator >= 1.0) {
      fanSpeed = constrain(fanSpeed + 1, 0, 100);
      stepAccumulator -= 1.0;
    }

    while (stepAccumulator <= -1.0) {
      fanSpeed = constrain(fanSpeed - 1, 0, 100);
      stepAccumulator += 1.0;
    }

    stepper.setSpeed(fanSpeed);
  }
}


void handleFanControl() {
  // Se temperatura > 27°C, força velocidade mínima
  if (temperatura > 27.0 && fanSpeed == 0) {
    fanSpeed = 10;
    stepper.setSpeed(fanSpeed);
  }

  if (temperatura < 26.0 && fanSpeed == 10) {
  fanSpeed = 0;
  stepper.setSpeed(fanSpeed);
}

  if (fanSpeed > 0) {
    stepper.step(10); // mantém o motor rodando
  }
}
