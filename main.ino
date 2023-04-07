#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include "ThingSpeak.h"
WiFiClient client;
const int channelID = 2036795;
String TS_API_KEY ="KQL38FXAIHKOUJFU";
const char* MY_SSID = "OPPO A15";
const char* MY_PWD = "chhavi3251#";
const char* TS_SERVER = "api.thingspeak.com";

int sent = 0;

float sensorVoltage; 
float sensorValue;
float sensorMV;
int indexUV=0;
SimpleTimer timer;
void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
    delay(10);
  connectWifi();
  
  
}
 
 
 
void loop() 
{
    timer.run(); // Initiates SimpleTimer

 for(int i=0; i<5; i++){
  sensorValue += analogRead(A0);
  delay(200);}
  sensorValue= sensorValue/5;
  sensorVoltage = (sensorValue/1023)*5;
  sensorMV = sensorVoltage*1000;
  Serial.print("sensor reading = ");
  Serial.println(sensorValue);
  //Serial.println("");
  Serial.print("sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  Serial.print("sensor voltage in mv =");
  Serial.print(sensorMV);
  Serial.println(" mV");
 ;
  if(sensorMV<50){
    indexUV = 0;
  } else if(sensorMV<227){
    indexUV = 1;
  } else if(sensorMV<318){
    indexUV = 2;
  } else if(sensorMV<408){
    indexUV = 3;
  } else if(sensorMV<503){
    indexUV = 4;
  } else if(sensorMV<606){
    indexUV = 5;
  } else if(sensorMV<696){
    indexUV = 6;
  } else if(sensorMV<795){
    indexUV = 7;
  } else if(sensorMV<881){
    indexUV = 8;
  } else if(sensorMV<976){
    indexUV = 9;
  } else if(sensorMV<1079){
    indexUV = 10;
  } else{
    indexUV = 11;
  }
  Serial.print("index = ");
  Serial.println(indexUV);
  delay(1000);
  sendDataTS();
}
void connectWifi()
{
  Serial.print("Connecting to "+ *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("");  
}
void sendDataTS(void)
{
     
    if (client.connect(TS_SERVER, 80)) {
    
    // Measure Analog Input (A0)
   
    float t = sensorMV;
    float h = indexUV;
    // Construct API request body
    String body = "&field1=";
           body += String(t);
           body += "&field2=";
           body += String(h);
          

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + TS_API_KEY);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);

    Serial.print("sensorMV: ");
    Serial.print(t);
    delay(1000);
    Serial.print("indexUV: ");
    Serial.print(h);
    
    Serial.println(" Send to Thingspeak.");
  }
  client.stop();
  Serial.println("Waiting...");
 
}
