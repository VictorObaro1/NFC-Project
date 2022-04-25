#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
//#include <SoftwareSerial.h>
const char* ssid = "Nokia G20";
const char* password = "mypasscode99";

const char* serverName = "http://offline-micropayment.herokuapp.com/api/reader";
String feedback, ID;
#define button 5
#define led 4
bool state;
WiFiClient client;
HTTPClient http;
StaticJsonDocument<200> doc;
//SoftwareSerial mcu(5, 6);

void setup() {
  // Open serial communications and wait for port to open:
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  //mcu.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void HttpGetRequest() {
  digitalWrite(led, HIGH);
  unsigned int reloadTime = 0;
  http.begin(client, serverName);
  int httpResponseCode = http.POST("");
  String payload = "{}";
  if (httpResponseCode > 0) {
    payload = http.getString();
    reloadTime = 10;
    digitalWrite(led, LOW);
  }
  http.end();
  reloadTime++;
  while (reloadTime == 10);
  deserializeJson(doc, payload);
  unsigned int Amount = doc["amount"];
  Serial.println(Amount);
  digitalWrite(led, LOW);
}

void loop() { // run over and over
  /*if (Serial.available()) {
    String Reader_wallet_balance = Serial.readString();
    Serial.print(Reader_wallet_balance);
    }*/
  if (digitalRead(button)) {
    HttpGetRequest();
  }
}
