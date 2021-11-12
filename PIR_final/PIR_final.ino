
#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <ESP8266WebServer.h>

/********* WiFi Access Point ***********/ 
#define WLAN_SSID       "RaspKevin" 
#define WLAN_PASS       "12345678" 
#define MQTT_SERVER      "10.42.0.246" // static ip address
#define MQTT_PORT         1883                    
#define MQTT_USERNAME    "" 
#define MQTT_PASSWORD         "" 
Adafruit_INA219 sensor219; // Declare and instance of INA219

//WiFiServer server(80);                    
//
//IPAddress ip(10, 42, 0, 156);            // IP address of the server
//IPAddress gateway(10,42,0,1);           // gateway of your network
//IPAddress subnet(255,255,255,0);          // subnet mask of your network
//

//IPAddress ip(10, 42, 0,16); //ESP static IP address
//IPAddress gateway(10,42,0,1); //IP Address of your WiFi Router (Gateway)
//IPAddress subnet(255, 255, 255, 0); //Subnet mask
//IPAddress DNS(10,42,0,1); //DNS
//const char* deviceName = "pir-motion-sensor";


// Create an ESP8266 WiFiClient class to connect to the MQTT server. 


WiFiClient client; 
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
/********** Feeds *************/ 
// Setup a feed called 'pi_PIR' for publishing. 
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Publish pi_PIR = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/PIR/pi"); 


//data collection channel
Adafruit_MQTT_Publish data_PIR1 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/PIR1"); 
Adafruit_MQTT_Publish data_PIR2 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/PIR2"); 
Adafruit_MQTT_Publish data_PIR3 = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/DATA/PIR3"); 

/********* Sketch Code ************/ 

int calibrationTime = 5;
long unsigned int lowIn; 
long unsigned int pause = 5000; 
boolean lockLow = true;
boolean takeLowTime; 
int led = D4;                // the pin that the LED is atteched to
int PIR = D5;              // the pin that the sensor is atteched to
int state = LOW;             // by default, no motion detected
int val = 0;                 // variable to store the sensor status (value)
unsigned long clientTimer = 0;

ESP8266WebServer server(80);

void setup() {
  // Setup INA219
  Serial.begin(9600); 
  sensor219.begin();
  sensor219.setCalibration_16V_400mA();
  pinMode(led, OUTPUT);      // initalize LED as an output
  pinMode(PIR, INPUT);    // initialize sensor as an input

//Serial.begin(115200);
  //WiFi.config(ip, gateway, subnet, DNS);
  delay(100);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Fail connecting");
    delay(5000);
    ESP.restart();
  }
  Serial.print("   OK  ");
  Serial.print("Module IP: ");
  Serial.println(WiFi.localIP());




//  // enabling connection with raspberry pi
// //Serial.println(F("RPi-ESP-MQTT")); 
// // Connect to WiFi access point. 
//  Serial.println(); Serial.println(); 
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

 //give the sensor time to calibrate
  Serial.println("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(calibrationTime - i);
    Serial.print("-");
    delay(1000);
  }
  Serial.println();
  Serial.println("done");
 
  //while making this Instructable, I had some issues with the PIR's output
  //going HIGH immediately after calibrating
  //this waits until the PIR's output is low before ending setup
  while (digitalRead(PIR) == HIGH) {
    delay(500);
    Serial.print(".");     
  }
  Serial.println("SENSOR ACTIVE");



}



void loop() {
  MQTT_connect(); 
  float busVoltage = 0;
  float current = 0; // Measure in milli amps
  float power = 0;
  float shuntvoltage = 0;

  shuntvoltage = sensor219.getShuntVoltage_mV();
  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA();
  power = sensor219.getPower_mW();

 data_PIR1.publish(power ); 
 data_PIR1.publish(power);
 data_PIR2.publish("Voltage;");
 data_PIR2.publish(busVoltage);
 data_PIR3.publish("current:");
 data_PIR3.publish(current);

val =digitalRead(PIR);
if (val == HIGH) {  
       Serial.println("PIR HIGH");               // check if the sensor is HIGH   
    digitalWrite(led, HIGH);  // turn led on
Serial.println("The led is on");
    
pi_PIR.publish("PIR Detect Motion");
    pi_PIR.publish("Open the Door? Awaiting for Response.");
    
   
    shuntvoltage = sensor219.getShuntVoltage_mV();
  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA();
  power = sensor219.getPower_mW();  
  
//  Serial.println (", ", power);
  }

 data_PIR1.publish("power:");
 data_PIR1.publish(power);
 data_PIR2.publish("Voltage;");
 data_PIR2.publish(busVoltage);
 data_PIR3.publish("current:");
 data_PIR3.publish(current); 




if (val == LOW) { 
 
      Serial.println("PIR low");
      digitalWrite(led, LOW);  
      Serial.println ("The led is off"); 
     }
 
 delay(1000); 

}








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
