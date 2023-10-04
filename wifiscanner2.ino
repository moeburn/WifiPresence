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
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <BlynkSimpleEsp32.h>
char auth[] = "5D3OmcWGz1X1QeFZrl_HS0E6EgOaQA7y";  //BLYNK

#define LIGHT_TIMER 60 //seconds for lights to remain on after detecting wifi signal
#define RSSI_MIN -55 //minimum RSSI signal strength to turn lights on (negative values)

#define BLYNK_PRINT Serial // Defines the object that is used for printing
#define BLYNK_DEBUG // Optional, this enables more detailed prints

AsyncWebServer server(80);


//#include <StreamLib.h>
#define NEOPIXEL_PIN 18  //d7
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];

Approximate approx;
WiFiClient wifiClient;

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

int lightsTime = 0;
int rssistrength;
unsigned long timermillis, millisTemp;
CRGB color1 = CRGB(255, 100, 0);

void setup() {
  display.init();
  display.setFont(ArialMT_Plain_16);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(leds, NUM_LEDS);
  if (approx.init("mikesnet", "springchicken")) {
    approx.setActiveDeviceFilter("64:89:F1:AF:94:85");
    approx.setActiveDeviceHandler(onActiveDevice);
    approx.begin([]() {
  Blynk.config(auth, IPAddress(192, 168, 50, 197), 8080);
  
    });
  }

      int x; 
      while (WiFi.status() != WL_CONNECTED) {
    x+=3;
    if (x > 128){x=0;}
    
    display.drawString(x, 0, ".");
    display.display();
    delay(250);
  }
approx.connectWiFi();
Blynk.connect();
    Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am a wifiscanner.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
    

     //   approx.onceWifiStatus(WL_CONNECTED, [](String payload) {
 


  //  }, json);
timermillis = millis();

}

bool newdata = false;

void loop() {
  //delay(10);
  approx.loop();
    Blynk.run();
          if ((millis() - millisTemp >= 1000) && newdata)
      {
        millisTemp = millis();
        Blynk.virtualWrite(V1, rssistrength);
        newdata = false;
      }
    display.clear();
    String rssistring = "RSSI: " + String(rssistrength);
    display.drawString(0, 0, rssistring);

    

  if ((millis() - timermillis) < (LIGHT_TIMER * 1000)){
          fill_solid(leds, NUM_LEDS, color1);
  FastLED.show();
      String timestring = "Time: " + String((LIGHT_TIMER * 1000) - (millis() - timermillis));
    display.drawString(0, 16, timestring);
  }
  else {
          fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
  }


  /*if (lightsTime > 0){
    lightsTime--;
      fill_solid(leds, NUM_LEDS, color1);
  FastLED.show();
  }

  if (lightsTime == 0){
      fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();
  }*/
  
 /* if(ledToggleIntervalMs > 0 && millis() > ledToggleAtMs) {
    ledState = !ledState;
    ledToggleAtMs = millis() + ledToggleIntervalMs;
  }*/
  display.display();
}

void onActiveDevice(Device *device, Approximate::DeviceEvent event) {
  if(event == Approximate::SEND) {  
        String json = "{\"" + device->getMacAddressAsString() + "\":\"" + Approximate::toString(event) + "\"}";
    //ledToggleIntervalMs = map(device->getRSSI(), -100, 0, 1000, 0);
    Serial.println(device->getRSSI());
    rssistrength = device->getRSSI();
    if (rssistrength > RSSI_MIN){
    timermillis = millis();}
    newdata = true;
       
  /* approx.onceWifiStatus(WL_CONNECTED, [](String payload) {
      Blynk.connect();
    }, json);
    approx.connectWiFi();*/
  }
}
