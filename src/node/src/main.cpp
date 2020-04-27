#include "gen_info.h"

#include "namedMesh.h"

// #include <Arduino.h>

//#include "painlessMesh.h"

#define SLEEP_SECS 10

#define MESH_SSID     "meshNet"
#define MESH_PASSWORD   "meshPswd"
#define MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
namedMesh mesh;

String src_mac = NODE_LIST[1].c_str();   // set node's source mac addr      based on gen_info.h
String dst_mac = NODE_LIST[0].c_str();   // set node's destination mac addr based on gen_info.h

/***************** FUNCTIONS *****************/
/*
 * Read metrics from sensor
 * @return  metrics
 */
float readSensorData() {
  
  // ! ! ! Read data from the sensor modules TBA ! ! !

  return 86.8;
}

/*
 * Put node into deep sleep mode.
 */
void gotoSleep() {
  Serial.printf(" * Up for %i ms, going to sleep for %i secs...\n\n", millis(), SLEEP_SECS);
  ESP.deepSleep(SLEEP_SECS*1000000, WAKE_NO_RFCAL);
}

Task taskSendMessage( TASK_SECOND*10, TASK_FOREVER, []() {
  String msg = src_mac + "|" + readSensorData();
  Serial.print("! ! ! Sending message: ");
  Serial.println("from = " + src_mac + ", payload = " + msg);
  Serial.print("Status of transmission: ");
  Serial.println(mesh.sendSingle(dst_mac, msg));
  Serial.println("");

  // gotoSleep();
});

/*
 * Initialize mesh network.
 */
void initMeshNet() {
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init( MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT );
  // mesh.setName(src_nodeName); // This needs to be an unique name! 
  
  mesh.setName(src_mac);

//  mesh.onReceive([](uint32_t from, String &msg) {
//    Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
//    // receivedData[0] = msg.toFloat();
//    Serial.print("Status of msg transmission: ");
//    Serial.println(mesh.sendSingle(dst_nodeName, msg));
//  });
  mesh.onReceive([](String &from, String &msg) {
    Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
    
    Serial.print("- - - REsending message: ");
    Serial.println("from = " + from + ", payload = " + msg);
    Serial.print("Status of REtransmission: ");
    Serial.println(mesh.sendSingle(dst_mac, msg));
    Serial.println("");
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

  mesh.setContainsRoot(true);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
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
}