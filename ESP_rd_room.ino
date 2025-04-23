#include "thingProperties.h"
#include <Wire.h> 
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <LiquidCrystal_I2C.h>

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h> 

#include <JsonListener.h>
#include <time.h>
#include <ESP8266HTTPClient.h>

#include "DHTesp.h"



//OWM
String OPEN_WEATHER_MAP_APP_ID = "";//Enter your OWM key

float OPEN_WEATHER_MAP_LOCATION_LAT = 8.510624436495792;
float OPEN_WEATHER_MAP_LOCATION_LON = 76.9514450807469;
String OPEN_WEATHER_MAP_LANGUAGE = "en";
boolean IS_METRIC = true;
unsigned long lastUpdate = 0;
const long updateInterval = 19000;

// Tracks last request time - Last.FM
unsigned long lastRequestTime = 0;  
const unsigned long interval2 = 17000;

const char* lastfm_api_key = ""; //Enter your Last.FM API key
const char* username = "";//Enter your Last.FM username


//EXTRAs
const long utcOffsetInSeconds = 19800;
const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
unsigned long previousMillis = 0;
const long interval = 1000;
float temp;
float rh;
float stat;
int tfr=0;
int currentHour = 0;
int currentMinute = 0;
String tr="";
String ar="";
float humidity=0,temperature=0;
int scrollIndex = 0;
const int ledPin = 13;




//LCD
byte bell[8]  = {0x04,0x0e,0x0e,0x0e,0x1f,0x00,0x04,0x00};
byte note[8]  = {0x02,0x03,0x02,0x0e,0x1e,0x0c,0x00,0x00};
byte clockface[8] = {0x00,0x0e,0x15,0x17,0x11,0x0e,0x00,0x00};

byte duck[8]  = {0x00,0x0c,0x1d,0x0f,0x0f,0x06,0x00,0x00};
byte check1[8] = {0x00,0x01,0x03,0x16,0x1c,0x08,0x00,0x00};

byte smile[8] = {0x00,0x0a,0x0a,0x00,0x00,0x11,0x0e,0x00};
byte thermo[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B01110,
  B11111,
  B01110
};
byte hm[8] = {
  0b00100,  //   ⬆
  0b00100,  //   |
  0b01110,  //  ###
  0b01110,  //  ###
  0b01110,  //  ###
  0b11111,  // #####
  0b11111,  // #####
  0b01110   //  ###
};


LiquidCrystal_I2C lcd(0x27,20,4); 
//hd44780_I2Cexp lcd;
DHTesp dht;
ComfortState cf;
WiFiClient client1;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds, 60000);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 8);
  //lcd.begin(20,4);
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("INITIALIZING...");
  lcd.setCursor(0, 1);
  dht.setup(2, DHTesp::DHT22);
  twoline1("Attempting to connect to Wi-Fi network");
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  while (WiFi.localIP().toString() == "0.0.0.0") {
    lcd.print(".");
    delay(500);
  }
  //lcd.noBacklight();
  timeClient.begin();
  delay(1500);
  setDebugMessageLevel(0);
  ArduinoCloud.printDebugInfo();
  lcd.createChar(6, bell);  
  lcd.createChar(2, clockface); 
  lcd.createChar(4, note);
  lcd.createChar(3, duck);
  lcd.createChar(5, check1);
  lcd.createChar(7, smile);
  lcd.createChar(8, thermo);
  lcd.createChar(9, hm);
}
      




void loop() {
  ArduinoCloud.update();
  timeip();
  ArduinoCloud.update();
  onAlertChange();
  ArduinoCloud.update();
  if(tfr==1){
  fetchNowPlaying();
  ArduinoCloud.update();}
  //wttr();
  ambi();
}

void timeip()
{
    timeClient.update();
    setTime(timeClient.getEpochTime());
    currentHour = hour();
    currentMinute = minute();
    int currentDay = weekday();
    int currentMonth = month();
    int currentYr = year();
    int currday=day();

    String formattedDate = String(String(currday) + " " + months[currentMonth - 1]) + " " +   String(currentYr);

    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", currentHour, currentMinute);
    // Display on LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.write(byte(2));
    lcd.print("Time: ");
    lcd.print(timeStr);  
    
    lcd.setCursor(0, 1);
    lcd.write(byte(5));
    lcd.print("Date: ");
    lcd.print(formattedDate);
    String weekDays[]={"Sunday","Monday","Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    lcd.setCursor(0, 2);
    lcd.write(byte(3));lcd.print("It");lcd.write(39); lcd.print("s ");
    lcd.print(weekDays[currentDay-1]);
    //Serial.println(currentDay);
    String greeting;
    if (currentHour < 12) {
        greeting = "Good Morning!";
    } else if (currentHour < 16) {
        greeting = "Good Afternoon!";
     } else if (currentHour < 20) {
        greeting = "Good Evening!";      
    } else {
        greeting = "Good Night!";
    }
    lcd.setCursor(0, 3);
    lcd.write(byte(7));
    lcd.print(greeting);

    if (alert != "No new alerts" && alert != "")
    {
      lcd.setCursor(17,0);
      lcd.write(byte(6));
    }
    if(temperature>30)
    {
      lcd.setCursor(18,0);
      lcd.write(byte(8));

    }
    if(humidity>50)
    {
      lcd.setCursor(19,0);
      lcd.write(byte(9));      
    }
    
    //lcd.print("WiFi: ");
    //WiFi.disconnect(false);
    //lcd.print(WiFi.RSSI());lcd.print(" dBm");
    delay(1800);
}

void onAlertChange()  
{
  //int l=alert.length();
  String s2= alert; 
  remtwoline1(s2);
  remtwoline1(s2);
  //Serial.println(alert);
}

void onBkltChange()
{ uint8_t brightness = map(bklt.getBrightness(), 0, 100, 0, 255);
  if (bklt.getSwitch()) {
    lcd.backlight();

    analogWrite(ledPin, brightness);  

  }

  else{

    analogWrite(ledPin, LOW); 
    lcd.noBacklight();

  }
}

void onTracfetchChange()  {
  if (tracfetch==true)
  tfr=1;
  else
  tfr=0;
}

void fetchNowPlaying() {
  //if (WiFi.status() == WL_CONNECTED) {
    if (millis() - lastRequestTime >= interval2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connecting to LastFM");
      lcd.setCursor(0, 1);
      lcd.print("-------");
      lastRequestTime = millis();
      HTTPClient http;
      String api_url = "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=" + String(username) + "&api_key=" + String(lastfm_api_key) + "&format=json&limit=1";
      http.begin(client1,api_url);
      int httpCode = http.GET();  
      if (httpCode > 0) {   
             String payload = http.getString();
             //Serial.println("Response: " + payload);
             int nowPlayingIndex = payload.indexOf("\"@attr\":{\"nowplaying\":\"true\"}");
             if (nowPlayingIndex != -1) {   
              String trackName = extractValue(payload, "\"name\":\"");
              String artistName = extractValue(payload, "\"#text\":\""); 
              tr = trackName;
              ar = artistName;  
             }
             else
             {
              tr= "N/A";
              ar= " N/A";
             }
      }
      else 
           { Serial.println("❌ HTTP Request Failed"); 
            tr= "N/A";
            lcd.setCursor(0, 3);
            lcd.print("HTTP Request Failed");
            ar= "N/A";}

      http.end();
    
  }
  String track= ar + " - " + tr;
  //String track= "Now Playing: " + ar + " - " + tr;
  tractwoline1(track);
}

void ambi()
{
  lcd.clear();
  humidity = dht.getHumidity();
  arh=humidity;
  
  temperature = dht.getTemperature();
  atemp=temperature;

  lcd.setCursor(3, 0);lcd.print("AMBIENT CLIMATE");                    
  lcd.setCursor(0, 1);lcd.write(byte(8));lcd.print("Temp: ");
  lcd.print(temperature);lcd.write(223);lcd.print("C");
  lcd.setCursor(0, 2); lcd.write(byte(9));
  lcd.print("RH %: ");
  lcd.print(humidity,1);lcd.print("%");
  lcd.setCursor(0, 3);
  delay(1010);
}



void standby()
{
    timeClient.update();
    setTime(timeClient.getEpochTime());
    currentHour = hour();
    currentMinute = minute();
    int currentDay = weekday();
    int currentMonth = month();
    int currentYr = year();

    String formattedDate = String(months[currentMonth - 1]) + " " + String(currentDay)+ " " + String(currentYr);

    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", currentHour, currentMinute);
    // Display on LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.write(byte(2));
    lcd.print("Time: ");
    lcd.print(timeStr);  
    
    lcd.setCursor(0, 1);
    lcd.write(byte(5));
    lcd.print("Date: ");
    lcd.print(formattedDate);
    String weekDays[]={"Sunday","Monday","Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    lcd.setCursor(0, 2);
    lcd.write(byte(3));lcd.print("It");lcd.write(39); lcd.print("s ");
    lcd.print(weekDays[currentDay-1]);
    //Serial.println(currentDay);
    String greeting;
    if (currentHour < 12) {
        greeting = "Good Morning!";
    } else if (currentHour < 16) {
        greeting = "Good Afternoon!";
     } else if (currentHour < 20) {
        greeting = "Good Evening!";      
    } else {
        greeting = "Good Night!";
    }
    lcd.setCursor(0, 3);
    if(alert.length()<20)
    {
      lcd.write(byte(6));
      lcd.print(alert);
    }
    else{
      lcd.write(byte(7));
      lcd.print(greeting);
    }
    
    if (alert != "No new alerts" && alert != "")
    {
      lcd.setCursor(17,0);
      lcd.write(byte(6));
    }
    if(temperature>30)
    {
      lcd.setCursor(18,0);
      lcd.write(byte(8));

    }
    if(humidity>50)
    {
      lcd.setCursor(19,0);
      lcd.write(byte(9));      
    }
    //lcd.print("WiFi: ");
    //WiFi.disconnect(false);
    //lcd.print(WiFi.RSSI());lcd.print(" dBm");
    //delay(1800);
}



void twoline1(String st)
{
  lcd.clear();
  lcd.print(st);
  for (int i = 0; i < st.length(); i += 80) {  // Display in 32-char chunks
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(st.substring(i, i + 20)); // First 16 chars
    lcd.setCursor(0, 1);
    lcd.print(st.substring(i + 20, i + 40)); // Next 16 chars
    lcd.setCursor(0, 2);
    lcd.print(st.substring(i + 40, i + 60)); // Next 16 chars
    lcd.setCursor(0, 3);
    lcd.print(st.substring(i + 60, i + 80)); // Next 16 chars
    delay(1150); // Wait before next part
    }
}

void tractwoline1(String st)
{
  lcd.print(st);
  for (int i = 0; i < st.length(); i += 79) {  // Display in 32-char chunks
    lcd.clear();
    lcd.write(byte(4));
    lcd.setCursor(1, 0);
    lcd.print(st.substring(i, i + 19)); // First 16 chars
    lcd.setCursor(0, 1);
    lcd.print(st.substring(i + 19, i + 39)); // Next 16 chars
    lcd.setCursor(0, 2);
    lcd.print(st.substring(i + 39, i + 59)); // Next 16 chars
    lcd.setCursor(0, 3);
    lcd.print(st.substring(i + 59, i + 79)); // Next 16 chars
    delay(1290); // Wait before next part
    }
}

void remtwoline1(String st)
{
  lcd.print(st);
    for (int i = 0; i < st.length(); i += 79) {  // Display in 32-char chunks
    lcd.clear();
    lcd.write(byte(6));
    lcd.setCursor(1, 0);
    lcd.print(st.substring(i, i + 19)); // First 16 chars
    lcd.setCursor(0, 1);
    lcd.print(st.substring(i + 19, i + 39)); // Next 16 chars
    lcd.setCursor(0, 2);
    lcd.print(st.substring(i + 39, i + 59)); // Next 16 chars
    lcd.setCursor(0, 3);
    lcd.print(st.substring(i + 59, i + 79)); // Next 16 chars
    delay(1290); // Wait before next part
    }
}

String extractValue(String payload, String key) {
    int startIndex = payload.indexOf(key);
    if (startIndex == -1) return "Not Found";  // Return if key is missing

    startIndex += key.length();
    int endIndex = payload.indexOf("\"", startIndex);
    return payload.substring(startIndex, endIndex);
}

void scrollLine(int row, String msg) {
  lcd.setCursor(0, row);
  lcd.print(msg.substring(scrollIndex, scrollIndex + 20));

  scrollIndex++;
  if (scrollIndex > msg.length() - 20) {
    scrollIndex = 0;
  }
}