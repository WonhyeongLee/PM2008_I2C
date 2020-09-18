#include <pm2008_i2c.h>
#include <WiFi.h>
#include <ArduinoJson.h> //Json 사용을 위한 라이브러리 
#include <HTTPClient.h>  //HTTP 전송을 위한 라이브러리

PM2008_I2C pm2008_i2c;

char ssid[] = "KT_GiGA_2G_Wave2_1123";          // your network SSID (name)
char pass[] = "ke77ff4984";                     // your network password

HTTPClient http;  

void setup() {
  pm2008_i2c.begin();
  Serial.begin(115200);
  pm2008_i2c.command();
  delay(1000);

    /////////////////////////////////wifi 연결부분/////////////////////////////////////
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
  ///////////////////////////////////////////////////////////////////////////////////
}

void loop() {
  uint8_t ret = pm2008_i2c.read();
  if (ret == 0) {
    Serial.print("PM 1.0 (GRIMM) : ");
    Serial.println(pm2008_i2c.pm1p0_grimm);
    Serial.print("PM 2.5 (GRIMM) : : ");
    Serial.println(pm2008_i2c.pm2p5_grimm);
    Serial.print("PM 10 (GRIMM) : : ");
    Serial.println(pm2008_i2c.pm10_grimm);

     StaticJsonDocument<100> doc;  
     JsonObject root = doc.to<JsonObject>();
     root["Type"] = "PM2008-GRIMM";
     JsonObject value = root.createNestedObject("value");
     value["PM1"] = pm2008_i2c.pm1p0_grimm;
     value["PM2"] = pm2008_i2c.pm2p5_grimm;
     value["PM10"] = pm2008_i2c.pm10_grimm;
     
     String requestBody;
     serializeJson(root, requestBody);
     Serial.println(requestBody); 
     postDataToServer(requestBody);

    
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
