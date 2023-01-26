#include <NTPClient.h>

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_MQTT_FONA.h>

#include <ESP8266WiFi.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>;
#include <WiFiUdp.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
const char *ssid =  "OnePlus 9R";     // Enter your WiFi Name
const char *pass =  "12345678"; // Enter your WiFi Password
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "M_yahya"
#define MQTT_PASS "aio_VlMs58Sab2WWMDr4RlVb4aRdeSTp"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800 ,60000);
Servo myservo;                          //servo as gate
Servo myservos;                               //servo as gate
int VehicleEnter = D0;                      // entry sensor 
int VehicleExited = D2;                   //exit sensor
int space3 = D7;                 
int space2 = D6;
int space1  = D3;                   
int count =0; 
int CLOSE_ANGLE = 90;  // The closing angle of the servo motor arm
int OPEN_ANGLE = 0;  // The opening angle of the servo motor arm              
int  hh, mm, ss;
int pos;
int pos1;
String h, m,EntryTimeSpace1,ExitTimeSpace1, EntryTimeSpace2,ExitTimeSpace2, EntryTimeSpace3,ExitTimeSpace3;
boolean entrysensor, exitsensor,s1,s2,s3;
boolean s1_occupied = false;
boolean s2_occupied = false;
boolean s3_occupied = false;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe EntryGate = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/EntryGate");
Adafruit_MQTT_Subscribe ExitGate = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/ExitGate");
//Set up the feed you're publishing to
Adafruit_MQTT_Publish VehiclesParked = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/VehiclesParked");
Adafruit_MQTT_Publish EntrySpace1 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/EntrySpace1");
Adafruit_MQTT_Publish ExitSpace1 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/ExitSpace1");
Adafruit_MQTT_Publish EntrySpace2 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/EntrySpace2");
Adafruit_MQTT_Publish ExitSpace2 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/ExitSpace2");
Adafruit_MQTT_Publish EntrySpace3 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/EntrySpace3");
Adafruit_MQTT_Publish ExitSpace3 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/ExitSpace3");
void setup() {
  delay(1000);
  Serial.begin (9600); 
  mqtt.subscribe(&EntryGate);
  mqtt.subscribe(&ExitGate);
  timeClient.begin(); 
  myservo.attach(D4);      // servo pin to D6
  myservos.attach(D5);        // servo pin to D5
  pinMode(VehicleExited, INPUT);    // ir as input
  pinMode(VehicleEnter, INPUT);     // ir as input
  pinMode(space1, INPUT);
  pinMode(space2, INPUT);
  pinMode(space3, INPUT);
  WiFi.begin(ssid, pass);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);                          // display ssid
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");                          // if not connected print this
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
}
void loop() {
 MQTT_connect();
 timeClient.update();
 hh = timeClient.getHours();
 mm = timeClient.getMinutes();
 ss = timeClient.getSeconds();
 h= String(hh); 
 m= String(mm);
 h +" :" + m;
 
 entrysensor= !digitalRead(VehicleEnter);
 exitsensor = !digitalRead(VehicleExited);
 s1 = digitalRead(space1);
 s2 = digitalRead(space2);
 s3 = digitalRead(space3);
 
   if ( count <= 2) {
	   if (entrysensor == 1) {                     // if high then count and send data
	   count=  count+1;                             //increment count
	   myservos.write(OPEN_ANGLE);
	   delay(3000);
	   myservos.write(CLOSE_ANGLE);
	   }
   }
   
   if ( count >= 1 ) {
     if (exitsensor == 1) {                            //if high then count and send
   count= count-1;                                    //decrement count
   myservo.write(OPEN_ANGLE);
   delay(3000);
   myservo.write(CLOSE_ANGLE);
  }
   }
   
   
  if (! VehiclesParked.publish(count)) {}
  if (s1 == 0 && s1_occupied == true) {                     
        Serial.println("Occupied1 ");
        EntryTimeSpace1 =  h +" :" + m;
        //Serial.print("EntryTimeSpace1");
        //Serial.print(EntryTimeSpace1);
        
        s1_occupied = false;
        if (! EntrySpace1.publish((char*) EntryTimeSpace1.c_str())){}
    }
  if(s1 == 1 && s1_occupied == false) {
       Serial.println("Available1 ");
       ExitTimeSpace1 =  h +" :" + m;
       //Serial.print("ExitTimeSpace1");
       //Serial.print(ExitTimeSpace1);
     
       s1_occupied = true;
        if (! ExitSpace1.publish((char*) ExitTimeSpace1.c_str())){} 
}
  if (s2 == 0 && s2_occupied == true) {                     
      Serial.println("Occupied2 ");
      EntryTimeSpace2 =  h +" :" + m;
      //Serial.print("EntryTimeSpace2");
      //Serial.print(EntryTimeSpace2);
     
      s2_occupied = false;
      if (! EntrySpace2.publish((char*) EntryTimeSpace2.c_str())){}
    }
  if(s2 == 1 && s2_occupied == false) {
       Serial.println("Available2 ");
       ExitTimeSpace2 =  h +" :" + m;
       //Serial.print("ExitTimeSpace2");
       //Serial.print(ExitTimeSpace2);
       
       s2_occupied = true;
        if (! ExitSpace2.publish((char*) ExitTimeSpace2.c_str())){}
  }
  if (s3 == 0 && s3_occupied == true) {                     
      Serial.println("Occupied3 ");
      EntryTimeSpace3 =  h +" :" + m;
     //Serial.print("EntryTimeSpace3: ");
      //Serial.print(EntryTimeSpace3);
      s3_occupied = false;
       if (! EntrySpace3.publish((char*) EntryTimeSpace3.c_str())){}
    }
  if(s3 == 1 && s3_occupied == false) {
       Serial.println("Available3 ");
       ExitTimeSpace3 =  h +" :" + m;
       //Serial.print("ExitTimeSpace3: ");
       //Serial.print(ExitTimeSpace3);
       s3_occupied = true;
        if (! ExitSpace3.publish((char*) ExitTimeSpace3.c_str())){ }
  } 
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
     {
    
   if (subscription == &EntryGate)
     {
      //Print the new value to the serial monitor
      Serial.println((char*) EntryGate.lastread);
     
   if (!strcmp((char*) EntryGate.lastread, "ON"))
      {
       myservos.write(OPEN_ANGLE);
       delay(3000);
       myservos.write(CLOSE_ANGLE);
    }
}
  if (subscription == &ExitGate)
     {
      //Print the new value to the serial monitor
      Serial.println((char*) EntryGate.lastread);
     
   if (!strcmp((char*) ExitGate.lastread, "ON"))
      {
       myservo.write(OPEN_ANGLE);
       delay(3000);
       myservo.write(CLOSE_ANGLE);
    }
}
}  
}
void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}   
 
