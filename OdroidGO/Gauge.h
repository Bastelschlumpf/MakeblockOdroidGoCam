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
  * @file Gauge.h
  *
  * helper class to draw a gauge like a voltmeter
  */


class Gauge
{
protected:
   double range;
   String gaugeLabel;
   String unitText;
   int    xCenter;
   int    yCenter;
   int    radius;
   int    tickLen;
   int    tickCount;
   int    pointerWidth;

protected:
   void erase();
   void drawHalfCircle();
   void drawTicks();
   void drawLabels();
   void drawPointer(double value);
   void drawGaugeLabel(double value);
   
public:
   Gauge(double rangeSet, const String &gaugeLabelSet, const String &unitTextSet, 
         int xCenterSet, int yCenterSet, int radiusSet, int tickLenSet = 5, int tickCountSet = 7, int posinterWidthSet = 2);

   void draw(double value);
};


Gauge::Gauge(double rangeSet, const String &gaugeLabelSet, const String &unitTextSet,
             int xCenterSet, int yCenterSet, int radiusSet, int tickLenSet /*= 10*/, int tickCountSet /*= 7*/, int pointerWidthSet /*= 2*/)
   : range(rangeSet)
   , gaugeLabel(gaugeLabelSet)
   , unitText(unitTextSet)
   , xCenter(xCenterSet)
   , yCenter(yCenterSet)
   , radius(radiusSet)
   , tickLen(tickLenSet)
   , tickCount(tickCountSet)
   , pointerWidth(pointerWidthSet)
{
}

void Gauge::erase()
{
   GO.lcd.fillRect(xCenter - radius, yCenter - radius, 2 * radius, 2 * radius, BLACK);
}

void Gauge::drawHalfCircle()
{
   GO.lcd.drawCircleHelper(xCenter, yCenter, radius, 3, WHITE);  
   GO.lcd.drawLine(xCenter - radius, yCenter, xCenter + radius, yCenter, WHITE);
}

void Gauge::drawTicks()
{
   for (double angle = 0.0; angle < 180.0; angle += (180.0 / tickCount)) {
      int x1 = xCenter - (radius - tickLen) * cos(angle * PI / 180.0);
      int y1 = yCenter - (radius - tickLen) * sin(angle * PI / 180.0);
      int x2 = xCenter - radius * cos(angle * PI / 180);
      int y2 = yCenter - radius * sin(angle * PI / 180);

      GO.lcd.drawLine(x1, y1, x2, y2, WHITE);
   }
}

void Gauge::drawLabels()
{
   double labelValue = 0;
   double labelStep  = range / tickCount;
   
   GO.lcd.setTextPadding(0);
   for (double angle = 0.0; angle < 180.0; angle += (180.0 / tickCount)) {
      int x1 = xCenter - (radius - 2.5 * tickLen) * cos(angle * PI / 180.0);
      int y1 = yCenter - (radius - 2.5 * tickLen) * sin(angle * PI / 180.0);
      int x2 = xCenter - radius * cos(angle * PI / 180);
      int y2 = yCenter - radius * sin(angle * PI / 180);

      if (angle != 0) {
         GO.lcd.drawString(String((int) labelValue), x1, y1);
      }
      labelValue += labelStep;
   }
}

void Gauge::drawPointer(double value)
{
   if (range > 0) {
      int x1 = xCenter - pointerWidth;
      int y1 = yCenter;
      int x3 = xCenter + pointerWidth;
      int y3 = yCenter;
      int x2 = xCenter - (radius - tickLen) * cos(value / range * PI);
      int y2 = yCenter - (radius - tickLen) * sin(value / range * PI);

      Serial.println("value: " + String(value, 2) + " range: " + String(range, 2) + "x2: " + String(x2) + " y2: " + String(y2));
   
      GO.lcd.fillTriangle(x1, y1, x2, y2, x3, y3, RED);
   }
}

void Gauge::drawGaugeLabel(double value)
{
   String label = gaugeLabel + " " + String(value, 2) + " " + unitText; 
   
   GO.lcd.drawString(label, xCenter, yCenter + 10, 1);
}

void Gauge::draw(double value)
{
   erase();
   drawHalfCircle();
   drawLabels();
   drawTicks();
   drawPointer(value);
   drawGaugeLabel(value);
}
