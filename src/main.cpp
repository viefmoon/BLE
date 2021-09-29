#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//MAC to SET
uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF2}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF6}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7}; 

//Master MAC
uint8_t MasterAddress[] = {0x24, 0x6F, 0x28, 0xD1, 0xD9, 0x70};//ESP32 CON SIM}

int scanTime = 2; //In seconds
BLEScan* pBLEScan;


// typedef struct struct_message {
//     int id; // must be unique for each sender board
//     String uuid;
//     int rrsi;
// } struct_message;
// struct_message msg;


// // callback when data is sent
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("\r\nLast Packet Send Status:\t");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

// void SetNewMAC(){
//   esp_wifi_set_mac(WIFI_IF_STA, &SlaveNewMACAddress[0]);
// }

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String data = advertisedDevice.toString().c_str();
    char aux_data[12];
    strncpy(aux_data, data.c_str(), sizeof(aux_data));
    aux_data[sizeof(aux_data) - 1] = 0;
    char *token = strtok(aux_data, "-");
    if(token != NULL){
      printf("Encontramos un token: %s", UUID);
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  // WiFi.mode(WIFI_STA);

  // // Init ESP-NOW
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }

  // // Register function
  // esp_now_register_send_cb(OnDataSent);
  
  // // Register peer
  // esp_now_peer_info_t peerInfo;
  // memcpy(peerInfo.peer_addr, MasterAddress, 6);
  // peerInfo.channel = 0;  
  // peerInfo.encrypt = false;
  
  // Add peer        
  // if (esp_now_add_peer(&peerInfo) != ESP_OK){
  //   Serial.println("Failed to add peer");
  //   return;
  // }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  // Send message via ESP-NOW
  /*esp_err_t result = esp_now_send(MasterAddress, (uint8_t *) &myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }*/
}