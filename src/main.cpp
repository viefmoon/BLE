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
#define slave_id 0
#define first_c "aa"

//MAC to SET on slave
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1}; 
uint8_t SlaveNewMACAddress[] = {0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF2}; 
//uint8_t SlaveNewMACAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3}; 

//Master MAC
uint8_t MasterAddress[] = {0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};//ESP32 CON SIM}

uint8_t scanTime = 1; //In seconds
BLEScan* pBLEScan;

struct struct_message {
    uint8_t id; // slave id
    uint8_t rssi;
    char uuid_[9];
};
struct_message msg;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//Serial.print("\r\nLast Packet Send Status:\t");
//Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String data = advertisedDevice.toString().c_str();
    if(data != NULL){
      delay(500);
      char aux_data[12];
      char rssi[3];

      strncpy(aux_data, data.c_str(), sizeof(aux_data));
      aux_data[sizeof(aux_data) - 1] = 0;

      //Serial.print("UUID: ");
      for (size_t i = 0; i < 9; i++)
      {
        msg.uuid_[i] = aux_data[i];
        //Serial.print(msg.uuid_[i]);
      }
      msg.uuid_[8] = '\0';
      for (size_t i = 0; i < 3; i++)
      {
        rssi[i] = aux_data[i+9];
      }

      msg.rssi = atoi(rssi);

      //msg.distance = pow(10, ((double) (rrsi_ - MEASURE_RSSI)) / (10 * n));
      //Serial.print("  RSSI: ");
      //Serial.println(msg.rssi);

      //check first´s chars
      char a[3];
      a[0] = msg.uuid_[0];
      a[1] = msg.uuid_[1];
      a[2] = '\0';

      if (strcmp(a, first_c) == 0) {
        esp_err_t result = esp_now_send(MasterAddress, (uint8_t *) &msg, sizeof(msg));
        if (result == ESP_OK) {
          //Serial.println("Sent with success");
        }
        else {
          //Serial.println("Error sending the data");
        }
      }
    }
  }
};

void setup() {
  //Serial.begin(115200);
  msg.id = slave_id;

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(ESP_IF_WIFI_STA, &SlaveNewMACAddress[0]);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Error initializing ESP-NOW");
  return;
  }

  // Register function on data sent
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, MasterAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer");
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