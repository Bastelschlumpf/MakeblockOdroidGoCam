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
  * @file LightSensor.h
  *
  * Light sensor helper class 
  */

class LightSensors
{
protected:
   MeLightSensor left;
   MeLightSensor right;

public:
   LightSensors()
      : left(PORT_12)
      , right(PORT_11)
   {
   }
   
   void setup()
   {
   }
   
   int16_t getLeft()
   {
      return left.read();
   }

   int16_t getRight()
   {
      return right.read();
   }
};
