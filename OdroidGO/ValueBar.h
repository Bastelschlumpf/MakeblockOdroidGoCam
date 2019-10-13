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
  * @file ValueBar.h
  *
  * helper class to draw a vertical value bar.
  */

class ValueBar
{
protected:
   double range;
   String label;
   int    x;
   int    y;
   int    width;
   int    height;

public:
   ValueBar(double rangeSet, const String &labelSet, int xSet, int ySet, int widthSet, int heightSet);

   void draw(double value);
};

ValueBar::ValueBar(double rangeSet, const String &labelSet, int xSet, int ySet, int widthSet, int heightSet)
   : range(rangeSet)
   , label(labelSet)
   , x(xSet)
   , y(ySet)
   , width(widthSet)
   , height(heightSet)
{
}

void ValueBar::draw(double value)
{
   int pos = value * height / range;

   // erase
   GO.lcd.fillRect(x, y - height, width, height, BLACK);
   GO.lcd.fillRect(x - 7, y, width + 14, 20, BLACK);

   // draw bar
   GO.lcd.fillRoundRect(x + 1, y - pos, width - 2, pos, 3, YELLOW);
   GO.lcd.drawRoundRect(x, y - height, width, height, 3, RED);

   // draw value
   GO.lcd.drawString(String((int) value), x + width / 2, y + height + 10);

   // draw label
   GO.lcd.setTextColor(RED);
   GO.lcd.drawString(label, x + width / 2, y - 7);
   GO.lcd.setTextColor(YELLOW);
}
