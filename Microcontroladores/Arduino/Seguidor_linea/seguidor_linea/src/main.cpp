#include <Arduino.h>
#define ENABLE_3_4_PIN 13
#define ENABLE_1_2_PIN 10
#define MO1_PIN_POS 12
#define MO1_PIN_NEG 11
#define MO2_PIN_POS 8
#define MO2_PIN_NEG 9
#define SensorDerPinOut 7
#define SensorIzqPinOut 5
#define SensorCentroPinOut 6
int analogPinDer = A5;
int analogPinIzq = A6;
int analogPinCentro = A7;

// put function declarations here:
void enable();
void forward();
void backward();
void right();
void left();
void readSensors();

void setup() {
  // put your setup code here, to run once:
  pinMode(MO1_PIN_POS, OUTPUT);
  pinMode(MO1_PIN_NEG, OUTPUT);
  pinMode(MO2_PIN_POS, OUTPUT);
  pinMode(MO2_PIN_NEG, OUTPUT);
  pinMode(ENABLE_1_2_PIN, OUTPUT);
  pinMode(ENABLE_3_4_PIN, OUTPUT);
  pinMode(SensorDerPinOut, OUTPUT);
  pinMode(SensorIzqPinOut, OUTPUT);
  pinMode(SensorCentroPinOut, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 enable();
 readSensors();
 //forward();
 //readSensors();
 //backward();
 //readSensors();
 //right();
 //readSensors();
 //left();
}

// put function definitions here:
void enable(){
  digitalWrite(ENABLE_1_2_PIN, HIGH);
  digitalWrite(ENABLE_3_4_PIN, HIGH);
}

void forward() {
  digitalWrite(MO1_PIN_POS, HIGH);
  digitalWrite(MO2_PIN_POS, HIGH);
  delay(1000);  
  digitalWrite(MO1_PIN_POS, LOW);
  digitalWrite(MO2_PIN_POS, LOW);
  delay(1000);  
}

void backward() {
  digitalWrite(MO1_PIN_NEG, HIGH);
  digitalWrite(MO2_PIN_NEG, HIGH);
  delay(1000);  
  digitalWrite(MO1_PIN_NEG, LOW);
  digitalWrite(MO2_PIN_NEG, LOW);
  delay(1000);  
}

void right() {
  digitalWrite(MO2_PIN_POS, HIGH);
  delay(1000);  
  digitalWrite(MO2_PIN_POS, LOW);
  delay(1000);  
}

void left() {
  digitalWrite(MO1_PIN_POS, HIGH);
  delay(1000);  
  digitalWrite(MO1_PIN_POS, LOW);
  delay(1000);  
}

void readSensors() {
  digitalWrite(SensorDerPinOut, LOW);
  digitalWrite(SensorIzqPinOut, LOW);
  digitalWrite(SensorCentroPinOut, LOW);
  delayMicroseconds(300);
  digitalWrite(SensorDerPinOut, HIGH);
  delayMicroseconds(800);
  int sensorDerValue = analogRead(analogPinDer);
  digitalWrite(SensorDerPinOut, LOW);
  delayMicroseconds(300);
  digitalWrite(SensorIzqPinOut, HIGH);
  delayMicroseconds(800);
  int sensorIzqValue = analogRead(analogPinIzq);
  digitalWrite(SensorIzqPinOut, LOW);
  delayMicroseconds(300);
  digitalWrite(SensorCentroPinOut, HIGH);
  delayMicroseconds(800);
  int sensorCentroValue = analogRead(analogPinCentro);
  digitalWrite(SensorCentroPinOut, LOW);
  delayMicroseconds(300);
  Serial.println("Sensor Values:");
  Serial.println("Derecha: ");
  Serial.println(sensorDerValue);
  Serial.println("Izquierda: ");
  Serial.println(sensorIzqValue);
  Serial.println("Centro: ");
  Serial.println(sensorCentroValue);
  Serial.println("--------------");
  delay(800);
}

//arriba de 700 es negro