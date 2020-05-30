#include "gen_info.h"

#include "namedMesh.h"

// #include <Arduino.h>
// #include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

//#include "painlessMesh.h"

#define MESH_SSID     "meshNet"
#define MESH_PASSWORD   "meshPswd"
#define MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
namedMesh  mesh;

String src_mac = NODE_LIST[0].c_str();

/* - - - MQTT variables - - - */
#define MQTT_TOPIC          "test"
#define MQTT_PUBLISH_DELAY  10000
#define MQTT_CLIENT_ID      "base station"
#define MQTT_QOS            0
#define MQTT_RETAIN         true

const char *MQTT_SERVER    = "192.168.0.112";
const char *MQTT_USER      = "";      // NULL for no authentication
const char *MQTT_PASSWORD  = "";      // NULL for no authentication

/* - - - WiFi variables - - - */
const char *WIFI_SSID      = "TP-LINK_B03C";
const char *WIFI_PASSWORD  = "005771swGaZ";

/* Creates a partially initialised client instance */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/***************** FUNCTIONS *****************/
/*
 * Initialize WiFi connection.
 */
void initWiFiConnection() {
  Serial.print("\n- - - Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * Reinitialize MQTT connection in case if the first attempt failed.
 */
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Repeat MQTT connection attempt...");

    // attempt to connect
    // if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC, 
    //                        MQTT_QOS, MQTT_RETAIN, "disconnected", false)) {
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("* * * Connected * * *");

      // once connected, publish status of the node
      // mqttClient.publish(MQTT_TOPIC_STATE, "connected", true);
    } else {
      Serial.print("! ! ! Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds ! ! !");
      delay(2000);
    }
  }
}

/*
 * Publish(send) message to the MQTT broker(server) based on topic(identifier for message filtering).
 * @param  topic:    Topic to filter data
 * @param  src_mac:  MAC address of the device
 * @param  payload:  Message/data to be sent
 */
void mqttPublish(char* topic, const char* device, const char* payload) {
  Serial.print("Publishing ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  String message = "test_msrt,device=" + String(device) 
                    + " value=" + String(payload);
  Serial.println(message);
    Serial.println("");
  mqttClient.publish(topic, message.c_str(), true);
}

/*
 * Initialize mesh network.
 */
void initMeshNet() {
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init( MESH_SSID, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 4 );
  
  mesh.stationManual(WIFI_SSID, WIFI_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  mesh.setName(src_mac);

//  mesh.onReceive([](uint32_t from, String &msg) {
//    Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
//    // receivedData[0] = msg.toFloat();
//    Serial.print("Status of msg transmission: ");
//    Serial.println(mesh.sendSingle(dst_nodeName, msg));
//  });
  mesh.onReceive([](String &from, String &msg) {
    Serial.printf(" - - - Received from: %s,  data: %s\n - - - ", from.c_str(), msg.c_str());

    String device, payload;
    for (int i = 0; i < msg.length(); ++i) {
      if (msg.substring(i, i+1) == "|") {
        device = msg.substring(0, i);
        payload = msg.substring(i+1);
        break;
      }
    }
    
    initWiFiConnection();
    mqttClient.setServer(MQTT_SERVER, 1883);
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttPublish(MQTT_TOPIC, device.c_str(), payload.c_str());
  });
  
  mesh.onNewConnection([](uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
  });

  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });
  
  // mesh.onNodeTimeAdjusted([](int32_t offset) {
  //   Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  // });

  mesh.setRoot(true);
  mesh.setContainsRoot(true);
}

/*
 * Convert MAC address to string
 * @param  mac:   MAC address needed to be conveted
 */
String convertMacToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

// ***************** SETUP AND LOOP *****************

void setup() {
  Serial.begin(115200);

  initMeshNet();
}

void loop() {
  mesh.update();
  
  mqttClient.loop();
}
