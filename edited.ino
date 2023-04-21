#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include "ThingSpeak.h"

WiFiClient client;
// IFTTT mail integration
WiFiClient client2;
const int httpPort = 80;
const char* host = "maker.ifttt.com";
const char* apiKey = "bvZMIZ4hqr_S9APvvleOdD";
String url = "/trigger/uv_limit/with/key/";
url += apiKey;
// // channel 1
const int channelID = 2036795;
String TS_API_KEY ="S51ONI7XKJ7RZIB5";
// // channel 2
// // const int channelID = 2105135;
// // String TS_API_KEY ="VX4DFSC4GMAV995D";
// // channel 3
// const int channelID = 2105729;
// String TS_API_KEY ="NHHH5KBBA28KFFW1";

const int readChannelID = 2107463;
const char* TS_READ_API_KEY = "42J9B556EAEAJ9D8";
const int fieldNum = 1;

const char* MY_SSID = "OPPO A15";
const char* MY_PWD = "chhavi3251#";
const char* TS_SERVER = "api.thingspeak.com";

int sent = 0;

float sensorVoltage; 
float sensorValue;
float sensorMV;
int indexUV=0;
float total, clEnergy, energy = 0;
int lampState = 0;
SimpleTimer timer;

void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
    delay(10);
  connectWifi();
  connectMail();
  pinMode(D3, OUTPUT);
    digitalWrite(D3, HIGH);
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
  Serial.print("sensor reading = ");  Serial.println(sensorValue);
  Serial.print("sensor voltage = ");  Serial.print(sensorVoltage);  Serial.println(" V");
  Serial.print("sensor voltage in mv ="); Serial.print(sensorMV); Serial.println(" mV");

  indexUV = uvIndex(sensorMV);
  Serial.print("index = "); Serial.println(indexUV);

  clEnergy = energyCalculate(indexUV);
  Serial.print("Total Energy on unit area until now "); Serial.print(clEnergy); Serial.println(" mW-s/m^2");

  relayControl(clEnergy);
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
  ThingSpeak.begin(client) ;
}

void connectMail(){
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client2.connect(host, httpPort)) {
    Serial.println("Mail connection failed");
    return;
  }
}

void sendDataTS(void)
{
     
    if (client.connect(TS_SERVER, 80)) {
    
    // Measure Analog Input (A0)
   
    float t = sensorMV;
    float h = indexUV;
    float g= clEnergy;
    // Construct API request body
    String body = "&field1=";
           body += String(t);
           body += "&field2=";
           body += String(h);
           body += "&field3=";
           body += String(g);
          

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
    Serial.print("total energy: ");
    Serial.print(g);
    
    
    Serial.println(" Send to Thingspeak.");
  }
  client.stop();
  Serial.println("Waiting...");
 
}

int uvIndex(int sensor){
  int index;
  if(sensor<50){
    index = 0;
  } else if(sensor<227){
    index = 1;
  } else if(sensor<318){
    index = 2;
  } else if(sensor<408){
    index = 3;
  } else if(sensor<503){
    index = 4;
  } else if(sensor<606){
    index = 5;
  } else if(sensor<696){
    index = 6;
  } else if(sensor<795){
    index = 7;
  } else if(sensor<881){
    index = 8;
  } else if(sensor<976){
    index = 9;
  } else if(sensor<1079){
    index = 10;
  } else{
    index = 11;
  }
  return index;
}

float energyCalculate(int index){
  int time_rect = 1;  // in seconds
  float uvIntensity = (index*1000)/40;
  float energy  = (uvIntensity*time_rect);
  total += energy;
  return total;
}

void relayControl(float totalEnergy){
  long lampCmd = ThingSpeak.readLongField(readChannelID, fieldNum, TS_READ_API_KEY);
  int statusCode = ThingSpeak.getLastReadStatus();
  Serial.println("lampCmd value: ");
  Serial.println(lampCmd);
  Serial.println("statusCode:");
  Serial.println(statusCode);
  
  int maxEnergy = 500;
  if(totalEnergy>=maxEnergy || (statusCode==200 && lampCmd==0)){
    digitalWrite(D3, LOW);
    String yn;
    if(statusCode==200){
      if(lampCmd==0){yn = "Yes";}
      else{yn = "No";}
    } else{yn = "Connection Error";}
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     //"Content-Type: application/x-www-form-urlencoded\r\n" + "Content-Length: 13\r\n\r\n" +
                      "value1=" + maxEnergy + "\r\n" +  "value2=" + totalEnergy + "\r\n" + "value3=" + yn + "\r\n" + 
                      "Connection: close\r\n\r\n");
  } else if(statusCode!=200){
    Serial.println("Unable to read channel / No internet connection");
  } else{
    Serial.println("Waiting for threshold energy");
  }

  // Automatic Control of Lamp
  
  /*if(totalEnergy>=maxEnergy){
    digitalWrite(D3, LOW);
    // Code for sending mail to user that we have turned off the lamp.
    Serial.print("Requesting URL: ");
    Serial.println(url);
  } 
  // Manual Control of UV Lamp
  else if (statusCode == 200)
  {
    if(lampCmd==0){
      digitalWrite(D3, LOW);
    }
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }*/
  delay(1000);
}