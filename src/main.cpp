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
#define MQTT_Port 1883
#define MQTT_HearbeatDuration 1000
unsigned long MQTT_nowT;
// IPAddress dns(127, 0, 0, 54); old xapiens dns , no longer used in MKM site
EthernetClient ethClient;

// === TFT Pins ===
#define TFT_CS   33
#define TFT_DC   26
#define TFT_RST  27
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

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// === Button Pins ===
#define BUTTON1_PIN 36
#define BUTTON2_PIN 39
void connectToMQTT(){
  while(!mqttClient.connected()){
    Serial.println("Trying to connect to mqtt server");
    if (mqttClient.connect("ESP32Client",mqtt_username,mqtt_password)) {
      Serial.println("connected!");
      // Optionally, subscribe to a topic
      mqttClient.subscribe("test/topic");
    } else {
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

  tft.setTextColor(ILI9341_RED);
  tft.setCursor(10, 180);
  tft.println("To upload sketch, use OTA");

  // === BUTTON INIT ===
  pinMode(BUTTON1_PIN, INPUT); // Using external pull-up
  pinMode(BUTTON2_PIN, INPUT);

  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(10, 110);
  tft.print("Button 4: ");
  tft.setCursor(10, 140);
  tft.print("Button 5: ");
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

  // Display Button 4 State
  tft.setCursor(130, 110);
  tft.setTextColor(btn1Pressed ? ILI9341_GREEN : ILI9341_RED, ILI9341_BLACK); // text, background
  tft.print(btn1Pressed ? "PRESSED " : "RELEASE");

  // Display Button 5 State
  tft.setCursor(130, 140);
  tft.setTextColor(btn2Pressed ? ILI9341_GREEN : ILI9341_RED, ILI9341_BLACK);
  tft.print(btn2Pressed ? "PRESSED " : "RELEASE");
  mqttClient.loop();
  delay(100); // avoid flickering
}
