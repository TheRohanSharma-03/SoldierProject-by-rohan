/*
************************************SUCCESSFUL***************************************************
*/
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include<ESP8266HTTPClient.h>
#include <WiFiClient.h> 

#define REPORTING_PERIOD_MS     500

float BPM, SpO2;

/*Put your SSID & Password*/
const char* ssid = "jerry1";  // Enter SSID here
const char* password = "jerry123";  //Enter Password here

WiFiClient wificlient;

PulseOximeter pox;
uint32_t tsLastReport = 0;


void onBeatDetected()
{
  Serial.println("Beat Detected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  
  
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection

}
void loop() {
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  HTTPClient http;
    
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    String serverName = "http://jerrygps.000webhostapp.com//bpm.php?bpm="+String(BPM)+"&oxy="+String(SpO2)+"&temp=56700";
    http.begin(wificlient,serverName);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    // String httpRequestData = "";        
    int httpcode = http.GET();
    if(httpcode > 0){
      String payload = http.getString();
      Serial.println(httpcode);
      Serial.println(payload);
    }           
    else{
      Serial.println("Error");
    }
    http.end();    
    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");

    Serial.println("*********************************");
    Serial.println();

    tsLastReport = millis();
  }

}

