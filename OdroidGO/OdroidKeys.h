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
  * @file OdroidKeys.h
  *
  * Pressed Odroid GO keys 
  */


/**
  * Class to save the pressed keys
  */
  
class OdroidKeys
{
public:
   int  joyX;
   int  joyY;
   bool btnA;
   bool btnB;
   bool btnMenu;
   bool btnVolume;
   bool btnSelect;
   bool btnStart;

public:
   OdroidKeys();

   void   reset();
   void   check();
   String getKeys();
};

OdroidKeys::OdroidKeys()
   : joyX(0)
   , joyY(0)
   , btnA(false)
   , btnB(false)
   , btnMenu(false)
   , btnVolume(false)
   , btnSelect(false)
   , btnStart(false)
{
}

/** Reest all saved key actions. */
void OdroidKeys::reset()
{
   joyX      = 0;
   joyY      = 0;
   btnA      = false;
   btnB      = false;
   btnMenu   = false;
   btnVolume = false;
   btnSelect = false;
   btnStart  = false;
}

/** Checks all the Odroid GO keys. */
void OdroidKeys::check()
{
   if (GO.JOY_X.isAxisPressed() == 1) joyX      = 1;
   if (GO.JOY_X.isAxisPressed() == 2) joyX      = 2;
   if (GO.JOY_Y.isAxisPressed() == 1) joyY      = 1;
   if (GO.JOY_Y.isAxisPressed() == 2) joyY      = 2;
   if (GO.BtnA.isPressed()      == 1) btnA      = true;
   if (GO.BtnB.isPressed()      == 1) btnB      = true;
   if (GO.BtnMenu.isPressed()   == 1) btnMenu   = true;
   if (GO.BtnVolume.isPressed() == 1) btnVolume = true;
   if (GO.BtnSelect.isPressed() == 1) btnSelect = true;
   if (GO.BtnStart.isPressed()  == 1) btnStart  = true;
}   

/** Return all the key commands. */
String OdroidKeys::getKeys()
{
   String keys;
   char   sep = '?';
   
   if (joyX == 2) {
      keys += sep;
      keys += "X";
      sep  = '&';
   } 
   if (joyX == 1) {
      keys += sep;
      keys += "x";
      sep  = '&';
   } 
   if (joyY == 2) {
      keys += sep;
      keys += "Y";
      sep  = '&';
   }
   if (joyY == 1) {
      keys += sep;
      keys += "y";
      sep  = '&';
   }
   if (btnA) {
      keys += sep;
      keys += "A";
      sep  = '&';
   }
   if (btnB) {
      keys += sep;
      keys += "B";
      sep  = '&';
   }
   if (btnMenu) {
      keys += sep;
      keys += "M";
      sep  = '&';
   }
   if (btnVolume) {
      keys += sep;
      keys += "V";
      sep  = '&';
   }
   if (btnSelect) {
      keys += sep;
      keys += "s";
      sep  = '&';
   }
   if (btnStart) {
      keys += sep;
      keys += "S";
      sep  = '&';
   }
   reset();
   return keys;
}   
