/*
  Decabot.h
  Daniel Chagas
*/


#include"Decabot.h"

Decabot::Decabot(int delay, byte configuration){
	decabotConfiguration = configuration;
	//define pin modes for shift register, buzzer and LED
	millisDelay = delay;
	pinMode(buzzerPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	if(bitRead(decabotConfiguration,3)){
		//configure a 8 x 8 led display on 2,4,5
  		pinMode(ledLatchPin, OUTPUT); 
		pinMode(ledClockPin, OUTPUT);   
		pinMode(ledDataPin, OUTPUT);
		bitWrite(decabotConfiguration,0,0); //turn off ultrasonic configuration
		bitWrite(decabotConfiguration,2,0); //turn off servo configuration
	} else {
		if(bitRead(decabotConfiguration,0)){
			//configure a ultrasonic sensor on 4,5
			pinMode(trigPin, OUTPUT);
			pinMode(echoPin, INPUT);
			digitalWrite(trigPin, LOW);
		}
		if(bitRead(decabotConfiguration,2)){
			//configure a servo on pin 16
			pinMode(servoPin, OUTPUT);
		}
	}
	
}

Decabot::~Decabot() 
{
	//define pin modes for shift register, buzzer and LED
	pinMode(buzzerPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	digitalWrite(trigPin, LOW);
}

void Decabot::boot(){
	//Boot sequence
	Serial.println(F(" - - - - - - - - - - - - - - - - - - - - - - - - - - D E C A N O"));
	outputln(F("Initializing Decabot..."));
	if(bitRead(decabotConfiguration,3)){
		//configure a 8 x 8 led display on CLK 4, DIN 5, CS 16(A2)
		outputln(F("Led Matrix MAX7219 on CLK 4, DIN 5, CS 16(A2)"));
		// init MAX2719 states
		ledMatrixInit();
		ledFaceEyes(5);
		printFace();
	} else {
		if(bitRead(decabotConfiguration,0)){
			//configure a ultrasonic sensor on 4,5
			outputln(F("Ultrasonic sensor HC-SR04 on ECHO 4, TRIG 5"));
		}
		if(bitRead(decabotConfiguration,2)){
			//configure a servo on pin 16
			outputln(F("Servo motor S on 16(A2)"));
		}
	}
	outputln(F("Shift register 74CH595 on LATCH 8, CLK 7, DATA 6"));
	outputln(F("Step Motor on BAY1, BAY2"));
	resetMotors();
	whoAmI();
	soundBoot();
	outputln(F("READY!"));
	runCodeDominoSetup();
}

void Decabot::runCodeDominoSetup(){
	if(infiniteCode(902)=='[') {
		outputln(F("found CodeDomino on setup..."));
		//outputln(programName(774));
		run(10);
	}
}

void Decabot::inputSerial(char dataSerial) {
	//acumulates the serial data on a variable, and executes it when receive the end of line
	inputSerialString += dataSerial;
	if (dataSerial == '\n') {
		runCodeOnSerial();
		inputSerialString = "";
	}
}

void Decabot::runCodeOnSerial(){
	//receive Code Domino data thru serial and run
	int serialCodeSize = inputSerialString.length() + 1;
	char inputSerialChar[serialCodeSize]; 
	inputSerialString.toCharArray(inputSerialChar, serialCodeSize);	//convert string to char
	if((inputSerialString[0]=='[')or(inputSerialString[0]=='S')){		//checkif it is a code Domino
		codeDomino(inputSerialChar);
	} else {
		//tun the command imediatelly		
		codeInterpreter(inputSerialString[0], 0);
	}
}

int Decabot::readButton() {
	int buttonValue = analogRead(A0);
	int pressedButton = 0;
	if((buttonValue > 200)&&(timerButton + 1000 < millis())){
		tone(buzzerPin, 440, 200);	//beep on hold
	}
	if((buttonValue > 200)&&!lastButtonState){
		beep(100);
		lastButtonState = 1;
		lastButtonValue = buttonValue;
		timerButton = millis();
		digitalWrite(ledPin, 1);
	}
	if((buttonValue <= 200)&&lastButtonState){
		noTone(buzzerPin);
		lastButtonState = 0;
		digitalWrite(ledPin, 0);
		long actualTimer = millis();
		if(lastButtonValue<709) {
			pressedButton = 6;
		} else if(lastButtonValue<761){
			pressedButton = 5;
		} else if(lastButtonValue<822){
			pressedButton = 4;
		} else if(lastButtonValue<894){
			pressedButton = 3;
		} else if(lastButtonValue<977){
			pressedButton = 2;
		} else {
			pressedButton = 1;
		}
		if(timerButton + 1000 < actualTimer){	//long press detected increases return number by 6
			pressedButton += 6;
			//Serial.println(timerButton);
			//Serial.println(lastButtonValue);
		}
		String msg = F("button ");
		msg.concat(pressedButton);
		msg.concat(F(" pressed!"));
		outputln(msg);
		executeButton(pressedButton);
	}
}

void Decabot::executeButton(int button){
	switch(button){
		case 1:		//play button
			run();
			break;
		case 2:		//memory blocks 1 to 4
			run(0);
			break;
		case 3:		//memory blocks 1 to 4
			run(1);
			break;
		case 4:		//memory blocks 1 to 4
			run(2);
			break;
		case 5:		//memory blocks 1 to 4
			run(3);
			break;
		case 6:		//memory blocks 1 to 4
			if(executing) {
				codeEnd();
			} else {
				rfidCodeRecord();
			}
			break;
		case 7:
			dumpMemory();
			break;
		case 8:
			rfidCodeRecord(0);
			break;
		case 9:
			rfidCodeRecord(0);
			break;
		case 10:
			rfidCodeRecord(0);
			break;
		case 11:
			rfidCodeRecord(0);
			break;
		case 12:
			formatROM();
			break;
	}
}

void Decabot::rfidCodeRecord() {
	outputln("RFID not implemented yet!");
}

void Decabot::rfidCodeRecord(int blockMemory) {
	outputln("RFID not implemented yet!");
}

void Decabot::resetMotors() {
	updateMotors(B00000000);
	outputln(F("Reset motors"));
}

void Decabot::updateMotors(byte data){
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, data); //Send all off to 74HC595
	digitalWrite(latchPin, HIGH);
}

void Decabot::updateMotors(){
      switch (leftStep) {
        case 1:
          leftBin = B10010000;
          break;
        case 2:
          leftBin = B00110000;
          break;
        case 3:
          leftBin = B01100000;
          break;
        case 4:
          leftBin = B11000000;
          break;
      }
      switch (rightStep) {
        case 1:
          rightBin = B00001001;
          break;
        case 2:
          rightBin = B00000011;
          break;
        case 3:
          rightBin = B00000110;
          break;
        case 4:
          rightBin = B00001100;
          break;
      }
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, leftBin | rightBin); //Send all off to 74HC595
      digitalWrite(latchPin, HIGH);
}

void Decabot::whoAmI() {
	//Loads from EEPROM and outputs tag name and owner e-mail of Decabot
	Serial.print(F("\t"));
	UniqueIDdump(Serial);
	//load form EEPROM and put in RAM
	decabotName = "";
	for(int i=902;i<=906;i++){
		if((char) EEPROM.read(i) == '[') break;
		decabotName.concat((char) EEPROM.read(i));		
	}
	decabotOwner = "";
	for(int i=917;i<960;i++){
		if((char) EEPROM.read(i) == '[') break;
		decabotOwner.concat((char) EEPROM.read(i));
	}
	tmpOutput = F("[name][");
	tmpOutput.concat(decabotName);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
	tmpOutput = F("[owner][");
	tmpOutput.concat(decabotOwner);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
}

String Decabot::sayMyName(){
	return decabotName;
}

void Decabot::yourNameIs(String parameter){
	//change Decabot's tag name on EEPROM
	decabotName = parameter;
	soundRecording();
	EEPROM.update(896,'[');
	EEPROM.update(897,'n');
	EEPROM.update(898,'a');
	EEPROM.update(899,'m');
	EEPROM.update(900,'e');
	EEPROM.update(901,']');
	int i=0;
	for(i;i<=decabotName.length();i++){
		EEPROM.update(i + 902,decabotName[i]);
	}
	EEPROM.update(i+902,'[');
	EEPROM.update(i+903,'/');
	EEPROM.update(i+904,']');
	output(F("new name:"));
	Serial.println(decabotName);
}

void Decabot::yourOwnerIs(String parameter){
	//change decabot emails owner on EEPROM
	decabotOwner = 	parameter.substring(0,42);
	parameter.concat("[/]");
	soundRecording();
	EEPROM.update(912,'[');
	EEPROM.update(913,'o');
	EEPROM.update(914,'w');
	EEPROM.update(915,'n');
	EEPROM.update(916,']');
	int i=0;
	for(i;i<parameter.length();i++){
		if(i+917>959) break;
		EEPROM.update(i + 917,parameter[i]);
	}
	output(F("Decabot [owner]["));
	Serial.print(decabotOwner);
	Serial.println(F("][/]"));
}

void Decabot::primeDirectives(){
	outputln("Prime Directives:");
	Serial.println("\t1. Serve the public trust");
	Serial.println("\t2. Protect the innocent");
	Serial.println("\t3. Uphold the law");
	delay(3000);
	Serial.println("\t4. not found... [?]");
}

void Decabot::outputln(String message) {
	//Terminal-like output, with timestamp
	//User must start serial in setup() to work
	String msg = "";
	msg.concat(String(millis()/1000.0,2));
	msg.concat(F("\t"));
	for(int i=0;i<repeatCalls;i++) msg.concat(F(" | "));
	msg.concat(message);
	Serial.println(msg);
}

void Decabot::formatROM(){
	if(1){
		outputln("Formatting ROM...");
		for(int i=0;i<EEPROM.length();i++){
			EEPROM.update(i,'O');
		}
		outputln("Format complete!");
		//yourNameIs(sayMyName());
		dumpMemory();
	} else {
		outputln(F("To format you must input robot's name as a parameter"));
	}
}	

void Decabot::output(String message) {
	//Terminal-like output, with timestamp
	//Doesnt break line at the end
	String msg = "";
	msg.concat(String(millis()/1000.0,2));
	msg.concat(F("\t"));
	for(int i=0;i<repeatCalls;i++) msg.concat(F(" | "));
	msg.concat(message);
	Serial.print(msg);
}

void Decabot::dumpMemory(){
	Serial.print("\t0");
	for(int i=3;i<=64;i++){
		if(i%16==0) {
			Serial.print(i);
		} else {
			Serial.print(" ");
		}
	}
	for(int i=0;i<EEPROM.length()+128;i++) {
		if((i%16==0)&&(i%64!=0)) Serial.print(" ");
		if(i%64==0) {
			Serial.println("");
			if(i<128){
				Serial.print("[RAM"); 
			} else {
			
				if(i<768){
					Serial.print("[ROM");
					Serial.print((i-128)/64);
				} else {
					if(i<832){
						Serial.print("[SETUP");
					} else {
						if(i<1024){
							Serial.print("[LOOP");
							Serial.print(((i-128)/64)-10);
						} else {
							if(i<1152){
								Serial.print("[ROBOT");
							} else {
								Serial.print("Flash");
								Serial.print((i-128)/64);
							}
						}
					}
				}

			}
			Serial.print("]\t");
		}
		Serial.print(infiniteCode(i));
		if(infiniteCode(i)==' ') Serial.print('-');
	}
	Serial.println("");
	outputln(F("Dump memory complete!"));
}

void Decabot::beep(int time){
	tone(buzzerPin, 1000, 50);
	delay(time);
	noTone(buzzerPin);
	delay(time);
}

void Decabot::soundBoot() {
	digitalWrite(ledPin, HIGH);
	ledFaceEyes(5);
	for(int i=0;i<4;i++){
		ledFaceMouth(i);
		printFace();
		tone(buzzerPin, decabotMusic[i][0], decabotMusic[i][1]*200);
    	delay(decabotMusic[i][1]*200);
    	noTone(buzzerPin);
	}
	digitalWrite(ledPin, LOW);
}

void Decabot::soundBegin() {
	outputln(F("Start!"));
	tone(buzzerPin, 440, 200);
	delay(200);
	tone(buzzerPin, 494, 200);
	delay(200);
	tone(buzzerPin, 523, 400);
	delay(400);
	noTone(buzzerPin);
}

void Decabot::soundEnd() {
	outputln(F("End!"));
	tone(buzzerPin, 440, 50);
	delay(50);
	tone(buzzerPin, 494, 50);
	delay(50);
	tone(buzzerPin, 523, 400);
	delay(400);
	tone(buzzerPin, 494, 50);
	delay(50);
	tone(buzzerPin, 440, 50);
	delay(50);
	tone(buzzerPin, 391, 400);
	delay(400);
	noTone(buzzerPin);
}

void Decabot::soundRecording() {
	//sound played everytime something is recorded in EEPROM
	for(int x=0;x<3;x++){
		beep(50);
		delay(50);
	}
	delay(200);
}

void Decabot::oneStepLeft(int dir) {
	if(dir==1){
		leftStep++;
	} else {
		leftStep--;
	}
	if(leftStep>4) leftStep = 1;
	if(leftStep<1) leftStep = 4;
}

void Decabot::oneStepRight(int dir) {
	if(dir==1){
		rightStep++;
	} else {
		rightStep--;
	}
	if(rightStep>4) rightStep = 1;
	if(rightStep<1) rightStep = 4;
}

void Decabot::move(int distance, int leftDirection, int RightDirection){
	for(int i=0;i<distance;i++){
		oneStepLeft(leftDirection);
		oneStepRight(RightDirection);
		updateMotors();
		delay(millisDelay); //That's why you must avoid using this movements commands!
	}
	resetMotors();
}

void Decabot::forward(int centimeters){
	String msg = F("Going forward ");
	msg.concat(centimeters);
	msg.concat(F("cm"));
	outputln(msg);
	move(centimeters * 100,1,1);
}

void Decabot::left(int degrees){
	String msg = F("Turning left ");
	msg.concat(degrees);
	msg.concat(F("°"));
	outputln(msg);
	move(degrees * 7.8,1,0);
}

void Decabot::right(int degrees){
	String msg = F("Turning right ");
	msg.concat(degrees);
	msg.concat(F("°"));
	outputln(msg);
	move(degrees * 7.8,0,1);
}

void Decabot::codeDomino(char code[]){
	//loads codeDomino on RAM or ROM
	if(code[0]=='S') {	//if the first piece of code is Save
		outputln("Saving program on ROM...");
		soundRecording();
		int parameterDigits = 0;
		int parameterData = 0;
		//check how many numbers are after the command Save
		while(isDigit(code[1+parameterDigits])){
			parameterDigits++;
		}
		int j=0;
		for(int k = parameterDigits;k>0;k--){
			parameterData = parameterData + ((int)(code[k]) - 48) * poten(10,j); 
			j++;
		}
		//define the memory block address by the parameter after command Save
		int memoryBlock = parameterData;
		int i=0;
		for(i;i<128;i++){
			EEPROM.update(i + (memoryBlock*64),code[i+parameterDigits+1]);
			if(code[i] == 'O') break;
		}
		while(i%64!=0) {
			//fill the rest of memory block with end command
			EEPROM.update(i + (memoryBlock*64),'O');
			i++;
		}
		programName(memoryBlock*64+128);
		outputln("Complete!");
	} else {
		output(F("New program loaded on RAM!\n[load]"));
		Serial.print(F("\t"));
		int i=0;
		bool writingName = 0;
		for(i=0;i<128;i++){
			runningCode[i] = code[i]; //fill the given code to RAM
			if(runningCode[i]=='[') writingName = 1;
			if(!writingName) Serial.print(F("[")); else Serial.print(F(" "));		
			Serial.print(runningCode[i]);
			if(!writingName) Serial.print(F("]")); else Serial.print(F(" "));		
			if(runningCode[i]==']') writingName = 0;
			if(runningCode[i]=='O') break;
		}
		Serial.println("");
		Serial.print(F("\t "));
		for(int j=0;j<=i;j++){
			Serial.print(j+1);
			if(j<8) {
				Serial.print("  ");
			} else {
				Serial.print(" ");
			}
		}
		Serial.println("");
		for(i;i<128;i++) runningCode[i]='O'; //fill the rest of the array with End command
	}
	loadLedsCheck();
}

void Decabot::saveCodeROM(int memoryBlock) {
	/*
	outputln("Saving program on ROM...");
	soundRecording();	
	int i=0;
	for(i;i<128;i++){
		EEPROM.update(i + (memoryBlock*64),runningCode[i]);
		if(runningCode[i] == 'O') break;
	}
	runningCodeIndex = i; //ends the execution of the code imediatelly
	while(i%64!=0) {
		//fill the rest of memory block with end command
		EEPROM.update(i + (memoryBlock*64),'O');
		i++;
	}
	outputln("Complete!");
	*/
}

void Decabot::run(){
	soundBegin();
	runningCodeIndex = 0;
	codeMillisBegin = millis();
	executing = 1;
	nextCommand();
}

void Decabot::run(int blockMemory){
	soundBegin();
	runningCodeIndex = (blockMemory * 64) + 128;
	codeMillisBegin = millis();
	executing = 1;
	nextCommand();
}

void Decabot::nextCommand(){
	//call commands using the running Code Index variable and the runningCode array
	if(!isDigit(infiniteCode(runningCodeIndex))){ //check if it is a letter
		output(F("("));
		Serial.print(runningCodeIndex+1);
		Serial.print(F(")"));
		int parameterDigits = 0;
		int parameterData = 0;
		//check how many numbers are after the command
		while(isDigit(infiniteCode(runningCodeIndex+1+parameterDigits))){
			parameterDigits++;
		}
		int j=0;
		for(parameterDigits;parameterDigits>0;parameterDigits--){
			parameterData = parameterData + ((int)(infiniteCode(runningCodeIndex+parameterDigits)) - 48) * poten(10,j); 
			j++;
		}
		codeInterpreter(infiniteCode(runningCodeIndex),parameterData);
		runningCodeIndex++;
	} else {
		runningCodeIndex++;
	}
}

char Decabot::infiniteCode(int index){
	//treats RAM and EEPROM as a unique sequential memory
	if(index<128){
		return runningCode[index];
	} else {
		return EEPROM.read(index-128);
	}
}

void Decabot::codeInterpreter(char command, int parameter){
	if(command=='A') unknowCode();
	if(command=='a') ledFaceEyebrows(varC, parameter);	//change angry state face
	if(command=='B') codeGoTo(parameter);		//go to a position in code
	if(command=='b') ledFaceMouth(parameter);	//change mouth
	if(command=='C') unknowCode();
	if(command=='c') ledFaceEyebrows(parameter, varA);	//change eyebrow close state
	if(command=='D') unknowCode();
	if(command=='d') unknowCode();
	if(command=='E') unknowCode();
	if(command=='e') ledFaceEyes(parameter);	//move eyes
	if(command=='F') codeForward(parameter);	//move forward
	if(command=='f') printFace();
	if(command=='G') saveCodeROM(parameter);	
	if(command=='g') unknowCode();
	if(command=='H') unknowCode();
	if(command=='h') unknowCode();
	if(command=='I') unknowCode();
	if(command=='i') unknowCode();
	if(command=='J') unknowCode();
	if(command=='j') ledMatrixBattery(4);
	if(command=='K') unknowCode();
	if(command=='k') unknowCode();
	if(command=='L') codeLeft(parameter);		//turn left
	if(command=='l') unknowCode();
	if(command=='M') codeMusic(parameter);		//play sound
	if(command=='m') dumpMemory();			//dump memory on serial
	if(command=='N') unknowCode();
	if(command=='n') sayMyName();			//return decabot name
	if(command=='O') codeEnd();			//end of block of code
	if(command=='o') unknowCode();
	if(command=='P') unknowCode();
	if(command=='p') unknowCode();
	if(command=='Q') unknowCode();
	if(command=='q') unknowCode();
	if(command=='R') codeRight(parameter);		//turn right
	if(command=='r') run();
	if(command=='S') saveCodeROM(parameter);
	if(command=='s') codeSpeed(parameter);		//set temporary motor speed
	if(command=='T') unknowCode();
	if(command=='t') unknowCode();
	if(command=='U') objectDetection(1);
	if(command=='u') codeScanObjectPrecision(parameter);
	if(command=='V') unknowCode();
	if(command=='W') codeWait(parameter);		//wait 
	if(command=='w') whoAmI();
	if(command=='X') codeRepeat(parameter);
	if(command=='x') setPosition(parameter,yPos);	//define the X position in the arena
	if(command=='Y') codeStopRepeat();
	if(command=='y') setPosition(xPos,parameter);	//defune the Y position in the arena
	if(command=='Z') codeRunBlockMem(parameter);	//run a block of code from ROM
	if(command=='z') unknowCode();
	if(command=='[') programName(runningCodeIndex);
	if(command=='?') showPosition();
	if(command=='#') ledMatrixRandom();
}

String Decabot::programName(int memoryPosition){
	String name;
	if(infiniteCode(memoryPosition)!='[') {
		name = "unknow";	
	} else {
		memoryPosition++;	
		while(infiniteCode(memoryPosition)!=']'){
			name.concat(infiniteCode(memoryPosition));
			memoryPosition++;
		}
		runningCodeIndex = memoryPosition;
	}
	String msg = F("[program][");
	msg.concat((memoryPosition-128)/64);
	msg.concat("][");
	msg.concat(name);
	msg.concat("][/]");
	outputln(msg);
	return name;
}

void Decabot::loadLedsCheck(){
	if(infiniteCode(0)!='[') {
		ledPinState = 0;	
	} else {
		ledPinState = 1;
	}
}

void Decabot::unknowCode(){
	outputln("Unknow command");
}

void Decabot::codeForward(int distance){
	if(distance == 0) distance = 10; //set default
	leftSpeed = rightSpeed = millisDelay;
	leftDirection = rightDirection = 1;
	String msg = F("[forward][");
	msg.concat(distance);
	msg.concat(F("]cm"));
	Serial.println(msg);
	moving = 1;
	stepsToMove += distance * 100; //transform distance in steps
	//self position
	incrementXYPosition(distance);
	//ultrasonic
	lastDetection = 0; //resets the detection
}

void Decabot::codeForward(int distance,int speed){
	if(distance == 0) distance = 10; //set default
	leftSpeed = rightSpeed = map(speed,100,0,millisDelay,50);
	leftDirection = rightDirection = 1;
	String msg = F("[forward][");
	msg.concat(distance);
	msg.concat(F("]cm"));
	Serial.println(msg);
	moving = 1;
	stepsToMove += distance * 100; //transform distance in steps
	//self position
	incrementXYPosition(distance);
	//ultrasonic
	lastDetection = 0; //resets the detection
}

void Decabot::setHeading(float degrees){
	heading = degrees;
	showPosition();
}


void Decabot::incrementXYPosition(float distanceForward){
	xPos += sin(radian(heading)) * distanceForward;
	yPos +=	cos(radian(heading)) * distanceForward;
	showPosition();
}

void Decabot::codeLeft(int degrees){
	if(degrees == 0) degrees = 90; //set default
	leftSpeed = rightSpeed = millisDelay;
	leftDirection = 1;
	rightDirection = 0;
	String msg = F("[left][");
	msg.concat(degrees);
	msg.concat(F("]°"));
	Serial.println(msg);
	turningLeft = 1;
	moving = 1;
	stepsToMove = degrees * 7.8;
	rotatingTotal = heading + degrees;
	/*
	//self position
	heading += degrees;
	if(heading>360) heading -= 360;
	if(heading<0) heading += 360;
	*/
}

void Decabot::codeRight(int degrees){
	if(degrees == 0) degrees = 90; //set default
	leftSpeed = rightSpeed = millisDelay;
	leftDirection = 0;
	rightDirection = 1;
	String msg = F("[right][");
	msg.concat(degrees);
	msg.concat(F("]°"));
	Serial.println(msg);
	turningLeft = 0;
	moving = 1;
	stepsToMove = degrees * 7.8;
	rotatingTotal = heading - degrees;
	/*
	rotatingTotal = stepsToMove; //variable to help adjustHeading to calculate angle in real time
	//self position
	heading -= degrees;
	if(heading>360) heading -= 360;
	if(heading<0) heading += 360;
	*/
}

void Decabot::adjustHeading(){
	if(leftDirection&&!rightDirection) heading += 0.064102564;
	if(!leftDirection&&rightDirection) heading -= 0.064102564;
	if(heading>360) heading -= 360;
	if(heading<0) heading += 360;
	if(millis()%500==0) {
		String msg = F("[heading][");
		msg.concat(heading);
		msg.concat("][/]");
		outputln(msg);
	}
}

void Decabot::codeSpeed(int speed){
	if(speed == 0) speed = 9; //set default
	millisDelay = map(speed,10,1,3,13);
	String msg = F("[speed][");
	msg.concat(speed);
	msg.concat(F("]"));
	Serial.println(msg);
}

void Decabot::setRadious(float radious){
	//define radious to curve commands
	curveRadious = radious;
}

void Decabot::codeCurveLeft(int degrees){
	leftSpeed = rightSpeed = millisDelay;
	leftDirection = 1;
	rightDirection = 0;
	String msg = F("[curve left][");
	msg.concat(degrees);
	msg.concat(F("]° rad["));
	msg.concat(curveRadious);
	msg.concat(F("]cm"));
	Serial.println(msg);
	turningLeft = 1;
	moving = 1;
	stepsToMove = degrees * 7.8;
}	

void Decabot::setPosition(float x, float y){
	xPos = x;
	yPos = y;
	showPosition();
}

void Decabot::showPosition(){
	tmpOutput = F("[position][");
	tmpOutput.concat(sayMyName());
	tmpOutput.concat(F("]["));
	tmpOutput.concat(xPos);
	tmpOutput.concat(F("]["));
	tmpOutput.concat(yPos);
	tmpOutput.concat(F("]["));
	tmpOutput.concat(heading);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
}

void Decabot::codeEnd(){
	if(repeatCalls>0){
		runningCodeIndex = repeatPointers[repeatCalls];
		repeatCalls--;
		Serial.println(F("[return to program]"));
	} else {
		stepsToMove = 0; //stop moving
		executing = 0;
		Serial.print(F("[end] \n\tExecution time: "));
		int runTime = round((millis() - codeMillisBegin)/1000); 
		if(runTime<60){
			Serial.print(runTime);
			Serial.println(F(" second(s)"));
		} else {
			int runTimeMin = round(runTime/60);
			runTime = runTime%60;
			Serial.print(runTimeMin);
			Serial.print(F(" minute(s) and "));
			Serial.print(runTime);
			Serial.println(F(" second(s)"));
		}
		soundEnd();
		loadLedsCheck();
		resetMotors();
	}
}

void Decabot::codeGoTo(int piece){
	runningCodeIndex = piece-2;
	String msg = F("[go to] piece [");
	msg.concat(piece);
	msg.concat(F("]"));
	Serial.println(msg);
}

void Decabot::codeRepeat(int times){
	repeatCalls++; //increase the number of repetitions called
	String msg = F("[repeat][");
	msg.concat(times);
	msg.concat(F("]x"));
	Serial.println(msg);
	repeatPointers[repeatCalls] = runningCodeIndex + 1; //mark the begin of repetition, so the code can go back each interation
	repeatCounters[repeatCalls] = times; //mark the number of repetitions of each interation
}

void Decabot::codeRunBlockMem(int block){
	repeatCalls++; //increase the number of repetitions called
	String msg = F("[run][");
	msg.concat(block);
	msg.concat(F("]mem"));
	Serial.println(msg);
	repeatPointers[repeatCalls] = runningCodeIndex + 1; //mark the begin of repetition, so the code can go back each interation
	repeatCounters[repeatCalls] = 1; //doesn't have loop
	runningCodeIndex = (block * 64) + 127; //pointer start running from EEPROM. Ex: block 3 will be 3*64+128 = index 320 in the infiniteCode();
}

void Decabot::codeStopRepeat(){
	String msg = "";
	if(repeatCounters[repeatCalls]>1){ //check if still needs to repeat
		runningCodeIndex = repeatPointers[repeatCalls];
		repeatCounters[repeatCalls]--;
		msg.concat(F("[return to begin]"));
		msg.concat(repeatCounters[repeatCalls]);
		msg.concat(F("x"));
	} else {
		msg.concat(F("[return to code]"));
		repeatCalls--;
	}
	Serial.println(msg);	
}

void Decabot::codeWait(int timeWait){
	String msg = F("[wait][");
	msg.concat(timeWait);
	msg.concat(F("]secs"));
	Serial.println(msg);
	delay(timeWait * 1000);
}

void Decabot::codeMusic(int toneFreq){
	String msg = F("[music][");
	msg.concat(toneFreq);
	msg.concat(F("]"));
	Serial.println(msg);
	tone(buzzerPin, toneFreq);
	delay(500);
	noTone(buzzerPin);
}

void Decabot::updateSteps(){
	//check if there are movements to do
	if(stepsToMove > 0) {
		actualMillis = millis();
		if(actualMillis - lastLeftMillis >= leftSpeed){
			lastLeftMillis = actualMillis;
			oneStepLeft(leftDirection);
			if(turningLeft) stepsToMove--;
		}
		if(actualMillis - lastRightMillis >= rightSpeed){
			lastRightMillis = actualMillis;
			oneStepRight(rightDirection);
			if(!turningLeft) stepsToMove--;
		}
		adjustHeading();
	} else {
		moving = 0;
		//recalibrate the heading after the spin
		heading = rotatingTotal;
		if(heading>=360) heading -= 360;
		if(heading<0) heading += 360;
	}
}

void Decabot::update(){
	// Fire moving functions each delay in millis, to make step motors work properlly
	// Substitute for delay in moving steps
	if(moving){
		updateSteps();
		updateMotors();
		if(millis()%200==0){
			if(ledPinState) {
				ledPinState = 0;
			} else { 
				ledPinState = 1;
			}
		}
	} else {
		if(executing){
			nextCommand();
		}
	}
	if(millis()%objectDetectionDelay==0){ //object detection doesn't work while moving, because the self position is not incremental
		objectDetection(0);
	}
	if(millis()%5000==0){
		showPosition();
		//ledFaceEyes(random(3)+4);
		//ledFaceEyebrows(2,2);
		//ledFaceMouth(random(4));
	}
	readButton();
	//check if has a led matrix and if need to update the led matrix
	if((bitRead(decabotConfiguration,3))&&faceChanged){
		printFace();
		faceChanged = 0;
	}
	digitalWrite(ledPin, ledPinState);
	delay(1); //slow the code to not to run twice the update in the same millissecond
}

void Decabot::checkCodeDominoLoops(){
	//check if need to run the codes on loop memories
}

double Decabot::measureDistance(){
	ultrasonicTimer = millis();
	// Pulso de 5V por pelo menos 10us para iniciar medição.
	// 5V pulse by at least 10 milisseconds to measure
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(11);
	digitalWrite(trigPin, LOW);
	uint32_t pulseTime = pulseIn(echoPin, HIGH);
	double distance = 5 + 0.01715 * pulseTime; //plus 5 centimeters, because the sensor is on the edge of the robot
	// filter if the sensor is not installed, or high imprecise data
	if((distance!=0)&&(distance<100)){
		return distance;
	} else {
		return 0;
	}
}

void Decabot::objectDetection(bool forced){
	//check if there is a sensor
	if(bitRead(decabotConfiguration,0)){
		double hip = measureDistance(); //hipotenusa receives measured distance from ultrasonic
		if(hip!=0){ //check if the ultrasonic sensor is installed
			if(!((hip>=lastDetection-1)&&(hip<=lastDetection+1))){ //removes noise from ultrasonic reading
				int objX = (cos(radian(heading)) * hip) + xPos;
				int objY = (sin(radian(heading)) * hip) + xPos;
				String msg = "[obstacle][";
				msg.concat(objX);
				msg.concat("][");
				msg.concat(objY);
				msg.concat("][foundBy][");
				msg.concat(decabotName);
				msg.concat("][/]");
				outputln(msg);
				lastDetection = hip;
			}
		}
	} else {
		if(forced){
			outputln(F("[no distance sensor][/]"));
		}
	}
}

void Decabot::codeScanObjectPrecision(int range){
	//set from 1 to 5 the precision of scanning objects
	objectDetectionDelay = map(range,1,5,1000,100);
	String msg = F("[scan precision] level [");
	msg.concat(range);
	msg.concat(F("]"));
	Serial.println(msg);
}

void Decabot::debug(){
	Serial.print("\t actualMillis:");
	Serial.print(actualMillis);
	Serial.print("\t stepsToMove:");
	Serial.print(stepsToMove);
	Serial.print("\t moving:");
	Serial.print(moving);
	Serial.print("\t executing:");
	Serial.println(executing);
}

int Decabot::poten(int base, int expoent){
	if(expoent == 0)
	return 1;
	else   
	return base * poten(base, expoent - 1);
}

float Decabot::radian(float degree){
	return (degree * 71) / 4068;
}

//Led Matrix functions

void Decabot::ledMatrixInit(){
	outputln(F("Initializing led matrix..."));
	// disable test mode. datasheet table 10
	ledMatrixSetRegister(MAX7219_DISPLAYTEST_REG, MAX7219_OFF);
	// set medium intensity. datasheet table 7
	ledMatrixSetRegister(MAX7219_INTENSITY_REG, 0x1);
	// turn off display. datasheet table 3
	ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF);
	// drive 8 digits. datasheet table 8
	ledMatrixSetRegister(MAX7219_SCANLIMIT_REG, 7);
	// no decode mode for all positions. datasheet table 4
	ledMatrixSetRegister(MAX7219_DECODE_REG, B00000000);

	// clear matrix display
	ledMatrixClear();
}

void Decabot::ledMatrixSetRegister(byte address, byte value){
	digitalWrite(ledLatchPin, LOW);
	shiftOut(ledDataPin, ledClockPin, MSBFIRST, address);
	shiftOut(ledDataPin, ledClockPin, MSBFIRST, value);
	digitalWrite(ledLatchPin, HIGH);
}

void Decabot::ledMatrixClear(){
	// clear the dot matrix
	for (int i = 0; i < NUM_OF_COLUMNS; i++)
	{
		ledMatrixSetRegister(MAX7219_COLUMN_REG(i), B00000000);
	}
}

void Decabot::ledMatrixRandom(){
	for(int i=0;i<8;i++){
		ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
		ledMatrixSetRegister(i+1, random(256));
		ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
	}
	outputln(F("[leds random][/]"));
}

void Decabot::ledMatrixBattery(int value){
	ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
	ledMatrixSetRegister(1,battery[0]);
	ledMatrixSetRegister(2,battery[1]);
	for(int i=5;i>0;i--){
	if(value > 0) {
		ledMatrixSetRegister(i+2,battery[1]);
	} else {
		ledMatrixSetRegister(i+2,battery[2]);
	}
	value--;
	}
	ledMatrixSetRegister(8,battery[1]);
	ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
	outputln(F("[leds battery][/]"));
}

void Decabot::ledFaceClearMem(){
	for(int i=0;i<8;i++){
		ledFaceMem[i] = B00000000;
	}
}

void Decabot::ledFaceEyes(int position) {
	varE = position;
	faceChanged = 1;
	tmpOutput = F("[face eyes][");
	tmpOutput.concat(position);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
}

void Decabot::ledFaceEyebrows(int closed, int angry){
	varA = angry;
	varC = closed;
	faceChanged = 1;
	tmpOutput = F("[face eye brows][");
	tmpOutput.concat(closed);
	tmpOutput.concat(F("]["));
	tmpOutput.concat(angry);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
}

void Decabot::ledFaceMouth(int index){
	varB = index;
	faceChanged = 1;
	tmpOutput = F("[face mouth][");
	tmpOutput.concat(index);
	tmpOutput.concat(F("][/]"));
	outputln(tmpOutput);
}

void Decabot::printFace(){
	//clear face memory
	ledFaceClearMem();
	//put eyes on memory
	int eyePositionX = varE;
	while(eyePositionX>3){
		eyePositionX = eyePositionX - 3;
	}
	eyePositionX--;
	int eyePositionY;
	if(varE<=3) {
		eyePositionY = 1;
	} else if(varE>=7){
		eyePositionY = 3;
	} else {
		eyePositionY = 2;
	}
	ledFaceMem[eyePositionY] = eye[eyePositionX][0];
	ledFaceMem[eyePositionY+1] = eye[eyePositionX][1];
	//put eye brows on memory
	if(varC>4) varC=4;
	varC--; //gambs
	for(int i= 1;i<varC;i++){
		//clear eyes pixels
		ledFaceMem[varC-2] = ledFaceMem[varC-2] & eyeClear[varA-1][0];
		ledFaceMem[varC-1] = ledFaceMem[varC-1] & eyeClear[varA-1][1];
		ledFaceMem[varC] = ledFaceMem[varC] & eyeClear[varA-1][2];
	}
	//draw eyebrows
	ledFaceMem[varC-1] = ledFaceMem[varC-1] | eyeBrow[varA-1][0];
	ledFaceMem[varC] = ledFaceMem[varC] | eyeBrow[varA-1][1];
	ledFaceMem[varC+1] = ledFaceMem[varC+1] | eyeBrow[varA-1][2];
	varC++;
	//put mouth on memory
	if(varB!=0){
		ledFaceMem[5] = mouth[varB-1][0];
		ledFaceMem[6] = mouth[varB-1][1];
		ledFaceMem[7] = mouth[varB-1][2];
	}
	//print memory on leds
	for(int i=0;i<8;i++){
		ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
		ledMatrixSetRegister(i+1,ledFaceMem[i]);
		ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
	}
	outputln(F("[leds face][/]"));
}