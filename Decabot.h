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

#define code_square "[square]X4FLYO"
#define code_scan "[scan]L45u5Ru2L45O"

// library interface description
class Decabot
{
	public:
		//constructor		
		Decabot(int nothing);
		~Decabot();
		//basics
		void boot();
		void inputSerial(char dataSerial);
		void runCodeOnSerial();
		void whoAmI();
		String sayMyName();
		void yourNameIs(String parameter);
		void yourOwnerIs(String parameter);
		void outputln(String message);
		void output(String message);
		void dumpMemory();
		void formatROM(String name);
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
		//buttons
		int readButton();
		void executeButton(int button);
		void loadLedsCheck();
		//Code Domino
		void update();
		void codeDomino(char code[]);
		void run();
		void run(int blockMemory);
		void nextCommand();
		char infiniteCode(int index);
		void codeInterpreter(char command, int parameter);
		void codeForward(int distance);
		void codeForward(int distance,int speed);
		void codeLeft(int degrees);
		void codeRight(int degrees);
		void codeSpeed(int speed);
		void codeGoTo(int piece);
		void codeRepeat(int times);
		void codeStopRepeat();
		void codeRunBlockMem(int block);
		void codeWait(int timeWait);
		void codeMusic(int toneFreq);
		void setRadious(float radious);
		void codeCurveLeft(int);
		void setPosition(float x, float y);
		void setHeading(float degrees);
		void incrementXYPosition(float distanceForward);
		void showPosition();
		void adjustHeading();
		void codeEnd();
		void unknowCode();
		void saveCodeROM(int memoryBlock);
		String programName(int memoryPosition);
		//RFID functions
		void rfidCodeRecord();
		void rfidCodeRecord(int blockMemory);
		//ultrasonic functions
		double measureDistance();
		void objectDetection();
		void codeScanObjectPrecision(int range);
	private:
		int latchPin = 8;	//Pin connected to ST_CP of 74HC595
		int clockPin = 7;	//Pin connected to SH_CP of 74HC595
		int dataPin = 6;	//Pin connected to DS of 74HC595
		int trigPin = 5;	//Pin connected to HC-SR04 ultrasonic
		int echoPin = 4;	//Pin connected to HC-SR04 ultrasonic
		int buzzerPin = 3;	//Pin connected to buzzer
		int ledPin = 2;		//Pin connected to frontal LED
		bool ledPinState = 0;
		String decabotName = "A01  ";
		String decabotOwner = "anybody@decabot.com";
		String tmpOutput = "";
		String inputSerialString;
		//movements variables
		int millisDelay = 4;
		int leftStep = 0;
		byte leftBin = B00000000;
		int rightStep = 0;
		byte rightBin = B00000000;
		int leftSpeed = 5;
		int rightSpeed = 5;
		bool leftDirection = 1;
		bool rightDirection = 1;
		bool turningLeft = 0;
		float curveRadious = 5;
		//self position variables
		float xPos = 0;
		float yPos = 0;
		float heading = 0;
		//Code Domino variables
		bool moving = 0;
		int stepsToMove = 0;
		bool recording = 0;
		bool executing = 0;
		int runningCodeIndex = 0;
		char runningCode[128];
		long actualMillis = 0;
		long lastLeftMillis = 0;
		long lastRightMillis = 0;
		long codeMillisBegin = 0;
		//Code Domino User variables
		int varX = 10;
		int varY = 90;
		int varZ = 4;
		float varN = 0;
		float varP = 0;
		float varQ = 0;
		//runtime repeat variables
		int repeatCalls = 0;
		int repeatPointers[64];
		int repeatCounters[64];
		//button variables
		long timerButton = 0;
		bool lastButtonState = 0;
		int lastButtonValue = 0;
		//Ultrasonic measure variables
		uint32_t ultrasonicTimer;
		double lastDetection = 0;
		int objectDetectionDelay = 500;
		int rotatingTotal = 0;
		//private moving functions
		void updateSteps();
		//private functions
		void debug();
		//private math
		int poten(int base, int expoent);
		float radian(float degree);
};

#endif

