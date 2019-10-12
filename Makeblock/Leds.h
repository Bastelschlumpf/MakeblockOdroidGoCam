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
  * @file Leds.h
  *
  * Leds helper class 
  */

#define LED_RING_PORT     44
#define AURIGARINGLEDNUM  12
#define RINGALLLEDS        0


class Leds
{
protected:
   MeRGBLed meRGBLed;

public:
   Leds()
      : meRGBLed(0, AURIGARINGLEDNUM)
   {
   }
   
   void setup()
   {
      meRGBLed.setpin(LED_RING_PORT);   
   }
   
   void play()
   {
      for (int i = 50; i <= 150; i+=50 ) {
         meRGBLed.setColor( RINGALLLEDS, i, 0, 0 ) ;
         meRGBLed.show();
         delay(100);
         
         meRGBLed.setColor( RINGALLLEDS, 0, i, 0 );
         meRGBLed.show();
         delay(100);
         
         meRGBLed.setColor( RINGALLLEDS, 0, 0, i );
         meRGBLed.show();
         delay(100);
      }
      
      meRGBLed.setColor( RINGALLLEDS, 0, 0, 0 );
      meRGBLed.show();
      delay(50);
      
      for (int i = 1; i <= AURIGARINGLEDNUM; i++ ) {
         meRGBLed.setColor( i, 40, 10, 40);
         meRGBLed.show();
         delay(50);
      }
      
      for (int i = 1; i <= AURIGARINGLEDNUM; i++ ) {
         meRGBLed.setColor( i, 0, 0, 0);
      }
      meRGBLed.show();
   }
};
