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

LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(2048, STEP_A, STEP_C, STEP_B, STEP_D);
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

const float TEMP_THRESHOLD = 28.0;
float temperatura = 0.0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  stepper.setSpeed(10); //Aumenta a velocidade dos Steps dados pelo motor
}

void loop() {
  readTemperature();
  updateDisplay();
  delay(1000); 
}

void readTemperature() {
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  if (temperatura > TEMP_THRESHOLD) {
    stepper.step(2048); //Aumenta a quantidade de Steps dados pelo motor (parece mais ventoinha)
  }
}

void updateDisplay() {
  lcd.setCursor(0,0);
  lcd.print("Temperatura:   ");
  lcd.setCursor(0,1);
  lcd.print(temperatura, 1);
  lcd.print(" C      ");
}
