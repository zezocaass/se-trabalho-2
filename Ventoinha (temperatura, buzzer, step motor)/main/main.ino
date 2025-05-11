#include <OneWire.h>
#include <DallasTemperature.h>
#include <Stepper.h>

#define DS18B20_PIN 9
#define BUZZER_PIN 3
#define STEP_A 2
#define STEP_B 4
#define STEP_C 5
#define STEP_D 6

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
Stepper stepper(2048, STEP_A, STEP_C, STEP_B, STEP_D);

const float TEMP_THRESHOLD = 28.0;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  stepper.setSpeed(10); // RPM
}

void loop() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
  
  if (temp > TEMP_THRESHOLD) {
    tone(BUZZER_PIN, 2000);         // Liga buzzer
    stepper.step(100);              // Gira ventoinha
  } else {
    noTone(BUZZER_PIN);             // Desliga buzzer
  }
  

  delay(1000);
}
