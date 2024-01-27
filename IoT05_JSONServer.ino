#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "4157";
const char* password = "12356789";
const char* serverAddress = "http://192.168.0.198:3000/sensor";

WiFiClient client;
HTTPClient http;
DHT dht(D4, DHT11);

void connectToWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

void sendDataToServer(float humidity, float temperature) {
  DynamicJsonDocument jsonDocument(200);
  jsonDocument["hum"] = humidity;
  jsonDocument["tem"] = temperature;
  jsonDocument["timestamp"] = millis();

  String jsonData;
  serializeJson(jsonDocument, jsonData);

  http.begin(client, serverAddress);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);

  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.println("HTTP Response code: " + String(httpResponseCode));
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  dht.begin();
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long timerDelay = 10000;

  if ((millis() - lastTime) > timerDelay) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor");
    } else {
      Serial.print("Humidity: ");
      Serial.println(humidity);
      Serial.print("Temperature: ");
      Serial.println(temperature);

      sendDataToServer(humidity, temperature);
    }
    lastTime = millis();
  }
}