// https://github.com/vincentj87/mkm-io.git
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <PubSubClient.h>
#include "UNIT_UHF_RFID.h"
// === Ethernet Settings ===
#define ETH_SPI_SCS 5  // W5500 CS
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
IPAddress ip(172,19,16,105);
IPAddress gateway(172,19,16,254);
IPAddress subnet(255, 255,255,255);
IPAddress mqttServer(172,19,16,102);
// === RFID Settings ===
HardwareSerial SerialRFID(1);
Unit_UHF_RFID uhf;
// === Bluetooth Settings ===
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
uint8_t kassen_mac[] = { 0xDA, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }; // Example MAC address for the RFID reader
unsigned long lastReadRFID = 0;
// === MQTT Settings ===
#define MQTT_Port 1883
#define MQTT_HearbeatDuration 1000
unsigned long MQTT_nowT;
// IPAddress dns(127, 0, 0, 54); old xapiens dns , no longer used in MKM site
EthernetClient ethClient;

// === TFT Pins ===
#define TFT_CS   33
#define TFT_DC   26
#define TFT_RST  27
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// === Button Pins ===
#define BUTTON1_PIN 36
#define BUTTON2_PIN 39
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
PubSubClient mqttClient(mqttServer,MQTT_Port,callback,ethClient);
const char* mqtt_username ="admin";
const char* mqtt_password = "admin";
void rfidRead(){
  if((millis()-lastReadRFID)>100){
   uint8_t result = uhf.pollingMultiple(20);
   if(result>0){
    for (uint8_t i = 0; i < result; i++) {
          // mqttClient.publish("RFID1/","1");
            Serial.println("pc: " + uhf.cards[i].pc_str);
            Serial.println("rssi: " + uhf.cards[i].rssi_str);
            Serial.println("epc: " + uhf.cards[i].epc_str);
            Serial.println("-----------------");

   }

  }
  lastReadRFID = millis();
  }
}
void rfidShowTFT(){
  // Clear below text lines instead
  tft.fillRect(0, 160, 320, 40, ILI9341_BLACK);  // 240 wide, 40px tall (160 to 200)
  tft.setCursor(10,160);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("RFID: ");
  tft.setCursor(10,180);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(uhf.cards[0].epc_str);
}
void rfidSetup(){
  uhf.begin(&SerialRFID, 115200, 16, 17, false);
  uhf.setTxPower(2600);
  delay(1000);
  if(uhf.getVersion() != "ERROR") {
    tft.fillRect(0, 130, 320, 30, ILI9341_BLACK); 
    tft.setCursor(10,130);
    tft.setTextColor(ILI9341_GREEN);
    tft.print("RFID OK");
    Serial.println("RFID Module initialized successfully.");
  } else {
    tft.fillRect(0, 130, 320, 40, ILI9341_BLACK); 
    tft.setCursor(10,130);
    tft.setTextColor(ILI9341_RED);
    tft.print("RFID ERROR");
    Serial.println("Failed to initialize RFID Module.");
  }
}
void connectToMQTT(){
  while(!mqttClient.connected()){
    Serial.println("Trying to connect to mqtt server");
    if (mqttClient.connect("ESP32Client",mqtt_username,mqtt_password)) {
      Serial.println("connected!");
      tft.fillRect(0, 100, 320, 20, ILI9341_BLACK); 
      tft.setCursor(10, 100);
      tft.setTextColor(ILI9341_GREEN);
      tft.print("MQTT: OK"          );
      mqttClient.subscribe("test/topic");
    } else {
      tft.fillRect(0, 100, 320, 20, ILI9341_BLACK); 
      tft.setCursor(10, 100);
      tft.setTextColor(ILI9341_RED);
      tft.print("MQTT: DISCONNECTED");
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 3 seconds");
      // ArduinoOTA.poll();
      delay(3000);
    }
  }
}
void heartBeatRoutine(){
 if((MQTT_nowT+3000)<=millis()){
  MQTT_nowT=millis();
  mqttClient.publish("heartbeat","1");
 }
}


  
void setup() {
  Serial.begin(115200);
  delay(1000);

  // === TFT INIT ===
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.println("MKM PAIRING STATION");
  // === RFID INIT ===
  rfidSetup();
  // === Ethernet INIT ===
  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip, gateway, subnet);

  tft.setCursor(10, 40);
  if (Ethernet.linkStatus() == LinkOFF) {
    tft.println("Ethernet: FAIL");
    Serial.println("⚠️ Ethernet cable not connected!");
  } else {
    tft.println("Ethernet: OK");
    Serial.println("✅ Ethernet connected!");
  }

  // === OTA INIT ===
  ArduinoOTA.begin(Ethernet.localIP(), "arduino", "password", InternalStorage);
  // === QTT INIT ===
  mqttClient.setServer(mqttServer,MQTT_Port);
  Serial.println("OTA Ready");

  tft.setCursor(10, 70);
  tft.print("OTA IP: ");
  tft.println(Ethernet.localIP());

  // === BUTTON INIT ===
  pinMode(BUTTON1_PIN, INPUT); // Using external pull-up
  pinMode(BUTTON2_PIN, INPUT);

  
}

void loop() {
  if(!mqttClient.connected()){
    connectToMQTT();
    
  }
    
  // ArduinoOTA.poll();
  // connectToMQTT();
  
  if(mqttClient.connected()){
    heartBeatRoutine();
  }
  
  bool btn1Pressed = digitalRead(BUTTON1_PIN) == LOW;
  bool btn2Pressed = digitalRead(BUTTON2_PIN) == LOW;

  rfidRead();
  rfidShowTFT();

  mqttClient.loop();
  delay(100); // avoid flickering
}
