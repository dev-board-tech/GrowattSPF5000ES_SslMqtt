#include <cstdint>
#ifndef __CONFIG_H___
#define __CONFIG_H___

#include "credentials.h"


//#include "LITTLEFS.h"
#include <LittleFS.h>
#define SPIFFS LittleFS

#define MODULE_ID_1       1
#define MODULE_ID_2       2
#define MODULE_ID         MODULE_ID_2

#define USE_PUB_SUB_LIB  true
#define USE_SECURE_MQTT   false

#define USE_MESH
//ERROR | MESH_STATUS | CONNECTION | SYNC |
// COMMUNICATION | GENERAL | MSG_TYPES | REMOTE
#define MESH_DBG_LEVEL                  ERROR
#define MESH_PREFIX "morgothHomeCtlMeshNet"
#define MESH_PASSWORD "7hcqyi8u43XOT8ITYD3WJRKIAXCUY8AW3"
#define MESH_PORT 6666

#ifndef LED_BUILTIN
#define LED_BUILTIN 22
#endif

typedef struct {
  char *ssid;
  char *password;
}wiFiCredentials_t;

#define CONNECT_RETRY_COUNT 10// Each take half a second

enum {
  DISCONNECTED,
  CONNECTING,
  CONNECTED
} SchetchWiFiState = DISCONNECTED;

typedef enum {
  REASON_NONE,
  POWERED_UP,
  RESET,
  WIFI_DISCONNECT,
  MQTT_DISCONNECT
}connectReason;

typedef enum {
  LED_OFF,
  LED_ON,
}LED_STATE_e;

#define LED_BLUE     16// BLUE
#define LED_RED      2 // RED
#define LED_GREEN     0 // GREEN

typedef struct {
  int year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  bool err;
}webTimeSet_t;

#define CONCAT(x, y) x##y
#define STRINGIFY(x) #x

#define GROUP_NR          "1"
#define MODULE_NAME       "GROWATT_SPF5000ES"

#if MODULE_ID == MODULE_ID_1
#define MODULE_NR  "1"
#define MDNS_NAME "growatt-5000-1"
#define OTA_NAME "GROWATT-5000-1"
#elif MODULE_ID == MODULE_ID_2
#define MODULE_NR  "2"
#define MDNS_NAME "growatt-5000-2"
#define OTA_NAME "GROWATT-5000-2"
#endif

#define TIME_ZONE 3

#define REPORT_BUFF_LEN 4096

#define STATION_SSID     "MorgothAp"
#define STATION_PASSWORD "maricica19"
#define HOSTNAME "HTTP_BRIDGE"

#define SERIAL_MAX_RECEIVE  1024

#endif