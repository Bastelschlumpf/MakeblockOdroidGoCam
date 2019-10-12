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
  * @file PowerSensor.h
  *
  * Helper class to meassure the power sensor 
  */

#define POWER_PORT  A4

class PowerSensor
{
public:
   void setup()
   {
   }
   
   float getPower()
   {
      int16_t auriga_power = analogRead(POWER_PORT);
      float   power        = (auriga_power / 1024.0) * 15;
      return power; 
   } 
};
