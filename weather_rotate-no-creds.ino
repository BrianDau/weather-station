// Instructables Internet of Things Class sample code
// Circuit Triggers Internet Action
// A button press is detected and stored in a feed
// An LED is used as confirmation feedback
//
// Modified by Becky Stern 2017
// based on the Adafruit IO Digital Input Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-digital-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    ""
#define IO_KEY         ""

/******************************* WIFI Configuration **************************************/

#define WIFI_SSID       ""
#define WIFI_PASS       ""

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/
#include <ESP8266WiFi.h>
#include <AdafruitIO.h>
#include <Adafruit_MQTT.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define BUTTON_PIN 4
#define LED_PIN 13


#define PIXELS_PIN 15
#define PIXEL_COUNT 4
#define BRIGHTNESS 255

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXELS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_7segment matrix = Adafruit_7segment();  // create segment display object

AdafruitIO_Feed *current = io.feed("currentWeather"); // set up the 'currentWeather' feed
AdafruitIO_Feed *forecast = io.feed("forecast"); // set up the 'forecast' feed

//the following strings store the varous IFTTT weather report words I've discovered so far
String rain = String("rain");
String lightrain = String("light rain");
String rainshower = String ("rain shower");
String showers = String ("showers");
String snow = String("snow");
String fog = String("fog");
String cloudy = String("cloudy");
String mostlycloudy = String("mostly cloudy");
String partlycloudy = String("partly cloudy");
String clearsky = String("clear");
String fair = String("fair");
String sunny = String("sunny");
String mostlysunny = String("mostly sunny");
String rainandsnow = String("rain and snow");
String snowshower = String("snow shower");
String pmstorms = String("pm thunderstorms");
String storms = String("thunderstorms");
String scatteredstorms = String("scattered thunderstorms");
String storm = String("thunderstorm");
String condition = "";
String highTemp = "";
String lowTemp = "";
  
void setup() {

  pinMode(LED_PIN, OUTPUT);
  matrix.begin(0x70); // initialize matrix display

  pixels.setBrightness(BRIGHTNESS);
  pixels.begin();
  lightPixels(pixels.Color(0, 0, 0));

  
  // start the serial connection
  Serial.begin(115200);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'currentWeather' feed.
  // the handleCurrent function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  current->onMessage(handleCurrent);

  // set up a message handler for the 'forecast' feed.
  // the handleForecast function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  forecast->onMessage(handleForecast);
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

}

// this function is called whenever a 'currentWeather' message
// is received from Adafruit IO. it was attached to
// the currentWeather feed in the setup() function above.
void handleCurrent(AdafruitIO_Data *data) {

  lightPixels(pixels.Color(0, 0, 0)); // reset all pixels to off
  
  String currWeather = data->toString(); // store the incoming weather data in a string

  // Parse the semicolon separated text
  condition = getValue(currWeather,';',0);
  highTemp = getValue(currWeather,';',1);

  // After parsing, call the function to display the current weather
  displayData();

}

// this function is called whenever a 'forecast' message
// is received from Adafruit IO. it was attached to
// the forecast feed in the setup() function above.
void handleForecast(AdafruitIO_Data *data) {
  lightPixels(pixels.Color(0, 0, 0)); // reset all pixels to off
  
  String forecastWeather = data->toString(); // store the incoming weather data in a string
  
  condition = getValue(forecastWeather,';',0);
  highTemp = getValue(forecastWeather,';',1);
  lowTemp = getValue(forecastWeather,';',2);

  displayData();
}

// Function that shows the most recent data
void displayData() {
  condition.trim();
  condition.toLowerCase();
  highTemp.trim();
  lowTemp.trim();
  // Show forecasted/current data
  displayHighTemp();
  displayCondition();
  
}

// display temperature
void displayHighTemp() {  
  int todaysHigh = highTemp.toInt(); // store the incoming temperature data as an integer
  matrix.print(todaysHigh, DEC);  // send the temperature value to the display
  matrix.writeDisplay();          // light up display
  delay(500);                     // wait half a second
} 

void displayCondition() {
    // These if statements compare the incoming weather variable to the stored conditions, and control the NeoPixels accordingly.
  Serial.println(condition);
  // if there's rain in the forecast, tell the the first four pixels to be blue and the middle four pixels to be white (but don't draw them yet)
  if (condition.indexOf(rain) != -1 || condition.indexOf(lightrain) != -1 || 
    condition.indexOf(rainshower) != -1 || condition.indexOf(showers) != -1 || 
    condition.equalsIgnoreCase(pmstorms) || condition.indexOf(storms) != -1 || condition.equalsIgnoreCase(scatteredstorms) || 
    condition.equalsIgnoreCase(storm)) {
    Serial.println("precipitation in the forecast today");
    pixels.setPixelColor(2, pixels.Color(0,30,200)); // rain (blue-ish)
  }
  
  // if there's snow in the forecast, tell the the first four pixels to be whiteish blue and the middle four pixels to be white (but don't draw them yet)
  if (condition.equalsIgnoreCase(snow) || condition.equalsIgnoreCase(rainandsnow) || condition.equalsIgnoreCase(snowshower)){
    Serial.println("precipitation in the forecast today");
    pixels.setPixelColor(3, pixels.Color(255,255,255)); // snow (white)
    if(condition.equalsIgnoreCase(rainandsnow)) {
      // add in the rain
      pixels.setPixelColor(2, pixels.Color(0,30,200)); // rain (blue-ish)
    }
  }
  
  // if there's sun in the forecast, tell the last four pixels to be yellow (but don't draw them yet)
  if (condition.equalsIgnoreCase(clearsky) || condition.equalsIgnoreCase(fair) || condition.equalsIgnoreCase(sunny)){
    Serial.println("some kind of sun in the forecast today");
     pixels.setPixelColor(0, pixels.Color(255,150,0)); // sunny (yellow)
  }
  // if there're clouds in the forecast, tell the the middle four pixels to white and the last four pixels to be yellow (but don't draw them yet)
  if (condition.equalsIgnoreCase(cloudy) || condition.equalsIgnoreCase(mostlycloudy) || condition.equalsIgnoreCase(partlycloudy) || condition.equalsIgnoreCase(fog)){
    Serial.println("cloudy sky in the forecast today");
    pixels.setPixelColor(1, pixels.Color(50,50,150)); // clouds (grey)
    if(condition.equalsIgnoreCase(partlycloudy)){
      // it's partly cloudy, so add a peek of sun.
     pixels.setPixelColor(0, pixels.Color(255,150,0)); // sunny (yellow)
    }
   }
   pixels.show(); // light up the pixels
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Function to set all the NeoPixels to the specified color.
void lightPixels(uint32_t color) {
  for (int i=0; i<PIXEL_COUNT; ++i) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}
