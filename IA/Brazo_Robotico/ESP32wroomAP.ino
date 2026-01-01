#include <ESP32Servo.h>
#include <ArduinoJson.h>

// Pines de servos
const int SERVO_BASE = 12;
const int SERVO_HOMBRO = 13;  
const int SERVO_CODO = 14;
const int SERVO_PINZA = 15;

Servo servoBase, servoHombro, servoCodo, servoPinza;

// Configuración de pinza
const int PINZA_CERRADA = 70;
const int PINZA_ABIERTA = 50;

// Posiciones actuales
int posBase = 90;      
int posHombro = 65;   
int posCodo = 105;
bool pinzaCerrada = false;

// Posiciones de trabajo
const int POS_CENTRO = 90;
const int POS_ALTA_HOMBRO = 65;
const int POS_ALTA_CODO = 105;
const int POS_BAJA_HOMBRO = 150;
const int POS_BAJA_CODO = 180;
const int POS_DEPOSITO_DER = 25;

// Posiciones para bajada lenta
const int POS_MEDIA1_HOMBRO = 110;
const int POS_MEDIA1_CODO = 140;
const int POS_MEDIA2_HOMBRO = 130;
const int POS_MEDIA2_CODO = 160;

void setup() {
  Serial.begin(115200);
  
  servoBase.attach(SERVO_BASE);
  servoHombro.attach(SERVO_HOMBRO);
  servoCodo.attach(SERVO_CODO);
  servoPinza.attach(SERVO_PINZA);
  
  calibrateArm();
  Serial.println("{\"status\": \"ready\"}");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    processCommand(message);
  }
}

void processCommand(String message) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.println("{\"error\": \"invalid_json\"}");
    return;
  }
  
  String command = doc["command"];
  
  if (command == "pick_bad_fork") {
    executePickBadFork();
  }
  else if (command == "reset") {
    resetArm();
  }
  else if (command == "test_gripper") {
    testGripper();
  }
  else if (command == "go_center") {
    moverBrazoSuave(POS_CENTRO, POS_ALTA_HOMBRO, POS_ALTA_CODO);
  }
  else if (command == "test_sequence") {
    testSequence();
  }
  else if (command == "calibrate") {
    calibrateArm();
  }
  else {
    Serial.println("{\"error\": \"unknown_command\"}");
  }
}

void calibrateArm() {
  abrirPinzaRapido();
  moverBrazoSuave(POS_CENTRO, 100, 120);
  delay(1000);
  moverBrazoSuave(POS_CENTRO, POS_ALTA_HOMBRO, POS_ALTA_CODO);
  delay(1000);
  
  posBase = POS_CENTRO;
  posHombro = POS_ALTA_HOMBRO;
  posCodo = POS_ALTA_CODO;
  
  Serial.println("{\"calibration_status\": \"completed\"}");
}

void executePickBadFork() {
  Serial.println("{\"status\": \"starting_sequence\", \"object\": \"bad_fork\"}");
  
  if (posHombro != POS_ALTA_HOMBRO || posCodo != POS_ALTA_CODO) {
    calibrateArm();
  }
  
  // Solo busca en el centro
  bajarPorPasosLento();
  delay(1000);
  
  // Detección en el centro
  if (detectarTenedorEnCentro()) {
    Serial.println("{\"step\": \"object_detected\"}");
    cerrarPinzaRapido();
    delay(500);
    
    moverBrazoSuave(POS_CENTRO, POS_ALTA_HOMBRO, POS_ALTA_CODO);
    delay(1500);
    
    moverBrazoSuave(POS_DEPOSITO_DER, POS_ALTA_HOMBRO, POS_ALTA_CODO);
    delay(1500);

    abrirPinzaRapido();
    delay(500);
    
    Serial.println("{\"status\": \"sequence_completed\", \"object\": \"bad_fork\"}");
  } else {
    Serial.println("{\"status\": \"no_object_detected\"}");
    abrirPinzaRapido();
  }
  
  calibrateArm();
}

bool detectarTenedorEnCentro() {
  // Simula detección - en producción usar sensores reales
  // 80% probabilidad de detectar tenedor en centro
  return (random(100) < 80);
}

void bajarPorPasosLento() {
  moverBrazoDirecto(POS_CENTRO, POS_MEDIA1_HOMBRO, POS_MEDIA1_CODO);
  delay(1500);
  
  moverBrazoDirecto(POS_CENTRO, POS_MEDIA2_HOMBRO, POS_MEDIA2_CODO);
  delay(1500);
  
  moverBrazoDirecto(POS_CENTRO, POS_BAJA_HOMBRO, POS_BAJA_CODO);
  delay(1500);
}

void moverBrazoDirecto(int targetBase, int targetHombro, int targetCodo) {
  targetBase = constrain(targetBase, 20, 170);
  targetHombro = constrain(targetHombro, 60, 170);
  targetCodo = constrain(targetCodo, 90, 180);
  
  servoBase.write(targetBase);
  servoHombro.write(targetHombro);
  servoCodo.write(targetCodo);
  
  posBase = targetBase;
  posHombro = targetHombro;
  posCodo = targetCodo;
  
  delay(1500);
}

void testSequence() {
  testGripper();
  delay(1000);
  executePickBadFork();
}

void moverBrazoSuave(int targetBase, int targetHombro, int targetCodo) {
  targetBase = constrain(targetBase, 20, 170);
  targetHombro = constrain(targetHombro, 60, 170);
  targetCodo = constrain(targetCodo, 90, 180);
  
  int pasos = 30;
  for (int i = 0; i <= pasos; i++) {
    float progreso = (float)i / pasos;
    int base = posBase + (targetBase - posBase) * progreso;
    int hombro = posHombro + (targetHombro - posHombro) * progreso;
    int codo = posCodo + (targetCodo - posCodo) * progreso;
    
    servoBase.write(base);
    servoHombro.write(hombro);
    servoCodo.write(codo);
    delay(80);
  }
  
  posBase = targetBase;
  posHombro = targetHombro;
  posCodo = targetCodo;
  delay(800);
}

void cerrarPinzaRapido() {
  for (int angulo = PINZA_ABIERTA; angulo <= PINZA_CERRADA; angulo += 8) {
    servoPinza.write(angulo);
    delay(80);
  }
  servoPinza.write(PINZA_CERRADA);
  delay(300);
  pinzaCerrada = true;
  delay(500);
}

void abrirPinzaRapido() {
  for (int angulo = PINZA_CERRADA; angulo >= PINZA_ABIERTA; angulo -= 10) {
    servoPinza.write(angulo);
    delay(50);
  }
  servoPinza.write(PINZA_ABIERTA);
  delay(100);
  pinzaCerrada = false;
  delay(300);
}

void testGripper() {
  abrirPinzaRapido();
  delay(1000);
  cerrarPinzaRapido();
  delay(1000);
  abrirPinzaRapido();
  delay(1000);
}

void resetArm() {
  calibrateArm();
  Serial.println("{\"status\": \"reset_complete\"}");
}