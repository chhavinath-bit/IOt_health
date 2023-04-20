//#include <WiFi.h>
#include <ESP8266WiFi.h>

const char* ssid = "OPPO A15";
const char* password = "chhavi3251#";
const char* host = "maker.ifttt.com";
const char* apiKey = "bvZMIZ4hqr_S9APvvleOdD";

const int limit = 2;
void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Set LED state to match door state
  pinMode(D2, OUTPUT);
  digitalWrite(D2,HIGH);
  
  // Connect to Wi-Fi
  Serial.print("begin");
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
}

int i = 1;
void loop() {
    while(i<=limit) {
      i++;
      //Send email
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      String url = "/trigger/uv_limit/with/key/";
      url += apiKey;

      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     //"Content-Type: application/x-www-form-urlencoded\r\n" + "Content-Length: 13\r\n\r\n" + "value1=" + i + "\r\n"*/
                            "Connection: close\r\n\r\n");
      delay(500);
    }  
}
