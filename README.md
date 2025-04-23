# ESP8266 Desk Clock with a 20x4 LCD I2C Display

Use an ESP8266 to drive an LCD to display time, weather, room climate and notes, all remotely commanded by Arduino IoT Cloud.

---

## Requirements

> [!NOTE]
> You may edit the code to add, remove or modify features if you do not meet these requirements and you know what you are doing.

- Active Wi-Fi connection
- An LCD 20x4 screen with an I2C interface
- A DHT22 Temperature and Humidity Sensor (I do not recommend DHT11 sensors because of wide errors in their humidity readings).
- Arduino IoT Cloud connection with these variables below
- - String alert : RW - To send and alert or reminder to display on your screen
  - float arh : RO - Send relative humidity reading from DHT22 to cloud
  - float atemp : RO - Send temperature reading from DHT22 to cloud
  - bool tracfetch : RW - Turn Last.FM API calls for displaying "Now Playing" tracks
  - CloudDimmedLight bklt :RW - Turn ON/OFF or dim the LCD display backlight
- Separate connection to LCD backlight via PWM for backlight dimming

---

## Features

- Displays time fetched from the Internet using NTP
- Displays greetings based on time of day
- Ambient temperature and relative humidity display from a DHT22 sensor
- Displays reminders sent through Arduino IoT Cloud
- Displays the artist and track name you are listening to (fetched from Last.FM API)
- Full backlight control including dimming
- All control via Arduino Cloud IoT without the use of physical buttons
- Splits long texts into multiple pieces so it can be displayed in order, as the mapping in 20x4 displays is weird [(see here)](https://forum.arduino.cc/t/20x4-lcd-screen-only-prints-to-line-0-and-2/121300/8). It can handle strings larger than the maximum character limit of the LCD and prints the remaining characters on a new (cleared) screen after a delay.

---

### Required Libraries

- ESP8266WiFi
- ArduinoIoTCloud
- LiquidCrystal_I2C (hd44780 is an alternative too)
- WiFiUdp
- NTPClient
- TimeLib
- JsonListener
- ESP8266HTTPClient
- DHTesp

---

### Todo (Hopes and Dreams)

- Add weather display including storm or snow alerts and display AQI using the OpenWeatherMap API.
- Fetch quotes, new words and other fun stuff from the internet to display on the LCD
- Add pictures, wiring diagrams to this readme
- Clean up code and improve documentation

