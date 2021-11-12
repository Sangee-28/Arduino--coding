 /*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Sends and receives message from the server in every 2 seconds
 *  Communicates: wifi_server_01.ino
 */ 
#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 
#include <SPI.h>
#include <Servo.h> 
#include <PubSubClient.h>
Servo servo;
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





WiFiClient client;



// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
///********** Feeds *************/ 
//// Setup a feed called 'SERVO_pi' for publishing. 
//// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Publish pub_SERVO = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/SERVO/pub"); 
//// Setup a feed called 'pi_SERVO' for subscribing to changes. 
Adafruit_MQTT_Subscribe pi_SERVO = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/SERVO/pi"); 
///********* Sketch Code ************/ 

//data pulishing channel
Adafruit_MQTT_Publish data_SERVO1 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/servo1"); 
Adafruit_MQTT_Publish data_SERVO2 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/servo2"); 
Adafruit_MQTT_Publish data_SERVO3 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/servo3"); 




void setup() {
  
  Serial.begin(9600); 
  
  //servo.attach(2); //D4

// Setup INA219
pinMode(2,OUTPUT);
  sensor219.begin();
  sensor219.setCalibration_16V_400mA();
  Serial.println(".");
   


 
  // enabling connection with raspberry pi
 Serial.println(F("RPi-ESP-MQTT")); 
 // Connect to WiFi access point. 
 Serial.println(); Serial.println(); 
 Serial.print("Connecting to "); 
 Serial.println(WLAN_SSID); 
 WiFi.begin(WLAN_SSID, WLAN_PASS); 
 while (WiFi.status() != WL_CONNECTED) { 
   delay(500); 
   Serial.print("."); 
 } 
 Serial.println(); 
 Serial.println("WiFi connected"); 
 Serial.println("IP address: "); 
 Serial.println(WiFi.localIP()); 

// Setup MQTT subscription for esp8266_SERVO feed
 mqtt.subscribe(&pi_SERVO); 

}

void loop () {

 

 //Serial.println("IP address: ");
 MQTT_connect();
float busVoltage = 0;
  float current = 0; // Measure in milli amps
  float power = 0;
  float shuntvoltage = 0;

  shuntvoltage = sensor219.getShuntVoltage_mV();
  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA();
  power = sensor219.getPower_mW();

  //Serial.print("DATA,TIME,");
 // Serial.print("Power:         "); 
  //Serial.println(power ); 
 // Serial.println();
 // Serial.println(" mA"); 
 data_SERVO1.publish("power:");
 data_SERVO1.publish(power);
 data_SERVO2.publish("Voltage;");
 data_SERVO2.publish(busVoltage);
 data_SERVO3.publish("current:");
 data_SERVO3.publish(current);
  



Adafruit_MQTT_Subscribe *subscription; 
 while ((subscription = mqtt.readSubscription())) { 
   if (subscription == &pi_SERVO) { 
     char *message = (char *)pi_SERVO.lastread; 
    Serial.print(F("Got: ")); 
     Serial.println(message); 
        // Check if the message was ON, OFF
     if (strncmp(message, "FANON",5)==0){
      digitalWrite(2,HIGH);
            //turn the fan on
           
     }
                                   //to make it go slower, increase the number.
else if (strncmp(message, "FANOFF",6)==0){
  //digitalWrite(2,LOW);
  ESP.reset();
  delay(1000000);
   }
  

 
   


   }
 
}

delay(1000);
     
}

//Function to connect and reconnect as necessary to the MQTT server. 
void MQTT_connect() { 
 int8_t ret; 
 // Stop if already connected. 
 if (mqtt.connected()) { 
   return; 
 } 
 Serial.print("Connecting to MQTT... "); 
 uint8_t retries = 3; 
 while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected 
      Serial.println(mqtt.connectErrorString(ret)); 
      Serial.println("Retrying MQTT connection in 5 seconds..."); 
     mqtt.disconnect();  n
      delay(5000);  // wait 5 seconds 
      retries--; 
      if (retries == 0) { 
        // basically die and wait for WDT to reset me 
        while (1); 
      } 
 } 
 Serial.println("MQTT Connected!"); 
}
