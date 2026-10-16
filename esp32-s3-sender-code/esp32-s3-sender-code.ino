#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x10, 0xb4, 0x1d, 0xc4, 0x7a, 0xa0};

// New structure holding X and Y joystick data
typedef struct struct_message {
  int x;
  int y;
} struct_message;

struct_message joystickData;
esp_now_peer_info_t peerInfo;

// Define joystick pins
const int xPin = 4;
const int yPin = 5;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Commented out the print statements to prevent serial monitor spam 
  // since we will be sending data very fast now.
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Using the forced cast that worked for you earlier
  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // 1. Read raw values with the delay to prevent crosstalk
  int rawX = analogRead(xPin);
  delay(5); 
  int rawY = analogRead(yPin);

  // 2. Constrain the values to your ACTUAL reliable limits
  // (Change 200 and 3800 to whatever numbers your joystick reliably hits)
  rawX = constrain(rawX, 200, 3800);
  rawY = constrain(rawY, 200, 3800);

  // 3. Map the constrained 200-3800 range into a clean -100 to 100 percentage
  joystickData.x = map(rawX, 200, 3800, -100, 100);
  joystickData.y = map(rawY, 200, 3800, 100, -100);

  // Send the cleaned-up percentage data
  esp_now_send(broadcastAddress, (uint8_t *) &joystickData, sizeof(joystickData));
   
  delay(50); 
}
