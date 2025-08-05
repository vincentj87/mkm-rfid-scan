// https://github.com/vincentj87/mkm-io.git

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// === Ethernet Settings ===
#define ETH_SPI_SCS 5 // W5500 CS
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF};
// === ONE SEGMENT WITH SERVER CONFIG IP ===
// IPAddress ip(172,17,210,234);
// IPAddress gateway(172,17,210,254);
// IPAddress subnet(255, 255,255,0);
// IPAddress mqttServer(172,17,210,59);

// === ONE SEGMENT WITH SWITCH CONFIG IP ===
IPAddress ip(172, 19, 16, 65);
IPAddress gateway(172, 19, 17, 254);
IPAddress subnet(255, 255, 254, 0);
IPAddress mqttServer(172, 19, 16, 243);
EthernetClient ethClient;
// === RFID Settings ===
#include "UNIT_UHF_RFID.h"
HardwareSerial SerialRFID(1);
Unit_UHF_RFID uhf;
String rfidReadResult = "TEST123";
// === Bluetooth Settings ===
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
bool isBarcodeConnected = false, prevBarcodeConnected = false;
String barcodeReadResult = "";
// aa:a8:02:05:55:bd kassen mac
uint8_t kassen_mac[] = {0xAA, 0xA8, 0x02, 0x05, 0x55, 0xBD}; // Example MAC address for the RFID reader
unsigned long lastReadRFID = 0;

// === MQTT Settings ===
#define MQTT_Port 1883
#define MQTT_HearbeatDuration 1000
void callback(char *topic, byte *payload, unsigned int length);
PubSubClient mqttClient(mqttServer, MQTT_Port, callback, ethClient);
const char *mqtt_username = "admin";
const char *mqtt_password = "admin";
unsigned long MQTT_nowT;
// IPAddress dns(127, 0, 0, 54); old xapiens dns , no longer used in MKM site

// === TFT Pins ===
#define TFT_CS 33
#define TFT_DC 26
#define TFT_RST 27
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// === Button Pins ===
#define BUTTON1_PIN 36
#define BUTTON2_PIN 39
bool button1WasPressed = false,button2WasPressed=false;

unsigned long nowT = 0;

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
void barcodeMantain()
{

  if (!SerialBT.connected())
  {
    prevBarcodeConnected = false;
    tft.setTextSize(2);
    Serial.println("Attempting to connect to Zebra scanner...");
    SerialBT.connect(kassen_mac);
    Serial.println("Failed to connect. Retrying in 5 seconds...");
    tft.fillRect(0, 40, 320, 16, ILI9341_BLACK);
    tft.setCursor(10, 40);
    tft.setTextColor(ILI9341_RED);

    tft.print("KASSEN : DISCONNECTED");
  }
  else
  {
    if (prevBarcodeConnected == false)
    {

      tft.setTextSize(2);
      Serial.println("Connected to scanner!");
      tft.fillRect(0, 40, 320, 16, ILI9341_BLACK);
      tft.setCursor(10, 40);
      tft.setTextColor(ILI9341_GREEN);
      tft.print("KASSEN : CONNECTED");
      prevBarcodeConnected = true;
    }
  }
}

void barcodeBegin()
{

  SerialBT.begin("MKM_Pairing_Station", true);
  Serial.println("Bluetooth initialized");

  Serial.println("Waiting for Zebra scanner connection...");
  tft.fillRect(0, 40, 320, 16, ILI9341_BLACK);
  tft.setCursor(10, 40);
  tft.setTextColor(ILI9341_RED);
  tft.print("KASSEN : CONNECTING");
  SerialBT.connect(kassen_mac);

}
void barcodeShowTFT()
{
  // Clear below text lines instead
  tft.fillRect(0, 190, 320, 40, ILI9341_BLACK); // 240 wide, 40px tall (160 to 200)
  tft.setCursor(10, 190);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("BRCD: ");
  tft.setCursor(90, 190);
  tft.setTextColor(ILI9341_WHITE);
  // tft.print(uhf.cards[0].epc_str);
  tft.print(barcodeReadResult);
}
void barcodeRead()
{
  if ((millis() - nowT) > 100)
  {
    if (SerialBT.available())
    {
      barcodeReadResult = "";
      while (SerialBT.available() > 0)
      {
        char c = SerialBT.read();
        if (c == '\n' || c == '\r')
        {
          // End of barcode
          break;
        }
        barcodeReadResult += c;
      }
      barcodeReadResult.trim(); // Remove any leading/trailing whitespace
      Serial.println("Barcode: " + barcodeReadResult);
      barcodeShowTFT();
      // mqttClient.publish("barcode", barcode.c_str());
      nowT = millis();
    }
  }
}
void rfidShowTFT()
{
  // Clear below text lines instead
  tft.fillRect(0, 160, 320, 30, ILI9341_BLACK); // 240 wide, 40px tall (160 to 200)
  tft.setCursor(10, 160);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("RFID: ");
  tft.setCursor(90, 160);
  tft.setTextColor(ILI9341_WHITE);
  // tft.print(uhf.cards[0].epc_str);
  tft.print(rfidReadResult);
}
void rfidRead()
{
  if ((millis() - lastReadRFID) > 100)
  {
    rfidShowTFT();
    uint8_t result = uhf.pollingMultiple(20);
    // for test 
    rfidReadResult="TEST";
    if (result > 0)
    {
      for (uint8_t i = 0; i < result; i++)
      {
        Serial.println("pc: " + uhf.cards[i].pc_str);
        Serial.println("rssi: " + uhf.cards[i].rssi_str);
        Serial.println("epc: " + uhf.cards[i].epc_str);
        Serial.println("-----------------");
        rfidReadResult = uhf.cards[i].epc_str;
      }
      //  rfidShowTFT();
    }

    // String barcode="";

    // while(SerialRFID.available()){
    //  uint8_t c= SerialRFID.read();

    // }
    lastReadRFID = millis();
  }
}
void dataUpdate()
{
  String rfidData = "";
  String barcodeData = "";
  rfidData = uhf.cards[0].epc_str;
  barcodeData = "";
}

void rfidSetup()
{
  uhf.begin(&SerialRFID, 115200, 16, 17, false);
  uhf.setTxPower(2600);
  delay(1000);
  if (uhf.getVersion() != "ERROR")
  {
    tft.fillRect(0, 70, 320, 30, ILI9341_BLACK);
    tft.setCursor(10, 70);
    tft.setTextColor(ILI9341_GREEN);
    tft.print("RFID OK");
    Serial.println("RFID Module initialized successfully.");
  }
  else
  {
    tft.fillRect(0, 70, 320, 40, ILI9341_BLACK);
    tft.setCursor(10, 70);
    tft.setTextColor(ILI9341_RED);
    tft.print("RFID ERROR");
    Serial.println("Failed to initialize RFID Module.");
  }
}
void connectToMQTT()
{
  while (!mqttClient.connected())
  {
    Serial.println("Trying to connect to mqtt server");
    if (mqttClient.connect("ESP32Client", mqtt_username, mqtt_password))
    {
      Serial.println("connected!");
      tft.fillRect(0, 100, 320, 20, ILI9341_BLACK);
      tft.setCursor(10, 100);
      tft.setTextColor(ILI9341_GREEN);
      tft.print("MQTT: OK");
      mqttClient.subscribe("test/topic");
    }
    else
    {
      tft.fillRect(0, 100, 320, 20, ILI9341_BLACK);
      tft.setCursor(10, 100);
      tft.setTextColor(ILI9341_RED);
      tft.print("MQTT: DISCONNECTED");
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 3 seconds");
      
      delay(3000);
    }
  }
}
void heartBeatRoutine()
{
  if ((MQTT_nowT + 3000) <= millis())
  {
    MQTT_nowT = millis();

    mqttClient.publish("mkm/up/scanner/rfid/heartbeat", "{\"heartbeat\":1}");
  }
}
void HomePage()
{

  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.println("MKM PAIRING STATION");

  tft.setCursor(10, 40);
  tft.print("OTA IP: ");
  tft.println(Ethernet.localIP());
  delay(3000);
  tft.fillScreen(ILI9341_BLACK);
}
void buttonInit()
{
  pinMode(BUTTON1_PIN, INPUT); // Using external pull-up
  pinMode(BUTTON2_PIN, INPUT);
}
void mqttStatusTFT(){
  if(!mqttClient.connected()){
    tft.fillRect(0, 130, 320, 20, ILI9341_BLACK);
      tft.setCursor(10, 130);
      tft.setTextColor(ILI9341_RED);
      tft.print("MQTT: NOT CONNECTED");
  }else{
    tft.fillRect(0, 130, 320, 20, ILI9341_BLACK);
      tft.setCursor(10, 130);
      tft.setTextColor(ILI9341_GREEN);
      tft.print("MQTT: CONNECTED");
  }
}
void buttonRoutine()
{ 
  if(digitalRead(BUTTON2_PIN)==LOW){
    if (!button2WasPressed){
      rfidReadResult="";
      barcodeReadResult="";
      rfidShowTFT();
      barcodeShowTFT();
    }
    button2WasPressed=true;
  }else{
    button2WasPressed=false;
  }
  if (digitalRead(BUTTON1_PIN) == LOW)
  {
    if (!button1WasPressed)
    {
      button1WasPressed = true;
      if (rfidReadResult != "" and barcodeReadResult != "")
      {
        // formating to json format
        StaticJsonDocument<200> doc;
        doc["rfid"] = rfidReadResult;
        doc["barcode"] = barcodeReadResult;
        String jsonString;
        serializeJson(doc, jsonString);
        Serial.println("Sending data to MQTT: " + jsonString);
        if (mqttClient.connected())
        {
          mqttClient.publish("mkm/up/scanner/rfid/telemetry", jsonString.c_str());
          tft.fillRect(0, 120, 320, 20, ILI9341_BLACK);
          tft.setCursor(10, 120);
          tft.setTextColor(ILI9341_GREEN);
          tft.print("Data sent to MQTT");
        }
        else
        {
          tft.fillRect(0, 120, 320, 20, ILI9341_BLACK);
          tft.setCursor(10, 120);
          tft.setTextColor(ILI9341_RED);
          tft.print("MQTT not connected");
        }
      }
    }
  }
  else
  {
    button1WasPressed = false;
  }
}

void ethCheckLink()
{
  tft.setCursor(10, 10);
  if (Ethernet.linkStatus() == LinkOFF)
  {
    tft.println("Ethernet: FAIL");
    Serial.println("⚠️ Ethernet cable not connected!");
  }
  else
  {
    tft.println("Ethernet: OK");
    Serial.println("✅ Ethernet connected!");
  }
}
void RFIDRoutine(void *parameter)
{
  while (1)
  {
    rfidRead();

    vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust delay as needed
  }
}
void OTAUpdate(void *parameter)
{
  while (1)
  {
    ArduinoOTA.handle();
    vTaskDelay(100/ portTICK_PERIOD_MS); // Adjust delay as needed
  }
}
void MainLoop(void *parameter)
{
  while (1)
  {
   
    buttonRoutine();
    barcodeMantain();
    barcodeRead();

    if (mqttClient.connected())
    {
      heartBeatRoutine();
      
    }else{
      connectToMQTT();
    }
    

    mqttClient.loop();
    vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust delay as needed
  }
}
void setup()
{
  Serial.begin(115200);
  delay(1000);
 
  // === TFT INIT ===
  Serial.println("TFT Begin");
  tft.begin();

  // === Ethernet INIT ===
  Serial.println("ETH init");
  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip, gateway, subnet);
  
  // === HOME PAGE ===
  Serial.println("Home page");
  HomePage();

  // === ETHERNET STATUS ===
  Serial.println("Ethernet check");
  ethCheckLink();
  // === RFID INIT ===
  Serial.println("Rfid init ");
  rfidSetup();
  // === OTA INIT ===
  Serial.println("Ota init");
  ArduinoOTA.begin(Ethernet.localIP(), "arduino", "password", InternalStorage);
 
  // === QTT INIT ===
  Serial.println("MQTT Init");
  mqttClient.setServer(mqttServer, MQTT_Port);
  Serial.println("OTA Ready");

  // === BUTTON INIT ===
  Serial.println("button init ");
  buttonInit();
   // === Bluetooth INIT ===
  Serial.println("Barcode begin");
  barcodeBegin();
   // RTOS init 
  Serial.println("RTOS INIT ");
   xTaskCreatePinnedToCore(
      MainLoop,   // Function to implement the task
      "MainLoop", // Name of the task
      10000,      // Stack size in words
      NULL,       // Task input parameter
      1,          // Priority of the task
      NULL,       // Task handle
      1);     
  // === Create Tasks ===
  xTaskCreatePinnedToCore(
      OTAUpdate,   // Function to implement the task
      "OTAUpdate", // Name of the task
      10000,       // Stack size in words
      NULL,        // Task input parameter
      1,           // Priority of the task
      NULL,        // Task handle
      1);          // Core where the task should run (0 or 1)
     // Core where the task should run (0 or 1)
  xTaskCreatePinnedToCore(
      RFIDRoutine,   // Function to implement the task
      "RFIDRoutine", // Name of the task
      10000,         // Stack size in words
      NULL,          // Task input parameter
      2,             // Priority of the task
      NULL,          // Task handle
      1);            // Core where the task should run (0 or 1)
   

}

void loop()
{
  
}
