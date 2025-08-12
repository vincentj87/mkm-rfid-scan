// for rfid scanner standalone
#define NUT_RUNNER_OIL_PAN_PULLEY 
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#ifdef INTERNAL_XAPIENS
// === IP CONFIG XAPIENS TEST===
IPAddress ip(10, 74, 20, 75);
IPAddress mqttServer(10, 74, 20, 21); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(10, 74, 20, 254); // PC's IP
// IPAddress dns(10, 65, 104, 1);      // DNS server IP
// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0XEF};
#define heartbeat_topic "mkm/up/rfid/internal_xapiens/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/intenral_xapiens/telemetry/payload"
#define mqttClientName "internal_xapiens_rfid_scanner"
#endif

#ifdef NUT_RUNNER_BEARING_CAP //ip 63
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 63);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X63};
#define heartbeat_topic "mkm/up/rfid/bearing_cap/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/bearing_cap/telemetry/payload"
#define mqttClientName "nut_runner_bearing_cap_rfid_scanner"
#endif

#ifdef NUT_RUNNER_CONNECTING_PAD //ip 65
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 65);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X65};
#define heartbeat_topic "mkm/up/rfid/connecting_pad/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/connecting_pad/telemetry/payload"
#define mqttClientName "nut_runner_connecting_pad_rfid_scanner"
#endif

#ifdef NUT_RUNNER_OIL_SEAL //ip 66
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 66);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X66};
#define heartbeat_topic "mkm/up/rfid/oil_seal/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/oil_seal/telemetry/payload"
#define mqttClientName "nut_runner_oil_seal_rfid_scanner"
#endif

#ifdef NUT_RUNNER_OIL_PAN_PULLEY //ip 67
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 67);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X67};
#define heartbeat_topic "mkm/up/rfid/oil_panl/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/oil_pan/telemetry/payload"
#define mqttClientName "nut_runner_oil_pana_pulley_rfid_scanner"
#endif

#ifdef NUT_RUNNER_FLY //ip 68
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 68);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X68};
#define heartbeat_topic "mkm/up/rfid/oil_seal/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/oil_seal/telemetry/payload"
#define mqttClientName "nut_runner_fly_rfid_scanner"
#endif


#ifdef GASKET_SELECTOR//ip 69
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 6);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X69};
#define heartbeat_topic "mkm/up/rfid/oil_seal/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/oil_seal/telemetry/payload"
#define mqttClientName "nut_runner_gasket_selector_rfid_scanner"
#endif


#ifdef CYLINDER_HEAD //ip 70
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 70);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X67};
#define heartbeat_topic "mkm/up/rfid/c/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/cylinder_head/telemetry/payload"
#define mqttClientName "nut_runner_cylinder_head_pulley_rfid_scanner"
#endif



#ifdef NUT_RUNNER_OIL_SEAL_PULLEY //ip 71
// === IP CONFIG ===
IPAddress ip(172, 19, 16, 71);
IPAddress mqttServer(172,19,16,243); // PC's IP MQTT SERVER
IPAddress subnet(255, 255, 254, 0);
IPAddress gateway(172,19,17,254); // PC's IP

// === Ethernet Settings !!! CHANGE LAST BYTE SAME AS IP IN MKM===
EthernetClient ethClient;
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0X67};
#define heartbeat_topic "mkm/up/rfid/oil_seal/telemetry/heartbeat"
#define payload_topic "mkm/up/rfid/oil_seal/telemetry/payload"
#define mqttClientName "nut_runner_oil_seal_pulley_rfid_scanner"
#endif



// === MQTT CONFIG ===
unsigned long MQTT_nowT = 0; // var for heartbeat
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
#define MQTT_Port 1883
PubSubClient mqttClient(mqttServer, MQTT_Port, callback, ethClient);
const char *mqtt_username = "admin";
const char *mqtt_password = "admin";

void connectToMQTT()
{
  while (!mqttClient.connected())
  {
    Serial.println("Trying to connect to mqtt server");
    if (mqttClient.connect(mqttClientName, mqtt_username, mqtt_password))
    {
      Serial.println("connected!");
      // Optionally, subscribe to a topic
      mqttClient.subscribe("test/topic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(3000);
    }
  }
}
void heartBeatRoutine()
{
  if ((MQTT_nowT + 3000) <= millis())
  {
    MQTT_nowT = millis();

    mqttClient.publish(heartbeat_topic,"{\"heartbeat\":1}");
  }
}
// === RFID Settings ===
#include "UNIT_UHF_RFID.h"
#define RFID_POLLING_COUNT 20
HardwareSerial SerialRFID(1);
Unit_UHF_RFID uhf;
String rfidReadResult = "";
unsigned long lastReadRFID = 0;
uint8_t result = 0;
void rfidSetup()
{
  uhf.begin(&SerialRFID, 115200, 16, 17, false);
  uhf.setTxPower(2600);
  delay(1000);
  if (uhf.getVersion() != "ERROR")
  {
    //!!!!do something with led
    Serial.println("RFID Module initialized successfully.");
  }
  else
  {
    //!!!!do something with led
    Serial.println("Failed to initialize RFID Module.");
  }
}
void rfidRead()
{
  if((lastReadRFID+100)<=millis()) // Prevent reading too often
  {
    
  result = uhf.pollingMultiple(RFID_POLLING_COUNT);
  if (result > 0)
  {
    Serial.println("from rfidRead(), there is " + String(result) + " cards read.");
    rfidReadResult = "";
    for (uint8_t i = 0; i < result; i++)
    {
      rfidReadResult = uhf.cards[i].epc_str;
      Serial.println("Card EPC: " + rfidReadResult);
    }
    if(rfidReadResult!=""){//!!!!! add checking if the frist digit is correct

      mqttClient.publish(payload_topic, rfidReadResult.c_str());
      rfidReadResult = ""; // Clear after sending
    }
  }
  lastReadRFID = millis();
  }
}
void setup()
{
  Serial.begin(115200);
  delay(500);
  // === RFID INIT ===
  rfidSetup();
  // === Ethernet INIT ===
  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip,gateway, subnet);
  if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("⚠️ Ethernet cable not connected!");
  }
  else
  {
    Serial.println("✅ Ethernet connected!");
  }
  // === OTA INIT ===
  ArduinoOTA.begin(Ethernet.localIP(), "arduino", "password", InternalStorage);
  // === MQTT BEGIN ===
  mqttClient.setServer(mqttServer, MQTT_Port);
  Serial.print("temporary : exit setup");
}
void loop()
{
  if (!mqttClient.connected())
  {
    connectToMQTT();
  
  }
  else
  {
    rfidRead();
    heartBeatRoutine();
  }
  //
  // Handle OTA updates
  ArduinoOTA.handle();
  // Handle MQTT client
  mqttClient.loop();
  delay(100);
}