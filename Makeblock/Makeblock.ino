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
  * @file Makeblock.ino
  *
  * Main source with setup() and loop() 
  */
  
#include <MeAuriga.h>
#include "Sound.h"
#include "Leds.h"
#include "Move.h"
#include "PowerSensor.h"
#include "GyroSensor.h"
#include "TempSensor.h"
#include "LightSensors.h"
#include "SoundSensor.h"


Move         move;
Leds         leds;
Sound        sound;
GyroSensor   gyroSensor;
TempSensor   tempSensor;
PowerSensor  powerSensor;
LightSensors lightSensors;
SoundSensor  soundSensor;

String lastRoverInfo;
String serialData;

void updateLastRoverInfo()
{
   String info;

   info.reserve(128);
   
   // Info sample
   // 21 X:[1.04] Y:[-0.34] Z:[0.33] T:[26.71] V:[8.60] LL:[976] LR:[977] S:[128]

   // '21' as cmd-identifier
   info += "21 ";
   
   // Gyro sensor
   info += "X:[" + String(gyroSensor.getAngleX()) + "] ";
   info += "Y:[" + String(gyroSensor.getAngleY()) + "] ";
   info += "Z:[" + String(gyroSensor.getAngleZ()) + "] ";

   // Temperature sensor
   info += "T:[" + String(tempSensor.getTemp()) + "] ";

   // Power supply sensor
   info += "V:[" + String(powerSensor.getPower()) + "] ";
   
   // Left and right light sensor
   info += "LL:[" + String(lightSensors.getLeft()) + "] ";
   info += "LR:[" + String(lightSensors.getRight()) + "] ";

   // Sound sensor
   info += "S:[" + String(soundSensor.getStrength()) + "] ";
   
   // Serial.println("<-" + info);
   lastRoverInfo = info;
}

void setup() 
{
   Serial.begin(115200);
   Serial2.begin(115200);

   Serial.println("starting...");

   leds.setup();
   sound.setup();
   move.setup();
   gyroSensor.setup();
   tempSensor.setup();
   powerSensor.setup();
   lightSensors.setup();
   soundSensor.setup();

   serialData.reserve(255);
}

void loop() 
{
   while (Serial2.available() > 0) {
      int readByte = Serial2.read();

      // Serial.println("["+ String((char) readByte)+"]");
      if (readByte != -1 && readByte != '\r') {
         if (readByte != '\n') {
            serialData += (char) readByte;
         } else { // == '\n'
            Serial.println("<- " + serialData);
            // All commands should start with "21"
            if (serialData.indexOf("21") == 0) {
               if (serialData.indexOf('Y') != -1) {
                  move.forward(100.0);
               } else if (serialData.indexOf('y') != -1) {
                  move.reverse(100.0);
               } else if (serialData.indexOf('X') != -1) {
                  move.twistDelta(10.0);
               } else if (serialData.indexOf('x') != -1) {
                  move.twistDelta(-10.0);
               } else {
                  move.stop();
               } 
               
               if (serialData.indexOf('A') != -1) {
                  leds.play();
               } 
               if (serialData.indexOf('B') != -1) {
                  sound.play();
               }
      
               // Send to the camera-module
               Serial.print("-> " + lastRoverInfo);
               // Sending with newline delimiter.
               Serial2.println(lastRoverInfo); 
            }
            serialData = "";           
         }
      }
   } 

/*  
   if (move.isMoving()) {
      move.sendLocation();
   } else {
      premo.goTo(path[pathIdx][0], path[pathIdx][1]);
      Serial.println("goalX: " + String(path[pathIdx][0]) + " goalY: " + String(path[pathIdx][1]));
      pathIdx++;
      if (pathIdx >= 4) {
         pathIdx = 0;
      }
   }
*/   

   move.loop();
   gyroSensor.loop();
   updateLastRoverInfo();
}
