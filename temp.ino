
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
char* state ;

RTC_PCF8523 RTC; 

const int chipSelect = 10;

File logfile;

 
#define SYNC_INTERVAL 1000
#define LOG_INTERVAL  1000
#define LED 10
#define DHTPIN1 2     
#define DHTPIN2 3
#define DHTTYPE DHT22
#define ECHO_TO_SERIAL   1
uint32_t syncTime = 0; 
DHT dht1(DHTPIN1, DHTTYPE); 
DHT dht2(DHTPIN2, DHTTYPE); 
float Inside_temp;
float Outside_temp;

void error(char const *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while (1);
}

void initSDcard()
{
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

}

void createFile()
{
  char filename[] = "MLOG00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;  
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
}

void initRTC()
{
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  
  }
}


void setup()
{
    Serial.begin(9600);
    dht1.begin();
    dht2.begin();
    initSDcard();
    createFile();
    initRTC();
    logfile.println("millis,stamp,datetime,Inside_Temp,Outside_Temp,status");
    Serial.println("millis,stamp,datetime,Inside_Temp,Outside_Temp,status");
    pinMode(10,OUTPUT);

}

void loop()
{
    DateTime now;

  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  uint32_t m = millis();
  logfile.print(m);          
  logfile.print(", ");
#if ECHO_TO_SERIAL
  Serial.print(m);         
  Serial.print(", ");
#endif

  now = RTC.now();
  logfile.print(now.unixtime());
  logfile.print(", ");
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime());
  Serial.print(", ");
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
#endif 

  
    Inside_temp= dht1.readTemperature();
    Outside_temp=dht2.readTemperature();
    Serial.print("Temp of inside: ");
    Serial.print(Inside_temp);
    logfile.print(", ");
    logfile.print(Inside_temp);
    Serial.print(" Celsius ;");
    Serial.print("Temp of outside: ");
    Serial.print(Outside_temp);
    logfile.print(", ");
    logfile.print(Outside_temp);
    Serial.println(" Celsius");
    
    delay(2000);
  if(Outside_temp<25)
  {
    if(Outside_temp-Inside_temp>=1)
    {
      Serial.println("ON");
      logfile.println(", ON");
      digitalWrite(LED,HIGH);
    }
    else if(Inside_temp-Outside_temp>=7)
    {
      Serial.println("ON");
      logfile.println(", ON");
      digitalWrite(LED,HIGH);
    }
    else
    {
      Serial.println("OFF");
      logfile.println(", OFF");
      digitalWrite(LED,LOW);
     }
  }
  else
  {
    if(Inside_temp<25)
    {
      Serial.println("ON");
      logfile.println(", ON");
      digitalWrite(LED,HIGH);
    } 
    else
    {
      
      Serial.println("OFF");
      logfile.println(", OFF");
      digitalWrite(LED,LOW);
    }
  }


      if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();

  logfile.flush();
}
