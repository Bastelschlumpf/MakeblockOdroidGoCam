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
  * @file RoverInfo.h
  *
  * Parsed received Rover information 
  */

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
   String getInfoPart(String info, String type);

public:
   RoverInfo();

   void parse(const String &info);
};

RoverInfo::RoverInfo()
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

String RoverInfo::getInfoPart(String info, String type)
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

void RoverInfo::parse(const String &info)
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
