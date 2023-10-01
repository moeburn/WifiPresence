/*
    Find My example for the Approximate Library
    -
    Report the signal strenth of a device (specificed by its MAC address)
    -
    David Chatting - github.com/davidchatting/Approximate
    MIT License - Copyright (c) October 2020

    Example documented here > https://github.com/davidchatting/Approximate/tree/master#find-my--using-an-active-device-handler
*/
#include <FastLED.h>
#include <Wire.h>   
#include <Approximate.h>
#include "SH1106Wire.h"   // legacy: #include "SH1106.h"


//#include <StreamLib.h>
#define NEOPIXEL_PIN 18  //d7
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];

Approximate approx;

 SH1106Wire display(0x3c, SDA, SCL);   

//Define for your board, not all have built-in LED:
#if defined(ESP8266)
  const int LED_PIN = 14;
#elif defined(ESP32)
  const int LED_PIN = 2;
#endif
bool ledState = LOW;
long ledToggleAtMs = 0;
int ledToggleIntervalMs = 0;

void setup() {
  display.init();
  display.setFont(ArialMT_Plain_16);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(leds, NUM_LEDS);
  if (approx.init("mikesnet", "springchicken")) {
    approx.setActiveDeviceFilter("64:89:F1:AF:94:85");
    approx.setActiveDeviceHandler(onActiveDevice);
    approx.begin();
  }
}

int lightsTime = 0;
int rssistrength;
CRGB color1 = CRGB( 255, 204, 102);

void loop() {
  approx.loop();
    display.clear();
    String rssistring = "RSSI: " + String(rssistrength);
    display.drawString(0, 0, rssistring);
    String timestring = "Time: " + String(lightsTime);
    display.drawString(0, 16, timestring);
    display.display();


  if (lightsTime > 0){
    lightsTime--;
    leds[0] = color1;
    if (rssistrength > -40) {leds[1] = color1;} else {leds[1] = CRGB(0,0,0);}
    if (rssistrength > -30) {leds[2] = color1;} else {leds[2] = CRGB(0,0,0);}
    if (rssistrength > -20) {leds[3] = color1;} else {leds[3] = CRGB(0,0,0);}
    if (rssistrength > -10) {leds[4] = color1;} else {leds[4] = CRGB(0,0,0);}
    if (rssistrength > -5) {leds[5] = color1;} else {leds[5] = CRGB(0,0,0);}
      //fill_solid(leds, NUM_LEDS, color1);
  FastLED.show();
  }

  if (lightsTime == 0){
      fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
  }
  
  if(ledToggleIntervalMs > 0 && millis() > ledToggleAtMs) {
    ledState = !ledState;
    ledToggleAtMs = millis() + ledToggleIntervalMs;
  }
}

void onActiveDevice(Device *device, Approximate::DeviceEvent event) {
  if(event == Approximate::SEND) {  
    
    ledToggleIntervalMs = map(device->getRSSI(), -100, 0, 1000, 0);
    Serial.println(device->getRSSI());
    rssistrength = device->getRSSI();
    if (rssistrength > -40){lightsTime = 3000;}
  }
}