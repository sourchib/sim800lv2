#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
SoftwareSerial SerialAT(4,3); // RX, TX

//Milis
unsigned long now = millis();
unsigned long lastMeasure = 0;

//Network details
const char apn[]  = "m2mautotronic"; //sesuain apn sim card.
const char user[] = "";
const char pass[] = "";

// MQTT details
const char* broker = "108.137.155.108";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

void setup()
{
  Serial.begin(9600);
  SerialAT.begin(9600);
  Serial.println("System start.");
  modem.restart();
  Serial.println("Modem: " + modem.getModemInfo());
  Serial.println("Searching for telco provider.");
  if(!modem.waitForNetwork())
  {
    Serial.println("fail");
    while(true);
  }
  Serial.println("Connected to telco.");
  Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

  Serial.println("Connecting to GPRS network.");
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println("fail");
    while(true);
  }
  Serial.println("Connected to GPRS: " + String(apn));
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
  Serial.println("Connecting to MQTT Broker: " + String(broker));
  while(mqttConnect()==false) continue;
  Serial.println();
}

void loop()
{
  
  int counter = 0;
  counter++;
  String data = String(counter); //write your data here      
    
  if (!mqtt.connected())
  {
    mqttConnect();
  }
  mqtt.loop();
  now = millis();
  // Publishes pesan 5 detik
  if (now - lastMeasure > 10000)
  {
    lastMeasure = now;
    //testing data
    Serial.println(data);
    // data 1
    mqtt.publish("f1pm0/out/1", String(data).c_str());
   }
}

boolean mqttConnect()
{
  if(!mqtt.connect("SendData", "muchib", "muchib")) //id, username, password
  {
    Serial.print(".");
    return false;
  }
  Serial.println("Connected to broker.");
  return mqtt.connected();
}

void mqttCallback(char* topic, byte* payload, unsigned int len)
{
  Serial.print("Message receive: ");
  Serial.write(payload, len);
  Serial.println();
}
