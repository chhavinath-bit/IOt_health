#include <WiFi.h>

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
const char* host = "maker.ifttt.com";
const char* apiKey = "bvZMIZ4hqr_S9APvvleOdD";

const int limit = 10;
void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Set LED state to match door state
  pinMode(D2, OUTPUT);
  digitalWrite(D2,HIGH);
  
  // Connect to Wi-Fi
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

    if(i>=limit) {
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

      String url = "/trigger/Lamp_Limit/with/key/";
      url += apiKey;

      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Content-Type: application/x-www-form-urlencoded\r\n" + 
                     "Content-Length: 13\r\n\r\n" +
                     "value1=" + i + "\r\n");
    }  
}
