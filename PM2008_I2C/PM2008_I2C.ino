l#include <pm2008_i2c.h>
#include <WiFi.h>
#include <ArduinoJson.h> //Json 사용을 위한 라이브러리 
#include <HTTPClient.h>  //HTTP 전송을 위한 라이브러리
#include <PubSubClient.h> //MQTT 사용을 위한 라이브러리

PM2008_I2C pm2008_i2c;
const char* ssid = "KT_GiGA_2G_Wave2_1123";          // your network SSID (name)
const char* pass = "ke77ff4984";                     // your network password
const char* mqttServer = "172.30.1.53";
const int mqttPort = 1883;
//const char* mqttUser = "yourInstanceUsername";
//const char* mqttPassword = "yourInstancePassword";
HTTPClient http;  
WiFiClient espClient;
PubSubClient client(espClient);

//EspMQTTClient client(
//  "KT_GiGA_2G_Wave2_1123",      // your network SSID (name)
//  "ke77ff4984",                 // your network password
//  "172.30.1.53",  // MQTT Broker 서버 IP , 지금 경우에는 라즈베리파이 IP 입력
//  //"201510344",  userID  지금은 스킵
//  //"1234",       password 지금은 스킵
//  "ESP32_PM2008",     // 클라이언트 이름
//  1883              // MQTT 포트번호
//  );


void setup() {
  pm2008_i2c.begin();
  Serial.begin(115200);
  pm2008_i2c.command();
  delay(1000);
  setWifi();
  client.setServer(mqttServer, mqttPort);

}

void loop() { 
  
  uint8_t ret = pm2008_i2c.read();
   if (!client.connected()) {
    reconnect();
  }
  if (ret == 0) {
   int pm1,pm2,pm10;
   pm1 = pm2008_i2c.pm1p0_grimm;
   pm2 = pm2008_i2c.pm2p5_grimm;
   pm10 = pm2008_i2c.pm10_grimm;
    
    Serial.print("PM 1.0 (GRIMM) : ");
    Serial.println(pm1);
    Serial.print("PM 2.5 (GRIMM) : : ");
    Serial.println(pm2);
    Serial.print("PM 10 (GRIMM) : : ");
    Serial.println(pm10);

     StaticJsonDocument<100> doc;  
     JsonObject root = doc.to<JsonObject>();
     root["Type"] = "PM2008-GRIMM";
     JsonObject value = root.createNestedObject("value");
     value["PM1"] = pm1;
     value["PM2"] = pm2;
     value["PM10"] = pm10;

     String requestBody;
     serializeJson(root, requestBody);
     Serial.println(requestBody); 
     client.publish("mytopic/test", requestBody.c_str()); //MQTT Publish
     client.loop();
     //postDataToServer(requestBody); 

    
//    Serial.print("PM 1.0 (TSI) : ");
//    Serial.println(pm2008_i2c.pm1p0_tsi);
//    Serial.print("PM 2.5 (TSI) : : ");
//    Serial.println(pm2008_i2c.pm2p5_tsi);
//    Serial.print("PM 10 (TSI) : : ");
//    Serial.println(pm2008_i2c.pm10_tsi);
//    Serial.print("Number of 0.3 um : ");
//    Serial.println(pm2008_i2c.number_of_0p3_um);
//    Serial.print("Number of 0.5 um : ");
//    Serial.println(pm2008_i2c.number_of_0p5_um);
//    Serial.print("Number of 1 um : ");
//    Serial.println(pm2008_i2c.number_of_1_um);
//    Serial.print("Number of 2.5 um : ");
//    Serial.println(pm2008_i2c.number_of_2p5_um);
//    Serial.print("Number of 5 um : ");
//    Serial.println(pm2008_i2c.number_of_5_um);
//    Serial.print("Number of 10 um : ");
//    Serial.println(pm2008_i2c.number_of_10_um);
  }
  delay(1000);
}
void setWifi(){
    Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  Serial.print(F("WiFi connected! IP address: "));
  Serial.println(WiFi.localIP());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
void postDataToServer(String requestBody){
          Serial.println("Posting JSON data to Node-RED...");
          http.begin("http://172.30.1.53:1880/data");  
          http.addHeader("Content-Type", "application/json; charset = utf-8");
          int httpResponseCode = http.POST(requestBody);  //POST형식으로 
         if(httpResponseCode>0){ 
                String response = http.getString();                    
                Serial.println(response);
        }else{ 
                Serial.printf("Error");     
         }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
