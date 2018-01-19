//LIB is Adafruit IO Arduino
// https://learn.adafruit.com/mqtt-adafruit-io-and-you/intro-to-adafruit-mqtt

/*********************************************************************
The Sofia Project
Date: April 2017
Author: Quentin
*********************************************************************/

/**********************Internet of Things ***************************/
//#include <ESP8266WiFi.h>
//#include "Adafruit_MQTT.h"
//#include "Adafruit_MQTT_Client.h"
#include "config.h"

// WiFi
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
//#include <WiFiManager.h>
//#define WLAN_SSID       "MY_WIFI_NAME"
//#define WLAN_PASS       "MY_WIFI_PASSWORD"

// Feeds and Subscriptions
AdafruitIO_Feed *IOTalert = io.feed("SP_Alert");
AdafruitIO_Feed *IOTpingBox = io.feed("SP_PingBox");
AdafruitIO_Feed *IOTactiv = io.feed("SP_Activity");

AdafruitIO_Feed *IOTmessage = io.feed("SP_Message");
AdafruitIO_Feed *IOTping = io.feed("SP_Ping");

/**************************** Ping *******************************/
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

const IPAddress remote_ip(8, 8, 8, 8); //Adress to ping (google DNS)

/**************************** OLED Display *******************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display SPI
#define OLED_MOSI   D7  //SDA
#define OLED_CLK    D5  //SCL
#define OLED_DC     D2
#define OLED_CS     D10
#define OLED_RESET  D1
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Screen saver
#define NUMFLAKES 5
#define XPOS 0
#define YPOS 1
#define DELTAY 2

/**************************** Other *******************************/
// Inputs
#define potPin      A0
#define clickButton D3
#define tiltPin     D6

// LEDs
#define RGB_red     D4
#define RGB_green   D8
#define RGB_blue    D0


const unsigned char iconFigure [] PROGMEM = {
  // 'Strawberry'
  0x00, 0x44, 0x00, 0x24, 0x00, 0x6c, 0x0f, 0x7c, 0x1b, 0xbc, 0x3f, 0xdc, 0x6f, 0xaf, 0x59, 0x70,
  0x7f, 0xf8, 0xef, 0xb8, 0xfd, 0xe8, 0xfb, 0xe8, 0x9f, 0x70, 0xff, 0xe0, 0xe7, 0xc0, 0x7e, 0x00,
};

const unsigned char mailIcon [] PROGMEM = {
  0x00,0x00,0x00
  ,0x00,0x00,0x00
  ,0x3f,0xff,0xc0
  ,0xc0,0x00,0x30
  ,0x80,0x00,0x10
  ,0x90,0x00,0x90
  ,0x88,0x01,0x10
  ,0x84,0x02,0x10
  ,0x83,0x04,0x10
  ,0x81,0x98,0x10
  ,0x81,0xf8,0x10
  ,0x82,0x64,0x10
  ,0x84,0x02,0x10
  ,0x88,0x01,0x10
  ,0x90,0x00,0x90
  ,0x80,0x00,0x10
  ,0xc0,0x00,0x30
  ,0x3f,0xff,0xc0
  ,0x00,0x00,0x00
  ,0x00,0x00,0x00
};

const unsigned char wifiIcon [] PROGMEM = {
0x00,0x00,0x00
,0x00,0x00,0x00
,0x00,0x00,0x00
,0x03,0xfc,0x00
,0x0f,0xff,0x00
,0x38,0x01,0xc0
,0x70,0x60,0xe0
,0xc3,0xfc,0x30
,0x8f,0x0f,0x10
,0x1c,0x03,0x80
,0x18,0xf1,0x80
,0x03,0xfc,0x00
,0x03,0x0c,0x00
,0x00,0x00,0x00
,0x00,0x60,0x00
,0x00,0xf0,0x00
,0x00,0x60,0x00
,0x00,0x00,0x00
,0x00,0x00,0x00
,0x00,0x00,0x00
};

const unsigned char alertIcon [] PROGMEM = {
  0x00,0x00,0x00
  ,0x00,0xf0,0x00
  ,0x01,0x98,0x00
  ,0x01,0x08,0x00
  ,0x03,0x0c,0x00
  ,0x02,0x04,0x00
  ,0x04,0x02,0x00
  ,0x04,0x62,0x00
  ,0x08,0x61,0x00
  ,0x18,0x61,0x80
  ,0x10,0x60,0x80
  ,0x30,0x60,0xc0
  ,0x20,0x00,0x40
  ,0x40,0x00,0x20
  ,0x40,0x60,0x20
  ,0x80,0x00,0x10
  ,0x80,0x00,0x10
  ,0xc0,0x00,0x30
  ,0x7f,0xff,0xe0
  ,0x00,0x00,0x00

};

/********************* Program Variables ****************************/
String pingQ = "";
String message = "";


void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();

  pinMode(clickButton, INPUT_PULLUP);
  pinMode(tiltPin, INPUT_PULLUP);

  pinMode(RGB_red, OUTPUT);
  pinMode(RGB_green, OUTPUT);
  pinMode(RGB_blue, OUTPUT);

/************** Start Sequence *****************/
  display.drawRect(0, 0, 120, 64, WHITE);
  writeCenter("IL", 0, 12, 120, 2, WHITE);
  writeCenter("SCATOLINA", 0, 32, 120, 2, WHITE);
  writeCenter("DI SOFIA", 0, 52, 120, 2, WHITE);
  display.display();
  delay(1500);

  //Wifi_connect();

  IOT_connect();

  IOTactiv->save("[Start]");

}
//=========================================================================
//====================================== LOOP =============================
void loop()
{
  io.run();

  while(true)
  {
    mainMenu();
  }

}
//=========================================================================
//=========================================================================

////////////////// message handler //////////////////////
void handleMessage(AdafruitIO_Data *data)
{
  IOTactiv->save("*Received: "+String(data->value())+"*");

  if(String(data->value()) == "#alive")
  {
    IOTactiv->save("*Alive*");
  }
  else
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(119,4);
    display.println("M");
    display.display();
    delay(200);

    digitalWrite(RGB_green, HIGH);

    message = data->value();
//    IOTactiv->save("*Received: "+String(message)+"*");
  }
}

////////////////// Ping handler //////////////////////
void handlePing(AdafruitIO_Data *data)
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(119,4);
  display.println("P");
  display.display();
  delay(200);

  pingQ = data->value();
}

////////////////// CREATE MAIN *MENU* //////////////////////
void mainMenu()
{
  int choice = chooseMenuButton();

  if (choice == 1)
  {
    messagePage();
  }
  else if(choice == 2)
  {
    wifiPage();
  }
  else if(choice == 3)
  {
    alertPage();
  }
}

///////////////// SELECT *MENU* BUTTON /////////////////////////
int chooseMenuButton()
{
  int choice;
  int potValue;
  bool clicked = true;

  int i = 0;
  while(clicked == true)
  {
    i++;
    if(i < 10)
    {
      io.run();
      i = 0;
    }

    potValue = analogRead(potPin);
    if (potValue <= 300)
    {
      choice = 1;
    }
    else if (potValue >= 500)
    {
      choice = 3;
    }
    else
    {
      choice = 2;
    }

    showMenuButton(choice);
    clicked = digitalRead(clickButton);
    delay(10);
  }
  return choice;
}

//////////////////// DISPLAY *MENU* BUTTONS ///////////////////////////////
void showMenuButton(int choice)
{
  uint16_t choice1Color;
  uint16_t choice1bColor;
  uint16_t choice2Color;
  uint16_t choice2bColor;
  uint16_t choice3Color;
  uint16_t choice3bColor;

  if (choice == 1)
  {
    choice1Color = WHITE; //SELECTED
    choice1bColor = BLACK;

    choice2Color = BLACK;
    choice2bColor = WHITE;

    choice3Color = BLACK;
    choice3bColor = WHITE;
  }
  else if (choice == 2)
  {
    choice1Color = BLACK;
    choice1bColor = WHITE;

    choice2Color = WHITE; //SELECTED
    choice2bColor = BLACK;

    choice3Color = BLACK;
    choice3bColor = WHITE;
  }
  else
  {
    choice1Color = BLACK;
    choice1bColor = WHITE;

    choice2Color = BLACK;
    choice2bColor = WHITE;

    choice3Color = WHITE; //SELECTED
    choice3bColor = BLACK;
  }

  display.clearDisplay();
  writeCenter("Main Menu", 0, 4, 128, 1, WHITE);

  display.fillRect(0, 10, 128, 54 , BLACK); //clear the button region
  //Big left button
  display.fillRect(0, 10, 62, 54 , choice1Color);
  display.drawRect(0, 10, 62, 54 , WHITE);
  display.drawBitmap(22, 25,  mailIcon, 20, 20, choice1bColor);
  //writeCenter("Mail", 0, 37, 62, 1, choice1bColor);
  if (message.length() > 0)
  {
    writeCenter("(1)", 0, 50, 65, 1, choice1bColor);
  }
  else
  {
  }

  //small upper right button
  display.fillRect(66, 10, 59, 25 , choice2Color);
  display.drawRect(66, 10, 59, 25 , WHITE);
  display.drawBitmap(86, 12,  wifiIcon, 20, 20, choice2bColor);
  //writeCenter("Wifi", 66, 23, 59, 1, choice2bColor);
  //small lower right button
  display.fillRect(66, 39, 59, 25 , choice3Color);
  display.drawRect(66, 39, 59, 25 , WHITE);
  display.drawBitmap(86, 41,  alertIcon, 20, 20, choice3bColor);
  //writeCenter("Alert bf", 66, 52, 59, 1, choice3bColor);

  display.display();
}

/////////// MESSAGE_PAGE ///////////////
void messagePage()
{
  digitalWrite(RGB_green, LOW);
  IOTactiv->save("[Mail]");

  if (message.length() > 0)
  {
    display.clearDisplay();
    writeCenter("1 new message", 0, 30, 128, 1, WHITE);
    display.display();
    delay(1000);

    display.clearDisplay();
    writeCenter("Message", 0, 4, 128, 1, WHITE);
    display.drawRect(0, 10, 127, 42, WHITE);
    writeCenter(message, 0, 16, 128, 1, WHITE);
    display.display();
    message = "";
  }
  else
  {
    display.clearDisplay();
    writeCenter("No new messages", 0, 30, 128, 1, WHITE);
    display.display();
    message = "No new messages";
  }
  message = "";

  delay(500);
  waitClick();
}

///////// ALERT_PAGE ////////////
void alertPage()
{
  digitalWrite(RGB_red, HIGH);
  IOTactiv->save("[Alert]");
  delay(200);

  int potValue;
  int answer;
  bool clicked = true;

  while(clicked == true)
  {
    potValue = analogRead(potPin);
    if (potValue <= 400)
    {
      display.clearDisplay();
      writeCenter("This will contact;Quentin. Sure you; want to continue?", 0, 10, 128, 1, WHITE);
      display.drawRect(50, 47, 28, 15 , WHITE);
      writeCenter("NO", 0, 55, 128, 1, WHITE);
      display.display();
      answer = 1;
    }
    else if (potValue > 400)
    {
      display.clearDisplay();
      writeCenter("This will contact;Quentin. Sure you; want to continue?", 0, 10, 128, 1, WHITE);
      display.drawRect(50, 47, 28, 15 , WHITE);
      writeCenter("YES", 0, 55, 128, 1, WHITE);
      display.display();
      answer = 2;
    }
    delay(50);
    clicked = digitalRead(clickButton);
  }

  if(answer == 1)
  {
    display.clearDisplay();
    writeCenter("Request canceled!", 0, 30, 128, 1, WHITE);
    display.display();
  }
  else if (answer == 2)
  {
    IOTalert->save("99"); // Sends 99 if alert
    display.clearDisplay();
    writeCenter("Quentin got informed!", 0, 30, 128, 1, WHITE);
    display.display();
  }

  delay(100);
  waitClick();

  digitalWrite(RGB_red, LOW);
}

//////////// WIFI_PAGE ////////////
void wifiPage()
{
  IOTactiv->save("[Wifi]");
  int pingTimeS;
  int pingTimeQ;
  String qualityS;
  String qualityQ;

  display.clearDisplay();
  writeCenter("Gathering;network info...;(Long push to exit)", 0, 30, 128, 1, WHITE);
  display.display();
  //delay(500);

  while(true)
  {
    digitalWrite(RGB_blue, HIGH);
    delay(200);
    digitalWrite(RGB_blue, LOW);

    io.run();
    pingTimeQ = pingQ.toInt();
    //pingQ = "";
    qualityQ = evalPing(pingTimeQ);

    Ping.ping(remote_ip,4);
    pingTimeS = Ping.averageTime();
    IOTpingBox->save(pingTimeS);
    qualityS = evalPing(pingTimeS);

    display.clearDisplay();
    writeCenter("Network Info", 0, 4, 128, 1, WHITE);
    display.drawRect(0, 13, 127, 25, WHITE);
    writeCenter("Sofi's internet:;   "+String(qualityS)+" ("+String(pingTimeS)+" ms)", 0, 21, 128, 1, WHITE);
    display.drawRect(0, 37, 127, 25, WHITE);
    writeCenter("Quentin's internet:;   "+String(qualityQ)+" ("+String(pingTimeQ)+" ms)", 0, 45, 128, 1, WHITE);
    display.display();

    // Exit sequence
    if (digitalRead(clickButton) == false)
    {
      display.clearDisplay();
      writeCenter("Closing;network info...;(Release push button)", 0, 30, 128, 1, WHITE);
      display.display();
      delay(1500);
      return;
    }

  }
}

/////////// TBD_PAGE ////////////
//void tbdPage()
//{
//  display.clearDisplay();
//  writeCenter("Function not;available yet", 0, 30, 128, 1, WHITE);
//  display.display();
//  delay(500);

//  waitClick();
//}






//////////////////////////// CONNECT TO IOT //////////////////////////////
void IOT_connect()
{
  digitalWrite(RGB_blue, HIGH);

  display.clearDisplay();
  writeCenter("Connecting to the;scatolina service", 0, 28, 120, 1, WHITE);
  display.display();

  io.connect();

  IOTmessage->onMessage(handleMessage);
  IOTping->onMessage(handlePing);

  int i = 0;
  while(io.status() < AIO_CONNECTED)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(6 * i,52);
    display.println("* ");
    //writeCenter("* ", 6 * i, 50, 120, 1, WHITE);
    display.display();
    delay(500);
    i = i + 1;
  }

  display.clearDisplay();
  writeCenter("Connected", 0, 32, 120, 1, WHITE);
  display.display();
  delay(500);

  digitalWrite(RGB_blue, LOW);
}
//////////////////////////////////////////////////////////////

//////////////////////// CONNECT TO WIFI /////////////////////
//void Wifi_connect()
//{
//  display.clearDisplay();
//  writeCenter("Connecting to WiFi...", 0, 10, 128, 1, WHITE);         // SAY CONNECTING TO WHAT + MOVEMENT ....
//  display.display();
//  digitalWrite(RGB_blue, HIGH);
  // Connect to WiFi access point.
//  Serial.println();
//  Serial.println();
//  Serial.print("Connecting to: ");
//  Serial.println(WLAN_SSID);

//  WiFiManager wifiManager;
//  wifiManager.resetSettings();
//  wifiManager.autoConnect("IlScatolinoWiFi");

  //WiFi.begin(WLAN_SSID, WLAN_PASS);
  //while (WiFi.status() != WL_CONNECTED) // CONNECTING
  //{
  //  beatBlink(RGB_blue, 2000, 1);
//    Serial.print(".");
  //}
//  Serial.println();

//  Serial.println("Connected to Wifi");   // CONNECTED
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//  display.clearDisplay();
//  writeCenter("Connected!", 0, 10, 128, 1, WHITE);
//  display.display();
//  digitalWrite(RGB_blue, LOW);
//  delay(500);

  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&IOTquestion);
//}
////////////////////////////////////////////////////////////////////




/////////////////////// CHOOSE ANSWER /////////////////////////////////
// Builds two buttons and change lighting of them acordingly.
// Eventually return the value true or false depending on
// The clicked button

bool chooseButton(String choice1, String choice2)
{
  bool choice;
  bool clicked = true;
  int potValue;

  while(clicked == true)
  {
    potValue = analogRead(potPin);
    if (potValue<=400)
    {
      choice = true;
    }
    else
    {
      choice = false;
    }
    showButton(choice1, choice2, choice);
    clicked = digitalRead(clickButton);
    delay(10);
  }
  return choice;
}

//////////////////// DISPLAY BUTTONS /////////////////////////////////
// Builds the two buttons with highligh depending on the provided choice
// showButton("1st choice", "second choice", selected)

void showButton(String choice1, String choice2, bool choice)
{
  uint16_t choice1Color;
  uint16_t choice2Color;
  if (choice ==false)
  {
    choice1Color = WHITE;
    choice2Color = BLACK;
  }
  else
  {
    choice1Color = BLACK;
    choice2Color = WHITE;
  }

  //display.clearDisplay();
  display.fillRect(0, 44, 128, 20 , BLACK); //clear the button region
  //left button
  display.fillRect(3, 44, 59, 20 , choice2Color);
  display.drawRect(3, 44, 59, 20 , WHITE);
  writeCenter(choice1, 3, 54, 59, 1, choice1Color);
  //right button
  display.fillRect(65, 44, 59, 20 , choice1Color);
  display.drawRect(65, 44, 59, 20 , WHITE);
  writeCenter(choice2, 65, 55, 59, 1, choice2Color);

  display.display();
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////// WRITE CENTER /////////////////////////////
// Centers text on horizontal frame. Also places the y reference in
// the middle of the text rather than top
// New line by ";" separator
void writeCenter(String text, int x0, int y0, int width, int sizeText, uint16_t color)
{
  display.setTextSize(sizeText);
  display.setTextColor(color);

  String texti;
  int w;
  int textLength;
  int x;
  int y;
  for (int i=0; i<4; i++)
  {
    texti = getSubString(text,';',i);
    w = texti.length();
    textLength = w*sizeText*6;
    x = x0 + (width - textLength)/2;
    y = y0 - sizeText*8/2 + i*(8+2);
    display.setCursor(x,y);
    display.println(texti);
  }
}
////////////////////////////////////////////////////////////////////

/////////////////////// SEPARATE STRINGS IN LINES //////////////////
String getSubString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++)
  {
    if(data.charAt(i)==separator || i==maxIndex)
    {
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
////////////////////////////////////////////////////////////////////

////////////////////////// BLINK //////////////////////////////////
void blinkLed(char LEDpin, int blinkTime)
{
  digitalWrite(LEDpin, HIGH);
  delay(blinkTime);
  digitalWrite(LEDpin, LOW);
}
///////////////////////////////////////////////////////////////////

////////////////////// HEARTBEAT BLINK ///////////////////////////
void beatBlink(char LEDpin, int beatTime, int times)
{
  int interval = beatTime/255/2;
  for (int i=1; i<=times; i++)
  {
    for (int j=1; j<255; j++)
    {
      analogWrite(LEDpin, j);
      delay(interval);
    }
    for (int j=255; j>0; j--)
    {
      analogWrite(LEDpin, j);
      delay(interval);
    }
  }
  analogWrite(LEDpin, 0);
}
/////////////////////////////////////////////////////////////////

///////////////////////// WAIT CLICK ////////////////////////////
void waitClick()
{
  display.fillRect(25, 55, 78, 10 , WHITE);
  display.drawRect(25, 55, 78, 10 , WHITE);
  writeCenter("Continue", 25, 60, 78, 1, BLACK);
  display.display();

  delay(500);

  bool clicked = true;
  while(clicked == true)
  {
    clicked = digitalRead(clickButton);
    delay(10);
  }
  delay(300);
}
//////////////////////////////////////////////////////////////////

///////////////////////// EVALUATE PING ////////////////////////////
String evalPing(int ping)
{
  String quality;
  if (ping == 0)
  {
    quality = "ERROR";
  }
  else
  {
    if (ping < 50)
    {
      quality = "GOOD";
    }
    else if ((ping >= 50) && (ping <= 100))
    {
      quality = "AVERAGE";
    }
    else
    {
      quality = "BAD";
    }
  }
  return quality;
}
