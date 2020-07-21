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

//ledDisplay const
// the MAX7219 address map (datasheet table 2)
#define MAX7219_DECODE_REG      (0x09)
#define MAX7219_INTENSITY_REG   (0x09)
#define MAX7219_SCANLIMIT_REG   (0x0B)
#define MAX7219_SHUTDOWN_REG    (0X0C)
#define MAX7219_DISPLAYTEST_REG (0x0F)
#define MAX7219_DIGIT_REG(pos)  ((pos) + 1)
#define MAX7219_COLUMN_REG(pos) MAX7219_DIGIT_REG(pos)

// shutdown mode (datasheet table 3)
#define MAX7219_OFF             (0x0)
#define MAX7219_ON              (0x1)

// number of columns of the display matrx
#define NUM_OF_COLUMNS  (8)
// for each character bitmap, it consumes 4 bytes
#define BYTE_PER_MAP    (4)

// library interface description
class Decabot
{
	public:
		//constructor		
		Decabot(int delay, byte configuration);
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
		void formatROM();
		void primeDirectives();
		void setMaxBattery(int value);
		void setMinBattery(int value);
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
		void servo(int degrees);
		//buttons
		int readButton();
		void executeButton(int button);
		void loadLedsCheck();
		//Code Domino
		void update();
		void codeDomino(char code[]);
		void run();
		void run(int blockMemory);
		void runCodeDominoSetup();
		void checkCodeDominoLoops();
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
		void objectDetection(bool forced);
		void codeScanObjectPrecision(int range);
		//ledMatrix display functions
		void ledMatrixInit();
		void ledMatrixSetRegister(byte address, byte value);
		void ledMatrixClear();
		void ledMatrixRandom();
		void ledMatrixBattery(int value);
		void ledFaceClearMem();
		void ledFaceEyes(int position);
		void ledFaceEyebrows(int closed, int angry);
		void ledFaceMouth(int index);
		void printFace();
	private:
		byte decabotConfiguration = B00000000;
		int decabotMaxBattery = 5000;	//maximum battery value
		int decabotMinBattery = 4000;	//minimum battery value
		/*defines wich configuration the robot is set
		 1- Ultrasonic on 4,5
		 2- RFID on 9-13
		 3- Servo on A2
		 4- Led Matrix on 4,5,A2
		 5- Gyroscope on i2c
		 6- Laser distance sensor on i2c
		 7- Light sensors on A1,A3
		 8- PIR sensor on A2
		*/
		int latchPin = 8;	//Pin connected to ST_CP of 74HC595
		int clockPin = 7;	//Pin connected to SH_CP of 74HC595
		int dataPin = 6;	//Pin connected to DS of 74HC595
		int trigPin = 5;	//Pin connected to HC-SR04 ultrasonic
		int echoPin = 4;	//Pin connected to HC-SR04 ultrasonic
		int buzzerPin = 3;	//Pin connected to buzzer
		int ledPin = 2;		//Pin connected to frontal LED
		int servoPin = 16;	//Pin connected to servo
		int ledLatchPin = 16;	//Pin connected to Max7219 module CS
		int ledClockPin = 4;	//Pin connected to Max7219 module CLK
		int ledDataPin = 5;		//Pin connected to Max7219 module DIN
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
		int servoPosition = 90;	//variable who have the last servo position, for performance purposes
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
		int varA = 2;	//angry face
		int varB = 1;	//mouth
		int varC = 0;	//close eyebrows
		int varE = 5;	//eyeposition
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
		long readVcc(byte samples);
		int measureBattery();
		//ledMatrix variables
		byte ledFaceMem[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000}; //video ram memory for 8x8 display
		bool faceChanged = 0;
		const byte mouth[9][3] = {
			{B00000000,B00100100,B00011000}, //small smile
			{B01000010,B01100110,B00111100}, //smile
			{B01111110,B00100100,B00011000}, //open smile
			{B00000000,B00011000,B00011000}, //shut
			{B00011000,B00100100,B00011000}, //oh
			{B00111100,B01000010,B00111100}, //ah
			{B00000011,B00001110,B00111000}, //side mouth
			{B00000000,B01111110,B00000000}, //neutral
			{B00111100,B01100110,B01000010}  //sad
		};
		const byte eye[3][2] = {
			{B11001100,B11001100}, //right
			{B01100110,B01100110},
			{B00110011,B00110011}, //left
		};
		const byte battery[3] = {B00011000,B00111100,B00100100};
		const byte eyeBrow[3][3] = {
			{B10000001,B01000010,B00100100}, //angry
			{B00000000,B11100111,B00000000}, //normal
			{B00100100,B01000010,B10000001}, //sad
		};
		const byte eyeClear[3][3] = {
			{B00000000,B10000001,B11000011}, //angry
			{B00000000,B00000000,B11111111}, //normal
			{B00000000,B00111100,B01111110}, //sad
		};
		int decabotMusic[4][2]{
			{494,2},
			{554,2},
			{440,2},
			{880,1}
		};
};

#endif

