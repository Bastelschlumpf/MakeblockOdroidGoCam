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
  * @file ClipRect.h
  *
  * C++ program to implement Cohen Sutherland algorithm for line clipping. 
  */

// 
class ClipRect
{
protected:
   // Defining region codes 
   const int INSIDE = 0; // 0000 
   const int LEFT   = 1; // 0001 
   const int RIGHT  = 2; // 0010 
   const int BOTTOM = 4; // 0100 
   const int TOP    = 8; // 1000 

   int xMin; // Clipping Rect
   int yMin;
   int xMax;
   int yMax;

protected:
   // Function to compute region code for a point(x, y) 
   int computeCode(double x, double y)
   {
      // initialized as being inside 
      int code = INSIDE;

      if (x < xMin) {        // to the left of rectangle 
         code |= LEFT;
      } else if (x > xMax) { // to the right of rectangle 
         code |= RIGHT;
      }
      if (y < yMin) {        // below the rectangle 
         code |= BOTTOM;
      } else if (y > yMax) { // above the rectangle 
         code |= TOP;
      }

      return code;
   }

public:
   ClipRect(int xMinSet, int yMinSet, int xMaxSet, int yMaxSet)
      : xMin(xMinSet)
      , yMin(yMinSet)
      , xMax(xMaxSet)
      , yMax(yMaxSet)
   {
   }

   // Implementing Cohen-Sutherland algorithm 
   // Clipping a line from P1 = (x2, y2) to P2 = (x2, y2) 
   bool GetClipRect(int &x1, int &y1, int &x2, int &y2)
   {
      // Compute region codes for P1, P2 
      int code1 = computeCode(x1, y1);
      int code2 = computeCode(x2, y2);

      // Initialize line as outside the rectangular window 
      bool accept = false;

      while (true) {
         if ((code1 == 0) && (code2 == 0)) {
            // If both endpoints lie within rectangle 
            accept = true;
            break;
         } else if (code1 & code2) {
            // If both endpoints are outside rectangle, 
            // in same region 
            break;
         } else {
            // Some segment of line lies within the 
            // rectangle 
            int code_out;
            double x = 0.0, y = 0.0;

            // At least one endpoint is outside the 
            // rectangle, pick it. 
            if (code1 != 0) {
               code_out = code1;
            } else {
               code_out = code2;
            }

            // Find intersection point; 
            // using formulas y = y1 + slope * (x - x1), 
            // x = x1 + (1 / slope) * (y - y1) 
            if (code_out & TOP) {
               // point is above the clip rectangle 
               x = x1 + (x2 - x1) * (yMax - y1) / (y2 - y1);
               y = yMax;
            } else if (code_out & BOTTOM) {
               // point is below the rectangle 
               x = x1 + (x2 - x1) * (yMin - y1) / (y2 - y1);
               y = yMin;
            } else if (code_out & RIGHT) {
               // point is to the right of rectangle 
               y = y1 + (y2 - y1) * (xMax - x1) / (x2 - x1);
               x = xMax;
            } else if (code_out & LEFT) {
               // point is to the left of rectangle 
               y = y1 + (y2 - y1) * (xMin - x1) / (x2 - x1);
               x = xMin;
            }

            // Now intersection point x,y is found 
            // We replace point outside rectangle 
            // by intersection point 
            if (code_out == code1) {
               x1 = (int) x;
               y1 = (int) y;
               code1 = computeCode(x1, y1);
            } else {
               x2 = (int) x;
               y2 = (int) y;
               code2 = computeCode(x2, y2);
            }
         }
      }
      // Line accepted or rejected
      return accept; 
   }
};
