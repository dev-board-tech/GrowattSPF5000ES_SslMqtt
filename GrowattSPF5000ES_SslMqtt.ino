#include "config.h"
#include "WiFiClientSecure.h"
#include <arduFPGA-app-common-arduino.h>
#if USE_PUB_SUB_LIB
#include <PubSubClient.h>
#else
#include <ArduinoMqttClient.h>
#endif
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <Rtc_Pcf8563.h>
#include "crt.h"
#include "credentials.h"
#include "apiCtl.h"
#include "pv.h"


// MQTT Broker credentials
const char* mqtt_broker = "192.168.1.3";
const char* outTopicKeepAlive = "/alive";
String inTopic = "clients/5E5C3F676D223F84/home_ctl/" MODULE_NAME "/" GROUP_NR "/" MODULE_NR;
bool clientConnected = false;
bool wifiClientConnectedLast = false;
bool mqttClientConnectedLast = false;
#if USE_SECURE_MQTT
const int mqtt_port = 8883 ;
#else
const int mqtt_port = 1883 ;
#endif
const char* brokerUsername = BROKER_USERNAME;
const char* brokerPassword = BROKER_PASSWORD;

// Set ssl certificate
const char* root_ca =  caCrt;
const char* server_cert = serverCrt;
const char* server_key  = serverKey;

const uint8_t mqttCertFingerprint[] = CERT_FINGERPRINT;

#if USE_SECURE_MQTT
BearSSL::X509List cert(caCrt);
BearSSL::WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif
#if USE_PUB_SUB_LIB
PubSubClient client(espClient);
#else
MqttClient client(espClient);
#endif

const char* host = OTA_NAME;
const char* otaPass = OTA_PASS;
wiFiCredentials_t wiFiCredentials[] = WIFI_CREDENTIALS;

bool wifiConnectStatusState = false;
int wiFiCredentialsCnt = 0;
int wiFiConnectRetryCnt = CONNECT_RETRY_COUNT;
sTimer wifiConnectingTimer(500);
sTimer keepAlive_Timer(1000);
sTimer pvRadTimeout_Timer(10000);
uint64_t pId = 0;

char tmpBuf[64];

int _progress = 0;

Rtc_Pcf8563 rtc;
// Define NTP Client to get time
WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org");
NTPClient timeClient(ntpUDP, "192.168.1.3");
long refreshCnt = 0;

#if USE_PUB_SUB_LIB
void callback(char* topic, byte* payload, unsigned int length) {
#else
void callback(int length) {
  String s = client.messageTopic();
  const char *topic = s.c_str();
  byte payload[length];
  client.read(payload, length);
#endif
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    return;
  }
  //serialResponse(topic, tmpPayload);
  String iTop = inTopic;
  iTop += "/response";
  String er = "{err: ";
  if(!doc["outputConfig"].isNull()) {
    int value = doc["outputConfig"].as<String>().toInt();
    if(value < 4 && !PV_GetBusy()) {
      PV_WriteData(DEVICE_ID, 0x01, value);
      String ot = "{outputConfig: ";
      ot += value;
      ot += "}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), ot.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(ot.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
    } else {
      er += "\"outputConfig\"}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), er.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(er.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
     }
  } else if(!doc["chargeConfig"].isNull()) {
    int value = doc["chargeConfig"].as<String>().toInt();
    if(value < 3 && !PV_GetBusy()) {
      PV_WriteData(DEVICE_ID, 0x02, value);
      String ot = "{chargeConfig: ";
      ot += value;
      ot += "}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), ot.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(ot.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
     } else {
      er += "\"chargeConfig\"}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), er.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(er.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
     }
  } else if(!doc["acChargeCurrent"].isNull()) {
    int value = doc["acChargeCurrent"].as<String>().toInt();
    if(value <= 100 && !PV_GetBusy()) {
      PV_WriteData(DEVICE_ID, 99, value);
      String ot = "{acChargeCurrent: ";
      ot += value;
      ot += "}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), ot.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(ot.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
     } else {
      er += "\"acChargeCurrent\"}";
#if USE_PUB_SUB_LIB
  client.publish(iTop.c_str(), er.c_str(), false);
  client.flush();
#else
  client.beginMessage(iTop.c_str());
  client.print(er.c_str());
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
     }
  } else if(!doc["restart"].isNull()) {
    if(doc["restart"].as<String>().equals("yes")) {
      ESP.restart();
    }
  }
}

void mqttConnect() {
  if (!client.connected()) {
    String client_id = PSTR("hallCtl_");
    client_id += String(WiFi.macAddress());
#if USE_PUB_SUB_LIB
    if (client.connect( client_id.c_str(), brokerUsername, brokerPassword)) {
#else
    client.setId(client_id.c_str());
    client.setUsernamePassword(brokerUsername, brokerPassword);
    if(client.connect(mqtt_broker, mqtt_port)) {
#endif
      /*while(Serial.available()) {
        Serial.read();
      }*/
      //client.subscribe(inTopic);
      String t = inTopic + "/#";
      client.subscribe(t.c_str());

/*#if USE_PUB_SUB_LIB
  client.publish(inTopic.c_str(), MDNS_NAME ": Connacted", false);
  client.flush();
#else
  client.beginMessage(inTopic.c_str());
  client.print(MDNS_NAME ": Connacted");
  client.endMessage();
  client.flush();
#endif*/
    }
  }
}

void setup() {
  // Set software serial baud to 115200;
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  Serial.begin(9600);
  Wire.begin(5, 4);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LED_OFF);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LED_OFF);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LED_OFF);
  PV_Init();
  PV_triggerDataRead();
  pvRadTimeout_Timer.Start();
  wifiConnectingTimer.Start();
}

void loop() {
  PV_Loop();
  delay(1);
  if(!inTopic.isEmpty() && client.connected() && clientConnected != client.connected()) {
    clientConnected = client.connected();
  }
  if(!client.connected() && clientConnected != client.connected()) {
    clientConnected = client.connected();
  }
  if(wifiClientConnectedLast != WiFi.isConnected() || mqttClientConnectedLast != client.connected()) {
    wifiClientConnectedLast = WiFi.isConnected();
    mqttClientConnectedLast = client.connected();
    /*JsonDocument doc1;
    String output;
    doc1["wifiConnected"] = WiFi.isConnected();
    doc1["mqttConnected"] = client.connected();
    doc1["rssi"] = WiFi.RSSI();
    serialResponseStatus(doc1, "connectionChanged");*/
  }
  switch (SchetchWiFiState) {
    case DISCONNECTED:
      digitalWrite(LED_GREEN, LED_OFF);
      WiFi.begin(wiFiCredentials[wiFiCredentialsCnt].ssid, wiFiCredentials[wiFiCredentialsCnt].password);
      SchetchWiFiState = CONNECTING;
      wiFiCredentialsCnt++;
      if(wiFiCredentials[wiFiCredentialsCnt].ssid[0] == 0) {
        wiFiCredentialsCnt = 0;
      }
      wiFiConnectRetryCnt = CONNECT_RETRY_COUNT;
      break;
    case CONNECTING:
      if(wifiConnectingTimer.Tick()) {
        wifiConnectStatusState = !wifiConnectStatusState;
        digitalWrite(LED_GREEN, wifiConnectStatusState);
        if(!wiFiConnectRetryCnt--) {
          SchetchWiFiState = DISCONNECTED;
        }
      }
      if (WiFi.status() == WL_CONNECTED) {
        SchetchWiFiState = CONNECTED;
        digitalWrite(LED_GREEN, LED_ON);
        // Print the IP address
        delay(100);
        configTime(3 * 3600, 0, "192.168.1.1", "192.168.1.1");
        //configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

        //Connecting to a mqtt broker width ssl certification
        //espClient.setCACert(root_ca);
        //espClient.setCertificate(server_cert);  // for client verification
        //espClient.setPrivateKey(server_key);    // for client verification
#if USE_SECURE_MQTT
        espClient.setTrustAnchors(&cert);
        espClient.setFingerprint(mqttCertFingerprint);
        BearSSL::X509List *serverCertList = new BearSSL::X509List(serverCrt);
        BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(serverKey);
        espClient.setClientRSACert(serverCertList, serverPrivKey);
#endif

        // Connect to the MQTT Broker remotely
#if USE_PUB_SUB_LIB
        client.setServer(mqtt_broker, mqtt_port);
        client.setCallback(callback);
        client.setBufferSize(1800);
#else
        client.onMessage(callback);
        client.setTxPayloadSize(1800);
#endif


        // Port defaults to 8266
        // ArduinoOTA.setPort(8266);

        // Hostname defaults to esp8266-[ChipID]
        ArduinoOTA.setHostname(host);

        // No authentication by default
        // ArduinoOTA.setPassword("admin");

        // Password can be set with it's md5 value as well
        // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
        // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
        ArduinoOTA.setPassword(otaPass);

        ArduinoOTA.onStart([]() {
          /*String type;
          if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
          else // U_SPIFFS
            type = "filesystem";
          // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
          _progress = -1;*/
        });
        ArduinoOTA.onEnd([]() {
          //ESP.restart();
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          /*if(_progress != (progress / (total / 100))) {
            _progress = (progress / (total / 100));
            //reportPrintF("Progress: %u%%\r", (progress / (total / 100)));
          }*/
          //ESP.wdtFeed();
        });
        ArduinoOTA.onError([](ota_error_t error) {
          /*reportPrintF("Error[%u]: ", (int)error);
          if (error == OTA_AUTH_ERROR) reportAppendLn((char *)"Auth Failed");
          else if (error == OTA_BEGIN_ERROR) reportAppendLn((char *)"Begin Failed");
          else if (error == OTA_CONNECT_ERROR) reportAppendLn((char *)"Connect Failed");
          else if (error == OTA_RECEIVE_ERROR) reportAppendLn((char *)"Receive Failed");
          else if (error == OTA_END_ERROR) reportAppendLn((char *)"End Failed");*/
        });
        ArduinoOTA.begin();

        timeClient.begin();
        // Set offset time in seconds to adjust for your timezone, for example:
        // GMT +1 = 3600
        // GMT +8 = 28800
        // GMT -1 = -3600
        // GMT 0 = 0
        timeClient.setTimeOffset(2*3600);
        if(timeClient.update()) {
          time_t epochTime = timeClient.getEpochTime();
          struct tm *ptm = gmtime ((time_t *)&epochTime);
          rtc.initClock();
          //day, weekday, month, century(1=1900, 0=2000), year(0-99)
          rtc.setDate(ptm->tm_mday, ptm->tm_wday, ptm->tm_mon+1, 1, ptm->tm_year - 100);
          //hr, min, sec
          rtc.setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
        }
        rtc.getDate();
        rtc.getTime();
        struct tm t;
        bzero(&t, sizeof(t));
        t.tm_year = rtc.getYear() + 100;
        t.tm_mon  = rtc.getMonth();
        t.tm_mday = rtc.getDay();
        t.tm_hour = rtc.getHour();
        t.tm_min  = rtc.getMinute();
        t.tm_sec  = rtc.getSecond();
        time_t time = mktime(&t);
        struct timeval tVal;
        tVal.tv_sec = time;
        tVal.tv_usec = 0;
        settimeofday(&tVal, nullptr);


        mqttConnect();
        keepAlive_Timer.Start();
        //client.setBufferSize(1800);
      }
      break;
    case CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        SchetchWiFiState = DISCONNECTED;
        keepAlive_Timer.Stop();
      } else {
#if USE_PUB_SUB_LIB
        client.loop();
#else
        client.poll();
#endif
        mqttConnect();
        /*if(keepAlive_Timer.Tick()) {
          keepAlive_Timer.Stop();
          if(client.connected() && !inTopic.isEmpty()) {
            JsonDocument doc;
            String tmpJson;
            char b[32];
            snprintf(b, sizeof(b), "%02u:%02u:%02u %02u/%02u/20%02u", 
                rtc.getHour(), rtc.getMinute(), rtc.getSecond(),
                rtc.getDay(), rtc.getMonth(), rtc.getYear());
            doc["time"] = b;
            doc["rssi"] = WiFi.RSSI();
            doc["timestamp"] = millis();
            serializeJson(doc, tmpJson);
            String t = inTopic + outTopicKeepAlive;
            client.publish(t.c_str(), tmpJson.c_str(), false);
            client.flush();
            pId++;
          }
          keepAlive_Timer.Start();
        }*/
        if(client.connected() && !inTopic.isEmpty()) {
          if(refreshCnt != PV_getDataFefreshCnt()) {
            refreshCnt = PV_getDataFefreshCnt();
            char buf[1800];
            //strcpy(buf, "{\"TemporaryData\":1}");
            mqttResponseLog(&rtc, buf);
#if USE_PUB_SUB_LIB
  client.publish(inTopic.c_str(), buf, false);
  client.flush();
#else
  client.beginMessage(inTopic.c_str());
  client.print(buf);
  client.endMessage();
  client.flush();
#endif
      //client.publish(iTop.c_str(), ot.c_str(), false);
      //client.flush();
             pId++;
            //PV_triggerDataRead();
            //pvRadTimeout_Timer.Start();
          }
        }
        if(pvRadTimeout_Timer.Tick()) {
            PV_triggerDataRead();
        }
      }
      break;
  }
  ESP.wdtFeed();
  ArduinoOTA.handle();
}
