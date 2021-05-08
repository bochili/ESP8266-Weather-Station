#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#define BLINKER_MIOT_SENSOR

#include <Blinker.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


char auth[] = "d4930aecbb0c";//换成自己的key
char ssid[] = "卧室的Wifi";//换成自己的wifi
char pswd[] = "20040131lbc";//换成自己的wifi密码

BlinkerNumber HUMI("num-hum");//新建组件
BlinkerNumber TEMP("num-tem");
//OLED显示屏的SCL接D1，SDA接D2
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 14     // GPIO14代表D5引脚，DHT11上的S端接D5

// 使用的温湿度传感器型号:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
int humi_read = 0, temp_read = 0;
void heartbeat()
{
    HUMI.print(humi_read);
    TEMP.print(temp_read);
}
void dataStorage()
{
    Blinker.dataStorage("temp", temp_read);
    Blinker.dataStorage("humi", humi_read);
}
//小爱同学语音
void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.temp(temp_read);
            BlinkerMIOT.humi(humi_read);
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.temp(20);
            BlinkerMIOT.humi(20);
            break;
    }
}
//小爱同学结束
void setup() {
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  // BLINKER_DEBUG.debugAll();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Blinker.begin(auth, ssid, pswd);
  Blinker.attachHeartbeat(heartbeat);
  Blinker.attachDataStorage(dataStorage);
  BlinkerMIOT.attachQuery(miotQuery);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  delay(5000);
  Blinker.run();
  //read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
      humi_read = h;
      temp_read = t;
  }

    Blinker.delay(2000);
  // clear display
  //OLED显示设置
  display.clearDisplay();
  
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,17);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %"); 
  
  display.display(); 
}
