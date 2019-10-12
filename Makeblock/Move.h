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
  * @file Move.h
  *
  * Auriga move helper class 
  */

#include <PreMo.h>

unsigned char table[128] = {0}; 

// ROBOT MEASUREMENTS]
const float XORADIUS = 15; // wheel radius in mm
const float XOLENGTH = 148; // wheel base length in mm

// PULSES PER REV
const int PULSES_PER_REV = 360; // number of pulses per revolution of the wheel.

// MAXIMUM MOTOR SPEED VALUE
const int MOTOR_SPEED_MAX = 255;

// INTERVALS
// MIN_PULSE_INTERVAL is the minimum time in microseconds before registering each encoder pulse.
// This is to compensate for "bouncing" on certain encoders.
const unsigned long MIN_PULSE_INTERVAL = 175; 

// SERIAL COMMUNICATION BAUD RATE
// PID TUNING
// PID for path following (for turning when followiung path)
const double KP = 50;
const double KD = 1;
// PID for motors (for twist method)
const double KP_motor = 1.5;
const double KI_motor = 0;

// INTERVALS
// When following path, send the location to app at this interval in millisecond.
const unsigned long SEND_INTERVAL = 200;

// ENCODER PULSES
unsigned long leftPulses;
unsigned long rightPulses;

// TIMING VARIABLES
unsigned long prevSendTime;
unsigned long prevRightTime;
unsigned long prevLeftTime;

// PATH FOLLOWING SPEED
// Target speed of path following in percentage.
const int PATH_FOLLOW_SPEED = 100;

// MOTORS
MeEncoderOnBoard leftMotor(SLOT2);
MeEncoderOnBoard rightMotor(SLOT1);

int iLeftPulse = 0;
int iRightPulse = 0;

void pulseLeft() 
{
   if (micros() - prevLeftTime > MIN_PULSE_INTERVAL) {
      leftPulses++;
      prevLeftTime = micros();
  } 
  if (digitalRead(leftMotor.getPortB()) == 0) {
      leftMotor.pulsePosMinus();
   } else {
      leftMotor.pulsePosPlus();;
   }

  /*
   if (digitalRead(leftMotor.getPortB()) == 1) {
      leftPulses++;
   } else {
      leftPulses--;
   }
   */
   //Serial.println("LeftPulse: " + String(leftPulses));
}

void pulseRight() 
{
   if (micros() - prevRightTime > MIN_PULSE_INTERVAL) {
      rightPulses++;
      prevRightTime = micros();
  } 
  if (digitalRead(rightMotor.getPortB()) == 0) {
      rightMotor.pulsePosMinus();
   } else {
      rightMotor.pulsePosPlus();;
   }
  /*
   if (digitalRead(rightMotor.getPortB()) == 0) {
      rightPulses++;
   } else {
      rightPulses--;
   }
   */
   //Serial.println("RightPulse: " + String(rightPulses));
}

// MOTOR SPEED FUNCTIONS
void setLeftForward(int speed) 
{
   leftMotor.runSpeed(speed);
   //Serial.println("setleftForward: " + String(speed));
}

void setLeftReverse(int speed) 
{
   leftMotor.runSpeed(-speed);
   //Serial.println("setLeftReverse: " + String(speed));
}

void setRightForward(int speed) 
{
   rightMotor.runSpeed(-speed);
   //Serial.println("setRightForward: " + String(speed));
}

void setRightReverse(int speed) 
{
   rightMotor.runSpeed(speed);
   //Serial.println("setRightReverse: " + String(speed));
}

void stopMotors() 
{
   leftMotor.runSpeed(0);
   rightMotor.runSpeed(0);
   // Serial.println("stopMotors");
}

void attachInterrupts() 
{
   attachInterrupt(leftMotor.getIntNum(), pulseLeft, RISING);
   attachInterrupt(rightMotor.getIntNum(), pulseRight, RISING);
}

void detachInterrupts() 
{
   detachInterrupt(leftMotor.getIntNum());
   detachInterrupt(rightMotor.getIntNum());
}

// PATH FOLLOWER
MotorManager motorManager(setLeftForward, setLeftReverse, setRightForward, setRightReverse, stopMotors);
EncoderManager encoderManager(&leftPulses, &rightPulses, PULSES_PER_REV);
PreMo premo(XORADIUS, XOLENGTH, KP, KD, KP_motor, KI_motor, &motorManager, &encoderManager);


int pathIdx = 0;

double path[][2] = { 
   {  0.0,   0.0},
   {  0.0, 100.0},
   {100.0, 100.0},
   {100.0,   0.0} 
};

bool once = true;

int step = 0;

void doSteps()
{
   if (step == 0) {
      Serial.println("forward");
      premo.forward(100.0);
   } else {
      Serial.println("twistDelta");
      premo.twistDelta(90.0);
   }
   step++;
   if (step >= 2) {
      step = 0;
   }
}


class Move
{
protected:

public:
   Move()
   {
   }
   
   void setup()
   {
      //Set PWM 8KHz
      TCCR1A = _BV(WGM10);
      TCCR1B = _BV(CS11) | _BV(WGM12);
      TCCR2A = _BV(WGM21) | _BV(WGM20);
      TCCR2B = _BV(CS21); 
      
      leftMotor.setPulse(9);
      rightMotor.setPulse(9);
      leftMotor.setRatio(39.267);
      rightMotor.setRatio(39.267);
      leftMotor.setPosPid(1.8,0,1.2);
      rightMotor.setPosPid(1.8,0,1.2);
      leftMotor.setSpeedPid(0.18,0,0);
      rightMotor.setSpeedPid(0.18,0,0);  
      leftMotor.setMotionMode(DIRECT_MODE);
      rightMotor.setMotionMode(DIRECT_MODE);    
     
      motorManager.setSpeedLimits(100, MOTOR_SPEED_MAX);
      premo.twistBothMotors(false);
      premo.setPathFollowSpeed(PATH_FOLLOW_SPEED);
      
      attachInterrupts();
   }

   void loop()
   {
      premo.loop();
      leftMotor.loop();
      rightMotor.loop();
   }

   bool isMoving()
   {
      return (premo.isFollowingPath() || premo.isTwisting());
   }

   void forward(float distance)
   {
      premo.forward(distance);
   }
   
   void reverse(float distance)
   {
      premo.reverse(distance);
   }
   
   void twistDelta(float angle)
   {
      premo.twistDelta(angle);
   }
   
   void stop()
   {
      premo.stop();
   }

   void sendLocation() 
   {
      if (millis() - prevSendTime > SEND_INTERVAL) {
         // Todo sending location should be less code for user.
         float xPos = premo.getX();
         float yPos = premo.getY();
         float heading = premo.getHeading();
         float xGoal = premo.getGoalX();
         float yGoal = premo.getGoalY();
         Serial.println("xPos: " + String(xPos) + " yPos: " + String(yPos) + " heading: " + String(heading) + " xGoal: " + String(xGoal) + " yGoal: " + String(yGoal));
         prevSendTime = millis();
      }
   }
};
