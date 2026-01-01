#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Configuración Access Point
const char* ssid = "RobotCam";
const char* password = "12345678";

WebServer server(80);

// Pines para AI-Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void handleVideo() {
  Serial.println("🎥 Iniciando stream CON BUFFER CONTROL");
  
  WiFiClient client = server.client();
  client.setTimeout(5000); // Timeout de 5 segundos
  
  // Encabezados MJPEG
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n";
  response += "Connection: close\r\n";
  response += "Access-Control-Allow-Origin: *\r\n";
  response += "\r\n";
  
  if (client.write(response.c_str()) != response.length()) {
    Serial.println("❌ Error enviando headers");
    return;
  }

  uint32_t lastFrameTime = 0;
  const uint32_t FRAME_INTERVAL = 100; // 10 FPS máximo
  uint16_t frameCount = 0;
  bool clientConnected = true;

  while (clientConnected && client.connected()) {
    uint32_t currentTime = millis();
    
    // Control de FPS estricto
    if (currentTime - lastFrameTime >= FRAME_INTERVAL) {
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("⚠️  Frame nulo, reintentando...");
        continue;
      }

      // Construir frame MJPEG completo
      String frameData = "--frame\r\n";
      frameData += "Content-Type: image/jpeg\r\n";
      frameData += "Content-Length: " + String(fb->len) + "\r\n";
      frameData += "\r\n";
      
      // Enviar headers del frame
      if (client.write(frameData.c_str()) != frameData.length()) {
        Serial.println("❌ Cliente desconectado (headers)");
        clientConnected = false;
        esp_camera_fb_return(fb);
        break;
      }
      
      // Enviar datos JPEG
      if (client.write(fb->buf, fb->len) != fb->len) {
        Serial.println("❌ Cliente desconectado (datos)");
        clientConnected = false;
        esp_camera_fb_return(fb);
        break;
      }
      
      // Finalizar frame
      if (client.write("\r\n", 2) != 2) {
        Serial.println("❌ Cliente desconectado (final)");
        clientConnected = false;
        esp_camera_fb_return(fb);
        break;
      }
      
      esp_camera_fb_return(fb);
      frameCount++;
      lastFrameTime = currentTime;
      
      // Log cada 30 frames
      if (frameCount % 30 == 0) {
        Serial.printf("📊 Frames enviados: %d\n", frameCount);
      }
    }
    
    // Pequeña pausa para dar tiempo al cliente
    delay(10);
    
    // Verificar si el cliente sigue conectado
    if (!client.connected()) {
      clientConnected = false;
    }
  }
  
  Serial.printf("🔌 Stream terminado. Frames enviados: %d\n", frameCount);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("🚀 ESP32-CAM - STREAM ESTABLE");

  // Configuración ESTABLE
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000; // 10MHz para más estabilidad
  
  // Configuración para ESTABILIDAD
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;    // 320x240
  config.jpeg_quality = 18;              // Compresión media-alta
  config.fb_count = 2;                   // 2 buffers para estabilidad

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Error cámara: 0x%x\n", err);
    return;
  }
  
  Serial.println("✅ Cámara inicializada - 2 buffers");

  // Access Point
  WiFi.softAP(ssid, password);
  delay(2000);
  
  Serial.println("==================================");
  Serial.print("📶 SSID: ");
  Serial.println(ssid);
  Serial.print("🌐 IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("🎯 Stream: 10 FPS ESTABLE");
  Serial.println("==================================");

  server.on("/video", handleVideo);
  
  server.begin();
  Serial.println("✅ Servidor listo - Stream estable");
}

void loop() {
  server.handleClient();
}