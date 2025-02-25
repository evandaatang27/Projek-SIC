#include "UbidotsEsp32Mqtt.h"
#include "DHT.h"

#define DHTPIN 4       
#define DHTTYPE DHT11  
#define PIRPIN 5       

const char *UBIDOTS_TOKEN = "BBUS-tvVEIiPg9Nm9iABa1wbncD52GbNCLf";
const char *WIFI_SSID = "Onoirtas";
const char *WIFI_PASS = "qwerty543211";
const char *DEVICE_LABEL = "esp32";
const char *VARIABLE_LABEL = "temperature";
const char *VARIABLE_LABEL2 = "motion";

const int PUBLISH_FREQUENCY = 5000;
unsigned long timer;

DHT dht(DHTPIN, DHTTYPE);
Ubidots ubidots(UBIDOTS_TOKEN);

void setup() {
  Serial.begin(115200);
  dht.begin(); 
  pinMode(PIRPIN, INPUT); 

  ubidots.setDebug(true);  
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }

  if ((millis() - timer) > PUBLISH_FREQUENCY) {
    float temperature = dht.readTemperature();  
    int motion = digitalRead(PIRPIN);          

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Motion: ");
    Serial.println(motion);

    
    if (!isnan(temperature)) {
      ubidots.add(VARIABLE_LABEL, temperature);
    } else {
      Serial.println("Failed to read temperature");
    }

    ubidots.add(VARIABLE_LABEL2, motion);
    ubidots.publish(DEVICE_LABEL);

    timer = millis();
  }

  ubidots.loop();
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
