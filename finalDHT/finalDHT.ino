/***************** 
  NodeMCU
******************/ 
#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
/********* WiFi Access Point ***********/ 
#define WLAN_SSID       "RaspKevin" 
#define WLAN_PASS       "12345678" 
#define MQTT_SERVER      "10.42.0.187" // static ip address
#define MQTT_PORT         1883                    
#define MQTT_USERNAME    "" 
#define MQTT_PASSWORD         "" 
#define DHT11_PIN D7
#define DHTTYPE DHT11
Adafruit_INA219 sensor219; // Declare and instance of INA219



WiFiServer server(80);                    

IPAddress ip(10, 42, 0, 156);            // IP address of the server
IPAddress gateway(10,42,0,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

// Create an ESP8266 WiFiClient class to connect to the MQTT server. 


WiFiClient client; 
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
/********** Feeds *************/ 
// Setup a feed called 'pi_DHT' for publishing. 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Publish pi_DHT = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DHT/pi"); 
// Setup a feed called 'esp8266_fan' for subscribing to changes. 
Adafruit_MQTT_Subscribe esp8266_fan = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/fan/esp8266"); 
/********* Sketch Code ************/ 
void MQTT_connect(); 

unsigned long DHTtimer = 0;
float h, t;
DHT dht(DHT11_PIN,DHTTYPE);

int frequency=1000; //Specified in Hz
int buzzPin=5; 
int timeOn=1000; //specified in milliseconds
int timeOff=1000; //specified in millisecods

#include <Servo.h>
Servo servo;

void setup() { 
 Serial.begin(115200); 
//dht.begin();

servo.attach(2); //D4

    // Setup INA219
  sensor219.begin();
  sensor219.setCalibration_16V_400mA();
  Serial.println(".");
   //delay(2000);


WiFi.config(ip, gateway, subnet);       // forces to use the fix IP 
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
 Serial.println("IP address: "); Serial.println(WiFi.localIP()); 

 // Setup MQTT subscription for esp8266_fan feed
 mqtt.subscribe(&esp8266_fan); 
} 

 uint32_t x=0; 
void loop() {
  
    float busVoltage = 0;
  float current = 0; // Measure in milli amps
  float power = 0;
  float shuntvoltage = 0;

  shuntvoltage = sensor219.getShuntVoltage_mV();
  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA();
  power = sensor219.getPower_mW();

  Serial.print("Power:         "); 
  Serial.print(power ); 
  Serial.println(" mA"); 
 
       
 MQTT_connect(); 

  
if (millis() > DHTtimer + 2000) {
    h = dht.readHumidity();
    t = dht.readTemperature();                // reads the DHT for temperature
   if (isnan(h) || isnan(t)) {
      Serial.print("HUMIDITY");
      Serial.println(String(h));
      Serial.println(String(h));
      Serial.print("TEMP");
      Serial.println(String(t));
      return;
    } else {
      DHTtimer = millis();
    } 
  }

//Serial.println("Temperature is");
  //  Serial.println(t);
  //  Serial.println("Humidity is");
  //  Serial.println(h);

    if(t>=25){
      
            servo.write(0);
         delay(1000);
         servo.write(90);
         delay(1000);
         servo.write(180);
         delay(1000);
    }

  if(t>=30){
    
       analogWrite(buzzPin, 20);
       delay(1000);
       analogWrite(buzzPin, 0);
    }
  
  
  
 
  
 
 Adafruit_MQTT_Subscribe *subscription; 
 while ((subscription = mqtt.readSubscription())) { 
   if (subscription == &esp8266_fan) { 
     char *message = (char *)esp8266_fan.lastread; 
     Serial.print(F("Got: ")); 
     Serial.println(message); 
        // Check if the message was ON, OFF, or TOGGLE. 
     if (strncmp(message, "BUZZERON",2)==0){
       // Turn the buzzer on 
      analogWrite(buzzPin, 20);
   
     } 
      else if (strncmp(message, "FANON",2)==0){
         // turn the fan on
                  servo.write(0);
         delay(1000);
         servo.write(90);
         delay(1000);
         servo.write(180);
         delay(1000);
        
         
         
   } 
  else if (strncmp(message, "FANOFF",3)==0){
//     turn the fan off'
    servo.write(0);
    
   }
else if (strncmp(message, "BUZZEROFF",3)==0){
  //turn the buzzer off
  analogWrite(buzzPin, 0);
}
   
 } 
 }
  
   pi_DHT.publish("temperature");
   pi_DHT.publish(t);
  // Serial.println(t);
//  delay(10000);
   pi_DHT.publish("humidity");
   pi_DHT.publish(h);
  // Serial.println(h);




 WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); Serial.println(request);
      client.flush();
      client.println("Hi client! No, I am listening.\r"); // sends the answer to the client
    }
    client.stop();                // tarminates the connection with the client
  }
}



//


// Function to connect and reconnect as necessary to the MQTT server. 
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
      mqtt.disconnect(); 
      delay(5000);  // wait 5 seconds 
      retries--; 
      if (retries == 0) { 
        // basically die and wait for WDT to reset me 
        while (1); 
      } 
 } 
 Serial.println("MQTT Connected!"); 
}
