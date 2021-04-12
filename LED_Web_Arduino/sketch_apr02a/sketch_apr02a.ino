/*
 * Web控制LED灯带
 * by flame
 * 2021-4-21
 */

#include "FastLED.h"            // 此示例程序需要使用FastLED库
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

//---灯带宏定义---
#define NUM_LEDS 44            // LED灯珠数量
#define LED_DT 2                // Arduino输出控制信号引脚
#define LED_TYPE WS2812B         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
//---请求提示---
const char* help_str = "this page to control LEDS\r\n\npin?light=value value must be in 0-255\r\n";
const char* error_values = "Please input right values!";
//---LED灯带控制相关变量---
uint8_t bright = 100;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
uint8_t light_switch = 1;
uint8_t light_mode = 1;   //彩虹控制标志
//---z智能配网相关变量---
String smart_data = "";
int size_data = 0; //WIFI数据长度
String wifiname;
String wifipsw;
bool flag; //读取EEPROM联网，是否成功标志位
int wifiname_len; // wifi名的长度
//---RGB颜色值 (0-255) ---
int r = 255;
int g = 255;
int b = 255;

//---使用静态IP 方便记忆与使用---
IPAddress staticIP(192,168,0,192);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,0,1);

ESP8266WebServer server(80);    //webServer

CRGB leds[NUM_LEDS];            // 建立光带leds

void Config_wifi(const char *wifiname, const char *psw) //函数用于直接读取eeprom中的数据，直接完成后直接联网
{
  int i = 0;
  WiFi.begin(wifiname, psw);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (i == 0)
    {
      Serial.print("正在配置网络");
    }
    Serial.print(".");
    i++;
    if (i == 15)
    {
      Serial.println("连接失败请使用APP一键配网");
      flag = true;
      break; 
    }
  }
  if (WiFi.isConnected())
  {

    Serial.println("网络连接成功");
    Serial.print("Local IP:");
    Serial.println(WiFi.localIP());
  }
}
void read_eeprom()
{
  wifiname = "";
  wifipsw = "";
  // EEPROM.begin(20); //申请操作到地址4095（比如你只需要读写地址为100上的一个字节，该处也需输入参数101）
  int a = int(EEPROM.read(0)) - int('0'); //wifi名的长度
  int b = (int(EEPROM.read(1)) - int('0')) * 10 + (int(EEPROM.read(2)) - int('0')) ; //读数据;

  Serial.println(a);
  Serial.println(b);
  for (int addr = 3; addr < b + 1; addr++)
  {
    int data = EEPROM.read(addr); //读数据
    if (addr < a + 3)
    {
      wifiname = wifiname + char(data);
    }
    else
    {
      wifipsw = wifipsw + char(data);
    }
    delay(2);
  }
  Serial.print("SSID:");
  Serial.println(wifiname);
  Serial.print("PASSWORD:");
  Serial.println(wifipsw);


}
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  delay(2000);
  // 等待配网
  WiFi.beginSmartConfig();

  while (1)
  {
    Serial.print(".");
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  // 设置自动连接


      wifiname_len = String(WiFi.SSID().c_str()).length(); //获取wifi名的长度
      //  EEPROM.write(0, char(wifiname_len)); //写在地址0的数据为存入的wifi名的长度


      // EEPROM.write(1, char(size_data)); //写在地址1的数据为存入的数据长度

      smart_data = String(wifiname_len) + String(size_data) + String (WiFi.SSID().c_str()) + String(WiFi.psk().c_str());
      size_data = String(smart_data).length();
      smart_data = String(wifiname_len) + String(size_data) + String (WiFi.SSID().c_str()) + String(WiFi.psk().c_str());

      Serial.println(size_data);
      Serial.println(smart_data); //打印出保存在eeprom数据

      Serial.println(String(smart_data).length());//打印出保存在eeprom中的数据长度

      for (int addr = 0; addr < size_data + 1; addr++)
      {
        //int data = addr%256; //在该代码中等同于int data = addr;因为下面write方法是以字节为存储单位的.
        //String(smart_data).charAt(addr);
        EEPROM.write(addr, toascii(String(smart_data).charAt(addr))); //写数据
      }
      EEPROM.commit(); //保存更改的数据
      // EEPROM.end(); //同EEPROM.commit();

      Serial.println("End write");
      break;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//参数为 / 时要执行的方法
void handleRoot() {
  server.send(200, "text/plain", help_str);
}
//未知参数执行的方法
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
//灯带开启关闭控制
void handleSwitch()
{
  String requests = server.arg("light");
  Serial.println("handleSwitch " + requests);
  if(requests == "on"){
    light_switch = 1;
    server.send(200, "text/plain", "Light is on");
  }
  else if(requests == "off"){
    light_switch = 0;
    server.send(200, "text/plain", "Light is off");
  }
  else{
    server.send(200, "text/plain", error_values);
  }
}
//灯带颜色控制
void handleColor()
{
  Serial.println("handleColor ");
  if(light_mode == 1)
  {
    server.send(200, "text/plain", "AutoMode Now! please shutdown autoMode first!");
  }
  else
  {
    r = server.arg("R").toInt();
    g = server.arg("G").toInt();
    b = server.arg("B").toInt();
    server.send(200, "text/plain", "try...");
  }
}

//灯带模式控制
void handleMode()
{
  String requests = server.arg("mode");
  Serial.println("handleMode " + requests);
  if(requests == "auto"){
    light_mode = 1;
    server.send(200, "text/plain", "AutoMode");
  }
  else if(requests == "noauto"){
    light_mode = 0;
    server.send(200, "text/plain", "NonAuto");
  }
  else{
    server.send(200, "text/plain", error_values);
  }
}
void handleBright()
{
  Serial.println("handleBright ");
  bright = r = server.arg("bright").toInt();
  server.send(200, "text/plain", "try...");
}
// 灯带彩虹模式
void LedFunc1()
{
  static uint8_t beginHue;
  beginHue++;   
  fill_rainbow(leds, NUM_LEDS, beginHue, 9); 
  FastLED.show();
  delay(10); 
}

void setup() { 
  Serial.begin(115200);
  EEPROM.begin(50); //申请操作到地址4095（比如你只需要读写地址为100上的一个字节，该处也需输入参数101）
  flag = false;
  read_eeprom();
  Config_wifi(wifiname.c_str(), wifipsw.c_str());
  if (flag)
  {
    smartConfig();
  }

  //这里可以设置网址后边跟着的参数。
  server.on("/", handleRoot);
  server.on("/switch", handleSwitch);
  server.on("/color", handleColor);
  server.on("/mode", handleMode);
  server.on("/bright", handleBright);

  server.on("/inline", []() { //匿名方法
    server.send(200, "text/plain", "this works as well");
  });
  //未知参数
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // 初始化光带
  FastLED.setBrightness(bright);                            // 设置光带亮度
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  MDNS.update();
  FastLED.setBrightness(bright); 
  if(light_switch == 0)
  {
     for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
      FastLED.show();
      //delay(30);
    }
  }
  if(light_switch == 1 && light_mode == 1){
    LedFunc1();
  }
  if(light_switch == 1 && light_mode == 0){
    for (int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(r, g, b);
      FastLED.show();
    }
  }
}
