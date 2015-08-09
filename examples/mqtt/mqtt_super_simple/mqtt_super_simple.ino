#define MQTT_DEBUG_MODE
#define WIFI_DEBUG_MODE

#include <WiFiConnector.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <pt.h>


const char* ssid     = "NAT.WRTNODE";
const char* pass     = "devicenetwork";

MqttConnector *mqtt;
WiFiConnector *wifi;

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
  Serial.println("BEGIN");
  Serial.println("BEGIN");
}

void init_wifi()
{
  wifi = new WiFiConnector(ssid, pass);

  wifi->on_connecting([&](const void* message)
  {
    Serial.print("connecting ");
    Serial.println ((char*)message);
  });

  wifi->on_connected([&](const void* message)
  {
    Serial.print ("WIFI CONECTED: ");
    Serial.println ((char*)message);
  });

  wifi->on_disconnected([&](const void* message)
  {
    Serial.print ("WIFI DISCONECTED: ");
    Serial.print ("WIFI DISCONECTED: ");
    Serial.print ("WIFI DISCONECTED: ");
    Serial.println ((char*)message);
  });

  wifi->connect();

}

void init_mqtt()
{

    String organization = String("quickstart");
    String url = organization + ".messaging.internetofthings.ibmcloud.com";
    // mqtt = new MqttConnector(url.c_str());
    mqtt = new MqttConnector("cmmc.xyz");
  // mqtt = new MqttConnector("mqtt.tespa.io");



  mqtt->prepare_configuration([&](MqttConnector::Config *config) -> void {
    String macAddress = WiFi.macAddress();
    macAddress.toLowerCase();

    config->clientId  = String("d:quickstart:cmmakerclub.com:");
    config->clientId += macAddress;
    config->topicPub  = "iot-2/evt/status/fmt/json";
    String url  = String("https://quickstart.internetofthings.");
           url += "ibmcloud.com/#/device/"+ macAddress +"/sensor/";

    Serial.println(url);

  });

  mqtt->prepare_configuration(NULL);

  mqtt->prepare_data([&](JsonObject *root) -> void {
    JsonObject& data = root->at("d");
     data["myName"] = "NAT";
     IPAddress ip = WiFi.localIP();
     String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
     data["adc"] = analogRead(A0);
     data["ip"] = ipStr.c_str();
     data["RSSI"] = WiFi.RSSI();
  }, 8500);

  // mqtt->prepare_subscribe(NULL);

  mqtt->prepare_subscribe([&](MQTT::Subscribe *sub) -> void {
    // (sub)->add_topic("HELLO");
    // Serial.println("WHAT?");
  });

  // mqtt->manipulate_subscribe([&](MQTT::Subscribe **sub) -> void {
  //   // (sub)->add_topic("HELLO");
  //   // Serial.println("WHAT?");
  // });

  mqtt->after_prepare_data([&](JsonObject * root) -> void {
    JsonObject& data = root->at("d");
    //delete data["version"];
  });
 
  mqtt->on_message([&](const MQTT::Publish &pub) -> void {
    Serial.print("ON MESSAGE: ");
    Serial.print(pub.topic());
    Serial.print(" => ");
    Serial.println(pub.payload_string());
  });

  mqtt->connect();
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  mqtt->loop(wifi);
  //mqtt->sync_pub("0");
}