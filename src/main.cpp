//Slave code
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//Slave id
#define slave_id 1
//MAC to SET on slave
uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF2}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF6}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7}; 

//Master MAC
uint8_t MasterAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};//ESP32 CON SIM}

int scanTime = 1; //In seconds
BLEScan* pBLEScan;

struct struct_message {
    uint8_t id; // must be unique for each sender board
    char uuid_[8];
    uint8_t rrsi_;
};
struct_message msg;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
Serial.print("\r\nLast Packet Send Status:\t");
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void SetNewMAC(){
  esp_wifi_set_mac(WIFI_IF_STA, &SlaveNewMACAddress[0]);
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String data = advertisedDevice.toString().c_str();
    if(data != NULL){
    char aux_data[12];
    char rssi[3];
    strncpy(aux_data, data.c_str(), sizeof(aux_data));
    aux_data[sizeof(aux_data) - 1] = 0;

    for (size_t i = 0; i < 8; i++)
    {
      msg.uuid_[i] = aux_data[i];
    }
    for (size_t i = 0; i < 3; i++)
    {
      rssi[i] = aux_data[i+9];
    }

    msg.rrsi_ = atoi(rssi);
    
    Serial.print(msg.uuid_[0]);
    Serial.print(msg.uuid_[1]);
    Serial.print(msg.uuid_[2]);
    Serial.print(msg.uuid_[3]);
    Serial.print(msg.uuid_[4]);
    Serial.print(msg.uuid_[5]);
    Serial.print(msg.uuid_[6]);
    Serial.println(msg.uuid_[7]);

    Serial.println(msg.rrsi_);
      // Send message via ESP-NOW
    esp_err_t result = esp_now_send(MasterAddress, (uint8_t *) &msg, sizeof(msg));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    }
  }
};

void setup() {
  Serial.begin(115200);
  msg.id = slave_id;

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  SetNewMAC();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  return;
  }

  // Register function
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, MasterAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

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

}