#include "esp_camera.h"
#include <WiFi.h>

// === CAMERA MODEL ===
#define CAMERA_MODEL_AI_THINKER

// === WiFi Credentials ===
const char* ssid = "Mujtaba 1";
const char* password = "smaks1972";

// === Camera Pin Config (AI Thinker) ===
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

// === Motor PWM Pins ===
#define MOTOR1_PIN 33
#define MOTOR2_PIN 12
#define MOTOR3_PIN 14
#define MOTOR4_PIN 13

// === Motor Speed ===
int motorSpeed = 200;

// === Setup PWM Motors ===
void setupPWM() {
  ledcSetup(0, 500, 8);
  ledcSetup(1, 500, 8);
  ledcSetup(2, 500, 8);
  ledcSetup(3, 500, 8);

  ledcAttachPin(MOTOR1_PIN, 0);
  ledcAttachPin(MOTOR2_PIN, 1);
  ledcAttachPin(MOTOR3_PIN, 2);
  ledcAttachPin(MOTOR4_PIN, 3);
}

void startMotor() {
  ledcWrite(0, motorSpeed);
  ledcWrite(1, motorSpeed);
  ledcWrite(2, motorSpeed);
  ledcWrite(3, motorSpeed);
}

void stopMotor() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

// === Camera Server ===
#include <WebServer.h>
WebServer server(80);

void handle_jpg_stream() {
  WiFiClient client = server.client();
  camera_fb_t *fb = NULL;
  char *part = "Content-Type: image/jpeg\r\n\r\n";
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    response = "--frame\r\n";
    response += part;
    server.sendContent(response);
    server.sendContent((const char *)fb->buf, fb->len);
    server.sendContent("\r\n");
    esp_camera_fb_return(fb);

    if (!client.connected()) break;
  }
}

void startCameraServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html",
      "<html><body><h1>ESP32-CAM</h1><img src='/stream'></body></html>");
  });
  server.on("/stream", HTTP_GET, handle_jpg_stream);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // === Camera Init ===
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // === WiFi Connect ===
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Camera Ready! Stream: http://");
  Serial.println(WiFi.localIP());

  // === Start Server and Motors ===
  startCameraServer();
  setupPWM();
  startMotor();  // motors spin at power-up
}

void loop() {
  delay(10000);  // No work here
}
