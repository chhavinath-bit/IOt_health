#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SimpleTimer.h>
#include "ThingSpeak.h"

WiFiClient client;
const char* MY_SSID = "OPPO A15";
const char* MY_PWD = "chhavi3251#";
const char* TS_SERVER = "api.thingspeak.com";

// //const int channelID = 2036795;
// //String TS_API_KEY ="S51ONI7XKJ7RZIB5";

// // channel 2
const int channelID = 2105135;
String TS_API_KEY ="VX4DFSC4GMAV995D";
// // channel 3
// const int channelID = 2105729;
// String TS_API_KEY ="NHHH5KBBA28KFFW1";

const int readChannelID = 2107463;
const char* TS_READ_API_KEY = "42J9B556EAEAJ9D8";
const int fieldNum = 1;
const char* TS_WRITE_API_KEY = "LU6RWHX67C8NKYHO";

const int read_s2_ChannelID = 2105135;
const char* TS_s2_READ_API_KEY = "VX4DFSC4GMAV995D";
const int sfieldNum = 3;
const int read_s3_ChannelID = 2105729;
const char* TS_s_3READ_API_KEY = "NHHH5KBBA28KFFW1";

int sent = 0;
float sensorVoltage; float sensorValue; float sensorMV;
int indexUV=0;
float total, clEnergy, energy = 0;
 float maxEnergy = 500, min_energy, k = maxEnergy;
// int lampState = 0, start = 1;
SimpleTimer timer;
/*
WiFiClient client2, client3;
const int httpPort = 80;
const char* host = "maker.ifttt.com";
const char* apiKey = "bvZMIZ4hqr_S9APvvleOdD";
*/

void setup() 
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  delay(100);
  connectWifi();
  //connectMail();
  //pinMode(D3, OUTPUT); digitalWrite(D3, LOW);
}


/*
void connectMail(){
  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client2.connect(host, httpPort)) {
    Serial.println("Mail connection failed");
    return;
  }
}
*/
void loop() 
{
    timer.run(); // Initiates SimpleTimer

  for(int i=0; i<5; i++){
    sensorValue += analogRead(A0);
    delay(200);
  }

  sensorValue= sensorValue/5;
  sensorVoltage = (sensorValue/1023)*5;
  sensorMV = sensorVoltage*1000;
  
  Serial.print("sensor reading = ");  Serial.println(sensorValue);
  Serial.print("sensor voltage = ");  Serial.print(sensorVoltage);  Serial.println(" V");
  Serial.print("sensor voltage ="); Serial.print(sensorMV); Serial.println(" mV");

  indexUV = uvIndex(sensorMV);
  Serial.print("index = "); Serial.println(indexUV);

  clEnergy = energyCalculate(indexUV);
  Serial.print("Total Energy on unit area until now "); Serial.print(clEnergy); Serial.println(" mW-s/m^2");

  min_energy = read_channel(clEnergy);
  //relayControl(min_energy);
  if(min_energy>=maxEnergy){
    total = 0;
  }
  sendDataTS();
  delay(1000);
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

void sendDataTS(void)
{
    /*if (client.connect(TS_SERVER, 80)) {
    float t = sensorMV; float h = indexUV;  float g= clEnergy;
    // Construct API request body
    String body = "&field1=";   body += String(t);
           body += "&field2=";  body += String(h);
           body += "&field3=";  body += String(g);
          
    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + TS_API_KEY);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);    
    Serial.println(" Sending UV Values to Thingspeak.");
  }
  client.stop();
  Serial.println("Waiting...");*/
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
  if(sensor<50){ index = 0;
  } else if(sensor<227){ index = 1;
  } else if(sensor<318){ index = 2;
  } else if(sensor<408){ index = 3;
  } else if(sensor<503){ index = 4;
  } else if(sensor<606){ index = 5;
  } else if(sensor<696){ index = 6;
  } else if(sensor<795){ index = 7;
  } else if(sensor<881){ index = 8;
  } else if(sensor<976){ index = 9;
  } else if(sensor<1079){ index = 10;
  } else{ index = 11;
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

float read_channel(float totalEnergy){
  float energys_1= ThingSpeak.readFloatField(readChannelID, sfieldNum, TS_READ_API_KEY);
  int statusCode1 = ThingSpeak.getLastReadStatus();
  float energys_3 = ThingSpeak.readFloatField(read_s3_ChannelID, sfieldNum, TS_s_3READ_API_KEY);
  int statusCode3 = ThingSpeak.getLastReadStatus();
  Serial.print("Energy from Sensor 1: "); Serial.println(energys_1);
  Serial.print("Energy from Sensor 2: "); Serial.println(totalEnergy);
  Serial.print("StatusCode2:"); Serial.println(statusCode1);
  Serial.print("Energy from Sensor 3: "); Serial.println(energys_3);
  Serial.print("StatusCode3:"); Serial.println(statusCode3);
  return min(totalEnergy, min(energys_1,energys_3));
}
/*
void relayControl(float totalEnergy){
  long lampCmd = ThingSpeak.readLongField(readChannelID, fieldNum, TS_READ_API_KEY);
  int statusCode = ThingSpeak.getLastReadStatus();
  Serial.print("lampCmd value: ");  Serial.println(lampCmd);
  Serial.print("statusCode:");  Serial.println(statusCode);
  
  if(statusCode==200){
    if(lampCmd==1 &&start==1){    // System Startup
        digitalWrite(D3, HIGH);
        start = 0;
    } else if(lampCmd==0 && start==0){      // Google Inactivation
        digitalWrite(D3, LOW);
        start = 1;
        sendMail(0);
        clEnergy = 0;
        delay(5000);
    } else if(totalEnergy>=maxEnergy && start==0){    // Energy Inactivation
        digitalWrite(D3, LOW);
        start = 1;
        sendMail(1);
        clEnergy = 0;
        lampCmd = 0;    change_lamp_state(lampCmd);
        delay(5000);
    } else if(lampCmd==0 && start==1){
        Serial.println("Turn the Lamp on Using Google Assistant");
        delay(1000);
    } else {
        Serial.println("Waiting for Threshold Energy");
        delay(1000);+
    }
  } else{
    Serial.println("Unable to read channel / No internet connection");
    delay(1000);
  }
}

void sendMail(int ga){
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin("http://maker.ifttt.com/trigger/uv_limit/with/key/bvZMIZ4hqr_S9APvvleOdD/?value1="+
    String(min_energy)+"&value2="+String(maxEnergy)+"&value3=YES"); //HTTP

    Serial.print("[HTTP] GET...\n");    // start connection and send HTTP header
    int httpCode = http.GET();          // httpCode will be negative on error
    if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        delay(500); // wait for half sec before retry again
    }
    http.end();
    Serial.println("Sending Mail....");
}

void change_lamp_state(int lamp){
   if (client3.connect(TS_SERVER, 80)) {
    int t = lamp;
    // Construct API request body
    String body = "&field1=";   body += String(t);
          
    client3.println("POST /update HTTP/1.1");
    client3.println("Host: api.thingspeak.com");
    client3.println("User-Agent: ESP8266 (nothans)/1.0");
    client3.println("Connection: close");
    client3.println("X-THINGSPEAKAPIKEY: " + TS_WRITE_API_KEY);
    client3.println("Content-Type: application/x-www-form-urlencoded");
    client3.println("Content-Length: " + String(body.length()));
    client3.println("");
    client3.print(body);    
    Serial.println(" Sending Lamp Status Change to Thingspeak.");
  }
  client3.stop();
  Serial.println("Waiting...");
}
*/
