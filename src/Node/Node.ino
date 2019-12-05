#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
#include "user_interface.h"
}

// this is the MAC Address of the remote ESP server which receives these sensor readings
uint8_t remoteMac[] = {0x1A, 0xFE, 0x34, 0xD5, 0xFA, 0x39};

// this is the MAC Address of this Node
uint8_t mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};

void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
}

#define WIFI_CHANNEL 1
#define SLEEP_SECS 5  // 15 minutes
#define SEND_TIMEOUT 15000  // 5000 millis seconds timeout = 5 sec

#define MESSAGELEN 18

// keep in sync with slave struct
struct __attribute__((packed)) SENSOR_DATA {
  char src_mac[MESSAGELEN];
  long val;
} sensorData;

long previousMillis = 0;
volatile boolean callbackCalled;

uint8_t tmp_bs[sizeof(sensorData)];
bool tmp_buffer = false;
bool send_status = false;

void setup() {
  int i = 0;
  Serial.begin(115200);
  Serial.println(">>ESP_Now Combo initialized!");
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());

  Serial.printf("Target mac: %2X:%2X:%2X:%2X:%2X:%2X", remoteMac[0], remoteMac[1], remoteMac[2], remoteMac[3], remoteMac[4], remoteMac[5]);
  Serial.printf(", channel: %i\n", WIFI_CHANNEL);
  
  if (esp_now_init() != 0) {
    Serial.println(">>ESP_Now initialized failed!");
    gotoSleep();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Serves as both master and slave
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {

    Serial.printf(">>Received from: %2X:%2X:%2X:%2X:%2X:%2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println();
    Serial.print(">>Received data: ");
    
    Serial.write(data, len);
    Serial.println();
    
    memcpy(tmp_bs, data, sizeof(sensorData));
    tmp_buffer = true;
    Serial.println(">>Saving payload to buffer!");
    callbackCalled = false;
  });

  esp_now_add_peer(remoteMac, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);
  esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {
    Serial.printf(">>Sending payload ------------ Done: status = %i\n", sendStatus);
    if(sendStatus == 1) send_status = false;
    else send_status = true;
  });
  
  callbackCalled = false;
  char t_mac[18];
  WiFi.macAddress().toCharArray(t_mac, 18);
  strncpy(sensorData.src_mac, t_mac, 18);
}

void loop() {
  long currentMillis = millis();
  
  if (currentMillis - previousMillis > SEND_TIMEOUT){
    Serial.print(">>Time passed: "); 
    Serial.println(currentMillis - previousMillis);
    previousMillis = currentMillis;
    sensorData.val = currentMillis;
    Serial.print(">>Payload: ");
    Serial.print(sensorData.src_mac);
    Serial.print(" ");
    Serial.println(sensorData.val);
 
    uint8_t bs[sizeof(sensorData)];
    memcpy(bs, &sensorData, sizeof(sensorData));  
    while(esp_now_send(NULL, bs, sizeof(sensorData))) 
    {     
      Serial.println(">>Trying to send payload!");  
    }
  }
  if(tmp_buffer && (currentMillis - previousMillis > 5000)) 
  {
    Serial.println(">>Sending buffer data!");
    while(esp_now_send(NULL, tmp_bs, sizeof(sensorData))) 
    {
      Serial.println(">>Trying to send buffer payload!");  
    }
    tmp_buffer = false;
  }
  
}

void gotoSleep() {
  Serial.printf("Up for %i ms, going to sleep for %i secs...\n", millis(), SLEEP_SECS);
  ESP.deepSleep(SLEEP_SECS * 1000000, RF_NO_CAL);
}
