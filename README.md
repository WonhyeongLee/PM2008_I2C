# PM2008_I2C
ESP32-먼지센서실습

# 연결
PM2008 아두이노 쉴드 장착기준 (아두이노 uno 배열보고 VCC , GND 맞춰서 ESP32 와 연결하면 된다)

SCL -> P22

SDA -> P21

VCC -> 5V

GND -> GND
 
1단계 목표 0916 :  PM2008_I2C 라이브러리를 이용해서 데이터를 받고 Node-RED 에 데이터 전송 (JSON 형식)
