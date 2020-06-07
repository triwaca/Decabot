/*
  Decabot.h
  Daniel Chagas
*/

// ensure this library description is only included once
#ifndef DECABOT_H
#define DECABOT_H

#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoUniqueID.h>

// library interface description
class Decabot
{
	public:
		Decabot(int nothing);
		~Decabot();
		void boot();
		void resetMotors();
		void updateMotors(byte data);
		void updateMotors();
		void whoami();
		void yourNameIs(String parameter);
		void yourOwnerIs(String parameter);
		void output(String message);
		void beep(int time);
		void soundBegin();
		void soundAfirmative();
		void soundEnd();
		void soundRecording();
		void oneLeft(int dir);
		void oneRight(int dir);
		void move(int distance, int leftDirection, int RightDirection);
		void forward(int centimeters);
		void left(int degrees);
		void right(int degrees);

	private:
		int latchPin = 8; //Pin connected to ST_CP of 74HC595
		int clockPin = 7; //Pin connected to SH_CP of 74HC595
		int dataPin = 6; //Pin connected to DS of 74HC595
		int buzzerPin = 3; //Pin connected to buzzer
		int ledPin = 2; //Pin connected to frontal LED
		char decabotName[5] = "A01  ";
		char decabotOwner[50] = "anybody@decabot.com";
		//movements variables
		int millisDelay = 4;
		int leftStep = 0;
		byte leftBin = B00000000;
		int rightStep = 0;
		byte rightBin = B00000000;
    
};

#endif

