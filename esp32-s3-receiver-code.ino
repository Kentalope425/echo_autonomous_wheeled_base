#include <esp_now.h>
#include <WiFi.h>

// Define the pins for UART communication to the Teensy
// Change these to match whichever pins you wired up on the S3
const int rxPin = 18;
const int txPin = 17;

typedef struct struct_message {
    int x;
    int y;
} struct_message;

struct_message joystickData;

void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));
  
  // 1. Debug output to your computer's USB Serial Monitor
  Serial.print("X: "); Serial.print(joystickData.x);
  Serial.print("  |  Y: "); Serial.println(joystickData.y);

  // 2. Forward the exact same data to the Teensy via Hardware Serial
  // Sending as: "X,Y\n" (e.g., "100,-50\n")
  Serial1.print("X:");
  Serial1.print(joystickData.x);
  Serial1.print(",Y:");
  Serial1.println(joystickData.y);
}
 
void setup() {
  // Initialize USB Serial for debugging (Computer)
  Serial.begin(115200);
  
  // Initialize Hardware Serial for forwarding (Teensy)
  Serial1.begin(115200, SERIAL_8N1, rxPin, txPin);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb((esp_now_recv_cb_t)OnDataRecv);
}
 
void loop() {}