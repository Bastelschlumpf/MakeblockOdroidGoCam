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
  * @file DrawRollPitchYaw.h
  *
  * Demo code for artifical horizon display based on Bodmer for a 160 x 128 TFT display.
  * https://github.com/Bodmer/TFT_ST7735
  */

#include "ClipRect.h"

#define DEG2RAD 0.0174532925

#define BROWN     0x5140 
#define SKY_BLUE  0x02B5 
#define DARK_RED  0x8000
#define DARK_GREY 0x39C7


class DrawRollPitchYaw
{
protected:
   int xCenter;
   int yCenter;
   int xWidth;
   int yWidth;
   int horWidth;

   int last_roll;
   int last_pitch;
   int last_yaw;

   ClipRect clipRect;

protected:
   void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
   void drawHorizon(int roll, int pitch);
   void drawHorizonInfo(int roll, int pitch);
   void drawCompass(int centreX, int centreY, int radius, int angle, int lastAngle);   
   void drawCompassRose(int centreX, int centreY, int radius, int angle);
   void drawCompassArrow(int x2, int y2, int x1, int y1, int alength, int awidth, int colour);

public:
   DrawRollPitchYaw(int xCenterSet, int yCenterSet, int xWidthSet, int yWidthSet);

   void begin();
   void draw(int roll, int pitch, int yaw);
};

DrawRollPitchYaw::DrawRollPitchYaw(int xCenterSet, int yCenterSet, int xWidthSet, int yWidthSet)
   : xCenter(xCenterSet)
   , yCenter(yCenterSet)
   , xWidth(xWidthSet)
   , yWidth(yWidthSet)
   , horWidth(180)
   , last_roll(0)
   , last_pitch(0)
   , last_yaw(0)
   , clipRect(xCenterSet - xWidthSet, yCenterSet - yWidthSet, xCenterSet + xWidthSet, yCenterSet + yWidthSet)
{
}

void DrawRollPitchYaw::begin()
{
   GO.lcd.setTextSize(1); 
   GO.lcd.setTextColor(TFT_YELLOW, SKY_BLUE);
   GO.lcd.setTextDatum(TC_DATUM);
   
   GO.lcd.fillRect(xCenter - xWidth, yCenter - yWidth, 2 * xWidth, yWidth, SKY_BLUE);
   GO.lcd.fillRect(xCenter - xWidth, yCenter,          2 * xWidth, yWidth, BROWN);
}

void DrawRollPitchYaw::draw(int roll, int pitch, int yaw)
{
   int delta_pitch = 0;
   int pitch_error = 0;
   int delta_roll  = 0;
   int delta_yaw   = 0;

   GO.lcd.setTextSize(1); 
   GO.lcd.setTextColor(TFT_YELLOW, SKY_BLUE);
   GO.lcd.setTextDatum(TC_DATUM);
   
   while ((last_pitch != pitch) || (last_roll != roll)) {
      delta_pitch = 0;
      delta_roll  = 0;
      delta_yaw   = 0;
      
      if (last_pitch < pitch) {
         delta_pitch = 1;
         pitch_error = pitch - last_pitch;
      }
      
      if (last_pitch > pitch) {
         delta_pitch = -1;
         pitch_error = last_pitch - pitch;
      }

      if (last_roll < roll) delta_roll = 1;
      if (last_roll > roll) delta_roll = -1;
      
      if (delta_roll == 0) {
         if (pitch_error > 1) delta_pitch *= 2;
      }

      drawHorizon(last_roll + delta_roll, last_pitch + delta_pitch);
   }
   drawHorizonInfo(last_roll, last_pitch);
   
   drawCompass(xCenter, yCenter + 2 * yWidth + 30, yWidth - 12, yaw, last_yaw);
   last_yaw = yaw;
}

void DrawRollPitchYaw::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) 
{
   if (clipRect.GetClipRect(x0, y0, x1, y1)) {
      GO.lcd.drawLine(x0, y0, x1, y1, color);
   }
}

void DrawRollPitchYaw::drawHorizon(int roll, int pitch)
{
   // Calculate coordinates for line start
   float sx = cos(roll * DEG2RAD);
   float sy = sin(roll * DEG2RAD);
   
   int16_t x0  = sx * horWidth;
   int16_t y0  = sy * horWidth;
   int16_t xd  = 0;
   int16_t yd  = 1;
   int16_t xdn = 0;
   int16_t ydn = 0;

   if (roll > 45 && roll <  135) {
      xd = -1;
      yd =  0;
   }
   if (roll >=  135) {
      xd =  0;
      yd = -1;
   }
   if (roll < -45 && roll > -135) {
      xd =  1;
      yd =  0;
   }
   if (roll <= -135) {
      xd =  0;
      yd = -1;
   }

   if ((roll != last_roll) && ((abs(roll) > 35)  || (pitch != last_pitch))) {
      xdn = 4 * xd;
      ydn = 4 * yd;
      drawLine(xCenter - x0 - xdn, yCenter - y0 - ydn - pitch, xCenter + x0 - xdn, yCenter + y0 - ydn - pitch, SKY_BLUE);
      drawLine(xCenter - x0 + xdn, yCenter - y0 + ydn - pitch, xCenter + x0 + xdn, yCenter + y0 + ydn - pitch, BROWN);
      xdn = 3 * xd;
      ydn = 3 * yd;
      drawLine(xCenter - x0 - xdn, yCenter - y0 - ydn - pitch, xCenter + x0 - xdn, yCenter + y0 - ydn - pitch, SKY_BLUE);
      drawLine(xCenter - x0 + xdn, yCenter - y0 + ydn - pitch, xCenter + x0 + xdn, yCenter + y0 + ydn - pitch, BROWN);
   }
   xdn = 2 * xd;
   ydn = 2 * yd;
   drawLine(xCenter - x0 - xdn, yCenter - y0 - ydn - pitch, xCenter + x0 - xdn, yCenter + y0 - ydn - pitch, SKY_BLUE);
   drawLine(xCenter - x0 + xdn, yCenter - y0 + ydn - pitch, xCenter + x0 + xdn, yCenter + y0 + ydn - pitch, BROWN);
   
   drawLine(xCenter - x0 - xd, yCenter - y0 - yd - pitch, xCenter + x0 - xd, yCenter + y0 - yd - pitch, SKY_BLUE);
   drawLine(xCenter - x0 + xd, yCenter - y0 + yd - pitch, xCenter + x0 + xd, yCenter + y0 + yd - pitch, BROWN);
   
   drawLine(xCenter - x0, yCenter - y0 - pitch,   xCenter + x0, yCenter + y0 - pitch,   TFT_WHITE);
   
   last_roll  = roll;
   last_pitch = pitch;
}

void DrawRollPitchYaw::drawHorizonInfo(int roll, int pitch)
{
   // Level wings graphic
   GO.lcd.fillRect(xCenter - 1, yCenter - 1, 3, 3, TFT_RED);
   GO.lcd.drawFastHLine(xCenter - 30,      yCenter, 24, TFT_RED);
   GO.lcd.drawFastHLine(xCenter + 30 - 24, yCenter, 24, TFT_RED);
   GO.lcd.drawFastVLine(xCenter - 30 + 24, yCenter,  3, TFT_RED);
   GO.lcd.drawFastVLine(xCenter + 30 - 24, yCenter,  3, TFT_RED);
   
   // Pitch scale
   GO.lcd.drawFastHLine(xCenter - 12,   yCenter - 40, 24, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter -  6,   yCenter - 30, 12, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter - 12,   yCenter - 20, 24, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter -  6,   yCenter - 10, 12, TFT_WHITE);
   
   GO.lcd.drawFastHLine(xCenter -  6,   yCenter + 10, 12, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter - 12,   yCenter + 20, 24, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter -  6,   yCenter + 30, 12, TFT_WHITE);
   GO.lcd.drawFastHLine(xCenter - 12,   yCenter + 40, 24, TFT_WHITE);
   
   // Pitch scale values
   GO.lcd.setTextColor(TFT_WHITE);
   GO.lcd.setCursor(xCenter - 12 - 13, yCenter - 20 - 3);
   GO.lcd.print("10");
   GO.lcd.setCursor(xCenter + 12 + 1, yCenter - 20 - 3);
   GO.lcd.print("10");
   GO.lcd.setCursor(xCenter - 12 - 13, yCenter + 20 - 3);
   GO.lcd.print("10");
   GO.lcd.setCursor(xCenter + 12 + 1, yCenter + 20 - 3);
   GO.lcd.print("10");
   
   GO.lcd.setCursor(xCenter - 12 - 13, yCenter - 40 - 3);
   GO.lcd.print("20");
   GO.lcd.setCursor(xCenter + 12 + 1, yCenter - 40 - 3);
   GO.lcd.print("20");
   GO.lcd.setCursor(xCenter - 12 - 13, yCenter + 40 - 3);
   GO.lcd.print("20");
   GO.lcd.setCursor(xCenter + 12 + 1, yCenter + 40 - 3);
   GO.lcd.print("20");
   
   // Display justified roll and pitch value near bottom of screen
   GO.lcd.setTextColor(TFT_YELLOW, BROWN); // Text with background
   GO.lcd.setTextDatum(MC_DATUM);            // Centre middle justified
   GO.lcd.setTextPadding(24);                // Padding width to wipe previous number
   GO.lcd.drawNumber(roll,  xCenter - 12, yCenter + 62, 1);
   GO.lcd.drawNumber(pitch, xCenter + 12, yCenter + 62, 1);
   
   // Draw fixed text
   GO.lcd.setTextColor(TFT_YELLOW);
   GO.lcd.setTextDatum(TC_DATUM);            // Centre middle justified
   GO.lcd.drawString("Bodmer's AHI", xCenter, yCenter + 71, 1);
}

void DrawRollPitchYaw::drawCompassRose(int centreX, int centreY, int radius, int angle) 
{
   int dxo, dyo, dxi, dyi;
   
   GO.lcd.drawCircle(centreX,centreY,radius,WHITE);  // Draw compass circle
   for (float i = 0; i <360; i = i + 22.5) {
      dxo = radius * cos(i*3.14/180);
      dyo = radius * sin(i*3.14/180);
      dxi = dxo * 0.95;
      dyi = dyo * 0.95;
      GO.lcd.drawLine(dxi+centreX,dyi+centreY,dxo+centreX,dyo+centreY,WHITE);   
   }
   GO.lcd.setTextColor(TFT_YELLOW, BLACK); // Text with background
   GO.lcd.setTextDatum(MC_DATUM);            // Centre middle justified
   GO.lcd.setTextPadding(24);                // Padding width to wipe previous number
   GO.lcd.drawNumber(angle, centreX, centreY + 17, 1);
}

void DrawRollPitchYaw::drawCompassArrow(int x2, int y2, int x1, int y1, int alength, int awidth, int colour) 
{
   float distance;
   int   dx, dy, x2o,y2o,x3,y3,x4,y4,k;
   
   distance = sqrt(pow((x1 - x2),2) + pow((y1 - y2), 2));
   dx = x2 + (x1 - x2) * alength / distance;
   dy = y2 + (y1 - y2) * alength / distance;
   k = awidth / alength;
   x2o = x2 - dx;
   y2o = dy - y2;
   x3 = y2o * k + dx;
   y3 = x2o * k + dy;
   x4 = dx - y2o * k;
   y4 = dy - x2o * k;
   GO.lcd.drawLine(x1, y1, x2, y2, colour);
   GO.lcd.drawLine(x1, y1, dx, dy, colour);
   GO.lcd.drawLine(x3, y3, x4, y4, colour);
   GO.lcd.drawLine(x3, y3, x2, y2, colour);
   GO.lcd.drawLine(x2, y2, x4, y4, colour);
} 

void DrawRollPitchYaw::drawCompass(int centreX, int centreY, int radius, int angle, int lastAngle)
{
   drawCompassRose(centreX, centreY, radius, lastAngle);
   
   int dx      = (0.7*radius * cos((angle    -90)*3.14/180)) + centreX;
   int dy      = (0.7*radius * sin((angle    -90)*3.14/180)) + centreY;
   int last_dx = (0.7*radius * cos((lastAngle-90)*3.14/180)) + centreX;
   int last_dy = (0.7*radius * sin((lastAngle-90)*3.14/180)) + centreY;
   
   drawCompassArrow(last_dx, last_dy, centreX, centreY, 4, 4, BLACK); // Erase last arrow      
   drawCompassArrow(dx,      dy,      centreX, centreY, 4, 4, WHITE); // Draw new arrow
}
