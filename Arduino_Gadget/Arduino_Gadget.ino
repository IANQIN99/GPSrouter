#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define DHTTYPE           DHT11
#define OLED_RESET 5
#define BUT1 A0
#define BUT2 A1
#define KNOB A2
#define VIBR 2
#define DPIN 3
#define LED 4
#define RXPin 10
#define TXPin 11

int state = 0;
int setstate = 0;
int quit = 0;
int h = 0;
int m = 0;
int s = 0;
int ds = 0;
int ampm = 0;
int j = 1;
double lattitude = 0;
double longtitude = 0;
int UTCyear = 0;
int UTCmonth = 0;
int UTCday = 0;
int UTChour = 0;
int UTCminute = 0;
int UTCsecond = 0;
unsigned long c = 0;
static const uint32_t GPSBaud = 9600;

DHT_Unified dht(DPIN, DHTTYPE);
Adafruit_SSD1306 display(OLED_RESET);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void bar(void);
void homescreen(void);
void menuscreen(int);
void screen(void);
void timescreen(void);
void HTscreen(void);
void GPSscreen(void);
void GPSWait(void);

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  ss.begin(GPSBaud);
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(KNOB, INPUT);
  pinMode(VIBR, OUTPUT);
  pinMode(RXPin, OUTPUT);
  digitalWrite(RXPin, HIGH);
  digitalWrite(VIBR, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (millis() <= 400)
  {
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(5, 0);
    display.println("PSYDO");
    display.display();
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  switch (state)
  {
    case 0:
      {
        homescreen();
        pinMode(RXPin, OUTPUT);
        digitalWrite(RXPin, HIGH);
        if (!digitalRead(BUT1) || !digitalRead(BUT2))
        {
          if (!digitalRead(BUT1))
          {
            delay(500);
            state = setstate;
          }
          else
          {
            delay(500);
            state = 9;
          }
          break;
        }
        break;
      }
    case 9:
      {
        pinMode(RXPin, OUTPUT);
        digitalWrite(RXPin, HIGH);
        setstate = map(analogRead(KNOB), 950, 50, 1, 8);
        menuscreen(setstate);
        if (!digitalRead(BUT1) || !digitalRead(BUT2))
        {
          if (!digitalRead(BUT1))
          {
            delay(500);
            state = setstate;
          }
          else
          {
            delay(500);
            state = 0;
          }
          break;
        }
        break;
      }
    case 1:
      {
        HTscreen();
        if (!digitalRead(BUT1) || !digitalRead(BUT2))
        {
          delay(500);
          state = 0;
          digitalWrite(VIBR, LOW);
        }
        break;
      }
    case 2:
      {
        pinMode(RXPin, INPUT);
        while (digitalRead(BUT1) && digitalRead(BUT2) && j)
        {
          bar();
          GPSWait();
        }
        if (j) delay(500);
        j = 0;
        GPSscreen();
        while (ss.available() > 0 && digitalRead(BUT1) && digitalRead(BUT2))
          if (gps.encode(ss.read()))
          {
            lattitude = gps.location.lat();
            longtitude = gps.location.lng();
            UTCyear = gps.date.year();
            UTCmonth = gps.date.month();
            UTCday = gps.date.day();
            UTCsecond = gps.time.second();
            UTCminute = gps.time.minute();
            UTChour = gps.time.hour();
          }

        if (!digitalRead(BUT1) || !digitalRead(BUT2))
        {
          delay(500);
          state = 0;
          j = 1;
        }
        break;
      }
    case 3:
      {
        screen();
        break;
      }
    case 4:
      {
        screen();
        break;
      }
    case 5:
      {
        screen();
        break;
      }
    case 6:
      {
        screen();
        break;
      }
    case 7:
      {
        screen();
        break;
      }
    case 8:
      {
        quit = analogRead(KNOB);
        timescreen();
        if (!digitalRead(BUT1) && digitalRead(BUT2))
        {
          delay(500);
          h++;
        }
        if (!digitalRead(BUT2) && digitalRead(BUT1))
        {
          delay(500);
          m++;
        }
        if (!digitalRead(BUT1) && !digitalRead(BUT2))
        {
          delay(500);
          s = 0;
          ds = 0;
          c = millis();
        }
        if (abs(analogRead(KNOB) - quit) > 8
           )state = 0;
        break;
      }
  }
  bar();

}


void bar(void)
{
  s = (int)((millis() - c + ds) / 1000);
  if (s >= 60)
  {
    if (s == 60)
    {
      s = 0;
      m++;
      c = millis();
      ds = 0;
    }
    if (s > 60)
    {
      m += (int)(s / 60);
      s = s % 60;
      ds =  s * 1000;
      c = millis();
    }
  }
  if (m == 60)
  {
    h++;
    m = 0;
  }
  if (h == 12 && ampm == 0)
  {
    ampm = 1 ;
  }
  if (h == 13 && ampm == 1)
  {
    h = 1;
    ampm = 2;
  }
  if ( h == 12 && ampm == 2)
  {
    h = 0;
    ampm = 0;
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("[--]");
  display.setCursor(36, 0);
  if (h < 10)display.println("0" + String(h));
  else display.println(String(h));
  display.setCursor(48, 0);
  display.println(":");
  display.setCursor(54, 0);
  if (m < 10)display.println("0" + String(m));
  else display.println(String(m));
  display.setCursor(66, 0);
  display.println(":");
  display.setCursor(72, 0);
  if (s < 10)display.println("0" + String(s));
  else display.println(String(s));
  display.setCursor(90, 0);
  if (!ampm) display.println("AM");
  else display.println("PM");
  display.setCursor(104, 0);
  display.println(" __% ");
  display.display();
  if (state != 2 || j) display.clearDisplay();
}

void homescreen(void)
{
  display.setCursor(0, 25);
  display.println("QUICK");
  display.setCursor(104, 25);
  display.println("MENU");
}

void menuscreen(int setstate)
{
  display.setCursor(0, 25);
  display.println("SELECT");
  display.setCursor(104, 25);
  display.println("BACK");
  for (int i = 1; i <= 4; i++)
  {
    if (i == setstate) display.setTextColor(BLACK, WHITE);
    else display.setTextColor(WHITE);
    display.setCursor(36 + 18 * ( i - 1), 9);
    display.println("F" + String(i));
  }
  for (int i = 0; i < 4; i++)
  {
    if ((i + 5) == setstate) display.setTextColor(BLACK, WHITE);
    else display.setTextColor(WHITE);
    display.setCursor(36 + 18 * i, 17);
    display.println("F" + String(i + 5));
  }
}

void screen(void)
{

}

void timescreen(void)
{
  display.setCursor(0, 25);
  display.println("HOU +");
  display.setCursor(98, 25);
  display.println("MIN +");
  display.setCursor(30, 9);
  display.println("TIME CONFIG");
  display.setCursor(0, 17);
  display.println("TURN THE KNOB TO QUIT");
}

void HTscreen(void)
{
  int k = 0;
  sensors_event_t event;
  display.setCursor(0, 9);
  dht.temperature().getEvent(&event);
  display.println("TEMPERATURE:" + String(int(event.temperature)) + "  C");
  if ((int)event.temperature > 30 && digitalRead(BUT1) && digitalRead(BUT2) && !k)
  {
    digitalWrite(VIBR, HIGH);
    k = 1;
  }
  if (k)
  {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  display.setCursor(0, 17);
  dht.humidity().getEvent(&event);
  display.println("HUMIDITY:   " + String(int(event.relative_humidity)) + "  %");
  display.setCursor(0, 25);
  display.println("PRESS ANY KEY TO QUIT");
}

void GPSWait(void)
{
  display.setCursor(0, 17);
  display.println("CHECK PPS ->RED FLASH");
}

void GPSscreen(void)
{
  display.setCursor(0, 9);
  display.println("LOC :" + String(lattitude) + "," + String(longtitude));
  display.setCursor(0, 17);
  if (s % 10 <= 6)
    display.println("DATE:" + String(UTCyear) + "/" + String(UTCmonth) + "/" + String(UTCday));
  else display.println("TIME:" + String(UTChour + 8) + ":" + String(UTCminute) + ":" + String(UTCsecond));
  display.setCursor(0, 25);
  display.println("PRESS ANY KEY TO QUIT");
}

