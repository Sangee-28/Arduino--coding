 /***************** 
  NodeMCU
******************/ 

  

#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 
#include <SPI.h>
#include <DHT.h>
#include<PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 sensor219; 

/********* WiFi Access Point ***********/ 
#define WLAN_SSID       "RaspKevin" 
#define WLAN_PASS       "12345678" 
#define MQTT_SERVER      "10.42.0.246" // static ip address
#define MQTT_PORT         1883                    
#define MQTT_USERNAME    "" 
#define MQTT_PASSWORD         "" 
#define DHT11_PIN D7
#define DHTTYPE DHT11



// Create an ESP8266 WiFiClient class to connect to the MQTT server. 
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
///********** Feeds *************/ 
// Setup a feed called 'pi_DHT' for publishing. 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Publish pi_DHT = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DHT/pi"); 
Adafruit_MQTT_Publish pi_SERVO = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/SERVO/pi");
Adafruit_MQTT_Publish pi_BUZZER = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/BUZZER/pi"); 

//data collection channel
Adafruit_MQTT_Publish data_DHT1 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/DHT1"); 
Adafruit_MQTT_Publish data_DHT2 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/DHT2"); 
Adafruit_MQTT_Publish data_DHT3 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/DHT3"); 

// Setup a feed called 'esp8266_DHT' for subscribing to changes. 
Adafruit_MQTT_Subscribe esp8266_DHT = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/DHT/esp8266"); 


unsigned long DHTtimer = 0;
float h, t;
DHT dht(DHT11_PIN,DHTTYPE);

int flag1 ;
int flag2 ;

void setup() { 
  pinMode(2, OUTPUT);
// Serial.begin(9600); 
// dht.begin();
//
//Serial.println("CLEARDATA"); 
//Serial.println("LABEL,Time,Power,...");
//Serial.println("RESETTIMER"); 

//// Setup INA219
//  sensor219.begin();
//  sensor219.setCalibration_16V_400mA();
//  Serial.println(".");
   

 
//// enabling connection with raspberry pi
// //Serial.println(F("RPi-ESP-MQTT")); 
// // Connect to WiFi access point. 
// Serial.println(); Serial.println(); 
// Serial.print("Connecting to "); 
// Serial.println(WLAN_SSID);  
// WiFi.begin(WLAN_SSID, WLAN_PASS); 
// while (WiFi.status() != WL_CONNECTED) { 
//   delay(500); 
//   Serial.print("."); 
// } 
// Serial.println(); 
// Serial.println("WiFi connected"); 
// Serial.println("IP address: "); 
// Serial.println(WiFi.localIP()); 
//
// // Setup MQTT subscription for esp8266_DHT feed
// mqtt.subscribe(&esp8266_DHT); 
//
////delay(2000);

}  




void loop() {  
  digitalWrite(2,HIGH);
  delay(1000);
  digitalWrite(2,LOW);
}

//MQTT_connect(); 
//
//  
//  float busVoltage = 0;
//  float current = 0; // Measure in milli amps
//  float power = 0;
//  float shuntvoltage = 0;
//
//  shuntvoltage = sensor219.getShuntVoltage_mV();
//  busVoltage = sensor219.getBusVoltage_V();
//  current = sensor219.getCurrent_mA();
//  power = sensor219.getPower_mW();
//
// data_DHT1.publish("power:");
// data_DHT1.publish(power);
// data_DHT2.publish("Voltage;");
// data_DHT2.publish(busVoltage);
// data_DHT3.publish("current:");
// data_DHT3.publish(current);
//
//
//  //Serial.print("DATA,TIME,");
// // Serial.print("Power:         "); 
// // Serial.print(power ); 
// // Serial.println();
// // Serial.println(" mA"); 
// 
//  
//  if (millis() > DHTtimer + 2000) {
////    h = dht.readHumidity();
////  shuntvoltage = sensor219.getShuntVoltage_mV();
////  busVoltage = sensor219.getBusVoltage_V();
////  current = sensor219.getCurrent_mA();
////  power = sensor219.getPower_mW();
////  data_DHT1.publish("power:");
//// data_DHT1.publish(power);
//// data_DHT2.publish("Voltage;");
//// data_DHT2.publish(busVoltage);
//// data_DHT3.publish("current:");
//// data_DHT3.publish(current);
// 
//    t = dht.readTemperature();                // reads the DHT for temperature
//   millis();
//   //Serial.println(millis());
//    
//    shuntvoltage = sensor219.getShuntVoltage_mV();
//  busVoltage = sensor219.getBusVoltage_V();
//  current = sensor219.getCurrent_mA();
//  power = sensor219.getPower_mW();
//  data_DHT1.publish("power:");
// data_DHT1.publish(power);
// data_DHT2.publish("Voltage;");
// data_DHT2.publish(busVoltage);
// data_DHT3.publish("current:");
// data_DHT3.publish(current);
//
//   if (isnan(h) || isnan(t)) {
//      Serial.print("HUMIDITY");
//      Serial.println(String(h));
//      Serial.println(String(h));
//      Serial.print("TEMP");
//      Serial.println(String(t));
//      return;
//    } else {
//      DHTtimer = millis();
//    } 
//  }
//
//Serial.println("Temperature is");
// Serial.println(t);
//   //Serial.println("Humidity is");
//   //Serial.println(h);
//
// 
// 
// 
//
//
//MQTT_connect(); 
//
// //  pi_DHT.publish("temperature");
//   pi_DHT.publish(t);
//   delay(2000); 
// 
//  // pi_DHT.publish("humidity");
//  // pi_DHT.publish(h);
// 
//  if((t>=18)&& (flag1==0)){
//    flag1 = 1;
//   // Serial.println("yes");
//    pi_SERVO.publish( "FANON");
//    pi_DHT.publish("The temperature is greater than 25 and the fan is on");
//   // Serial.println(flag1);
//  }
//   
//  
//  if((t>30)&& (flag2==0)){
//    pi_BUZZER.publish("BUZZERON");
//    pi_DHT.publish("the temperature is greater than 30 and the buzzer is on");
//     flag2=1;
//  }
// 
//   Adafruit_MQTT_Subscribe *subscription; 
// while ((subscription = mqtt.readSubscription())) { 
//   if (subscription == &esp8266_DHT) { 
//     char *message = (char *)esp8266_DHT.lastread; 
//     Serial.print(F("Got: ")); 
//     Serial.println(message); 
//        // Check if the message was ON, OFF, or TOGGLE. 
//     if (strncmp(message, "DHTOFF",2)==0){
//       // Turn Off the DHT sensor
//      digitalWrite(13, LOW); 
//      delay(10000);
//   
//     } 
// 
//}
//
// }
//
// int time ;
// time =millis();
// Serial.println( time);
//}
//
//
// 
//
//
//
//// Function to connect and reconnect as necessary to the MQTT server. 
//void MQTT_connect() { 
// int8_t ret; 
// // Stop if already connected. 
// if (mqtt.connected()) { 
//   return; 
// } 
// Serial.print("Connecting to MQTT... "); 
// uint8_t retries = 3; 
// while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected 
//      Serial.println(mqtt.connectErrorString(ret)); 
//      Serial.println("Retrying MQTT connection in 5 seconds..."); 
//      mqtt.disconnect(); 
//      delay(5000);  // wait 5 seconds 
//      retries--; 
//      if (retries == 0) { 
//        // basically die and wait for WDT to reset me 
//        while (1); 
//      } 
// } 
// Serial.println("MQTT Connected!"); 
//}
