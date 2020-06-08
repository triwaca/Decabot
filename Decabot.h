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
		//constructor		
		Decabot(int nothing);
		~Decabot();
		//basics
		void boot();
		void whoAmI();
		void yourNameIs(String parameter);
		void yourOwnerIs(String parameter);
		void output(String message);
		//sounds
		void beep(int time);
		void soundBegin();
		void soundBoot();
		void soundEnd();
		void soundRecording();
		//basic movements
		void resetMotors();
		void updateMotors(byte data);
		void updateMotors();
		void oneStepLeft(int dir);
		void oneStepRight(int dir);
		void move(int distance, int leftDirection, int RightDirection);
		void forward(int centimeters);
		void left(int degrees);
		void right(int degrees);
		//Code Domino
		void update();
		void codeDomino(char code[]);
		void run();
		void nextCommand();
		void codeForward(int distance);
		void codeEnd();
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
		//Code Domino variables
		bool moving = 0;
		int stepsToMove = 0;
		bool recording = 0;
		bool executing = 0;
		int runningCodeIndex = 0;
		char runningCode[128];
		long actualMillis = 0;
		long lastMillis = 0;
		//private moving functions
		void updateSteps();
		void debug();
		
    
};

#endif

