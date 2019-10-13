/*
   Copyright (C) 2019 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file OdroidGO.ino
  *
  * Main source with setup() and loop() 
  */

#include <odroid_go.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <JPEGDecoder.h>
#include "DrawRollPitchYaw.h"
#include "Gauge.h"
#include "ValueBar.h"

#include "Config.h"
#define USE_CONFIG_OVERRIDE //!< Switch to use ConfigOverride
#ifdef USE_CONFIG_OVERRIDE
  #include "ConfigOverride.h"
#endif


DrawRollPitchYaw drawRollPitchYaw(280, 50, 40, 50);
Gauge            voltmeter  (12.0, "Power", "V",     72, 225, 40);
Gauge            temperature(40.0, "Temp",  "Grad", 168, 225, 40);
ValueBar         leftLight  (1024, "L",      10, 227, 10, 42);
ValueBar         rightLight (1024, "R",     220, 227, 10, 42);

const int picBufferSize = 20*1024; // Origin 80*1024
void     *picBuffer     = NULL;
int       picLen        = 0;

bool   getJpg();
String getActionKeys();
String sendAction(String keys);


/**
  * Class to parse the information received from the Rover cam.
  */
class RoverInfo
{
public:
   int    roll;
   int    pitch;
   int    yaw;
   double temp;
   double volt;
   int    leftLight;
   int    rightLight;
   int    soundLevel;
   bool   infoMode;
   bool   lastInfoMode;

protected:
   String getInfoPart(String info, String type)
   {
      int idxStart = info.indexOf(type + ":[");
   
      if (idxStart != -1) {
         int idxEnd = info.indexOf(']', idxStart + type.length() + 2);
   
         if (idxEnd != -1) {
            return info.substring(idxStart + type.length() + 2, idxEnd);
         }
      }
      return "";
   }

public:
   RoverInfo()
      : roll(0)
      , pitch(0)
      , yaw(0)
      , temp(0.0)
      , volt(0.0)
      , leftLight(0)
      , rightLight(0)
      , soundLevel(0)
      , infoMode(true)
      , lastInfoMode(true)
   {
   }

   void parse(const String &info)
   {
      // X:[1.04] Y:[-0.34] Z:[0.33] T:[26.71] V:[8.60] LL:[976] LR:[977] S:[128] IM:[1]
      String X  = getInfoPart(info, "X");
      String Y  = getInfoPart(info, "Y");
      String Z  = getInfoPart(info, "Z");
      String T  = getInfoPart(info, "T");
      String V  = getInfoPart(info, "V");
      String LL = getInfoPart(info, "LL");
      String LR = getInfoPart(info, "LR");
      String S  = getInfoPart(info, "S");
      String IM = getInfoPart(info, "IM");

      lastInfoMode = infoMode;
   
      pitch      =  X.toInt();
      roll       = -Y.toInt();
      yaw        =  Z.toInt();
      temp       =  T.toDouble();
      volt       =  V.toDouble();
      leftLight  =  LL.toInt();
      rightLight =  LR.toInt();
      soundLevel =  S.toInt();
      infoMode   =  IM != "0";
   }
};

RoverInfo roverInfo;

/** Gets one jpg image from the ESP32-CAM. */
bool getJpg()
{
   HTTPClient http;
   bool       ret = false;

   http.setTimeout(500);
   
   // Serial.print("[HTTP] begin...\n");
   http.begin(CAPTURE_URL);
   
   // Serial.print("[HTTP] GET...\n");
   int httpCode = http.GET();
   
   // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
   // HTTP header has been send and Server response header has been handled
   if (httpCode <= 0) {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
   } else {
      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("[HTTP] Not OK!\n");
      } else {
         // get lenght of document (is -1 when Server sends no Content-Length header)
         picLen = http.getSize();
         // Serial.printf("[HTTP] size: %d\n", picLen);
      
         if (picLen <= 0) {
            Serial.printf("[HTTP] Unknow content size: %d\n", picLen);
         } else if (picLen >= picBufferSize) {
            Serial.printf("PicBuffer too small %d <-> %d\n", picLen, picBufferSize);
         } else {
            int         l      = picLen;
            uint8_t    *p      = (uint8_t*) picBuffer;
            WiFiClient *stream = http.getStreamPtr();
            
            while (http.connected() && (l > 0 || picLen == -1)) {
               size_t size = stream->available();
               
               if (size) {
                  int c = stream->readBytes(p, size);
                  p += size;
                  // Serial.printf("[HTTP] read: %d\n", c);
               } else {
                  break;
               }
            }
            // Serial.println();
            ret = true;
         }
      }
   }
   http.end();
   // Serial.print("[HTTP] connection closed.\n");
   
   return ret;
}

/** Checks all the Odroid GO keys. */
String getActionKeys()
{
   String keys;
   char   sep = '?';
   
   if (GO.JOY_X.isAxisPressed() == 2) {
      keys += sep;
      keys += "X";
      sep  = '&';
   } 
   if (GO.JOY_X.isAxisPressed() == 1) {
      keys += sep;
      keys += "x";
      sep  = '&';
   } 
   if (GO.JOY_Y.isAxisPressed() == 2) {
      keys += sep;
      keys += "Y";
      sep  = '&';
   }
   if (GO.JOY_Y.isAxisPressed() == 1) {
      keys += sep;
      keys += "y";
      sep  = '&';
   }
   if (GO.BtnA.isPressed() == 1) {
      keys += sep;
      keys += "A";
      sep  = '&';
   }
   if (GO.BtnB.isPressed() == 1) {
      keys += sep;
      keys += "B";
      sep  = '&';
   }
   if (GO.BtnMenu.isPressed() == 1) {
      keys += sep;
      keys += "M";
      sep  = '&';
   }
   if (GO.BtnVolume.isPressed() == 1) {
      keys += sep;
      keys += "V";
      sep  = '&';
   }
   if (GO.BtnSelect.isPressed() == 1) {
      keys += sep;
      keys += "s";
      sep  = '&';
   }
   if (GO.BtnStart.isPressed() == 1) {
      keys += sep;
      keys += "S";
      sep  = '&';
   }
   return keys;
}   

/** Send action commands to the ESP32-CAM module. */
String sendAction(String keys)
{
   HTTPClient http;
   String     ret;

   http.setTimeout(500);
   
   // Serial.print("[HTTP] begin...\n");
   http.begin(ACTION_URL + keys);
   
   // Serial.print("[HTTP] GET...\n");
   int httpCode = http.GET();

   // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
   // HTTP header has been send and Server response header has been handled
   if (httpCode <= 0) {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
   } else {
      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("[HTTP] Not OK!\n");
      } else {
         ret = http.getString();
         // Serial.printf("[HTTP] result: %s\n", ret);
      }
   }
   
   http.end();
   // Serial.print("[HTTP] connection closed.\n");
   
   return ret;
}

/** Demo mode for testing. */
void demo()
{
   drawRollPitchYaw.draw((rand() % 50) - 25, (rand() % 40) - 20, 360 - (rand() % 180));
   voltmeter.draw(rand() % 12); 
   temperature.draw(rand() % 30); 
   leftLight.draw(rand() % 1024);
   rightLight.draw(rand() % 1024);
   GO.update(); 
   yield();
   delay(500);
}

/** Main setup function. */
void setup()
{ 
   Serial.begin(115200);

   GO.begin();
   GO.lcd.println("Hello, ODROID-GO");
   delay(1000);

   drawRollPitchYaw.begin();

   WiFi.begin(WIFI_SID, WIFI_PW);

   picBuffer = malloc(picBufferSize);        // Stream-File-Buffer
   Serial.printf("PicBuffer: %x\r\n", picBuffer);

   Serial.println("Total Flash Memory: " + String(ESP.getFlashChipSize())     + " Byte");
   Serial.println("Used Flash Memory: "  + String(ESP.getSketchSize())        + " Byte");
   Serial.println("Free Sketch Memory: " + String(ESP.getFreeSketchSpace())   + " Byte");
   Serial.println("Free Heap Memory: "   + String(ESP.getFreeHeap())          + " Byte");
}

/** Main loop function. */
void loop()
{
//   demo();
//   return;
   
   if (WiFi.status() != WL_CONNECTED) {
      GO.lcd.drawString("WiFi not connected", 10, 10);
      Serial.println("No Wifi");
      delay(50);
   } else {
      String keys = getActionKeys();
      String info = sendAction(keys);

      Serial.println("keys -> info: " + keys + " -> " + info);
      roverInfo.parse(info);
      
      if (!getJpg()) {
         GO.lcd.drawString("no jpg data", 10, 10);
         Serial.println("No Jpg");
      } else {
         if (roverInfo.infoMode != roverInfo.lastInfoMode) {
            GO.lcd.clearDisplay();
            drawRollPitchYaw.begin();
         }
         GO.lcd.drawJpg((const uint8_t *) picBuffer, picLen); 
         if (roverInfo.infoMode) {
            drawRollPitchYaw.draw(roverInfo.roll, roverInfo.pitch, roverInfo.yaw);
         }
      }
      // FRAMESIZE_HQVGA,    // 240x176 
      voltmeter.draw(roverInfo.volt); 
      temperature.draw(roverInfo.temp); 
      leftLight.draw(roverInfo.leftLight);
      rightLight.draw(roverInfo.rightLight);
   }  

   GO.update(); 
   yield();
   delay(10);
}
