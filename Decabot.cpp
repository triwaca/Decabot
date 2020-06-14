/*
  Decabot.h
  Daniel Chagas
*/


#include"Decabot.h"

Decabot::Decabot(int delay) 
{
	//define pin modes for shift register, buzzer and LED
	millisDelay = delay;
	pinMode(buzzerPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
}

Decabot::~Decabot() 
{
	//define pin modes for shift register, buzzer and LED
	pinMode(buzzerPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
}

void Decabot::boot(){
	//Boot sequence
	Serial.println(F(" - - - - - - - - - - - - - - - - - - - - - - - - - - D E C A N O"));
	outputln(F("Initializing Decabot..."));
	resetMotors();
	whoAmI();
	//soundBoot();
	outputln(F("READY!"));
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
	//OUtputs tag name and owner e-mail of Decabot
	Serial.print(F("\t"));
	UniqueIDdump(Serial);
	String tmp1 = F("Decabot Name: ");
	tmp1.concat(sayMyName());
	outputln(tmp1);
	String owner = F("Owner: ");
	for(int i=917;i<960;i++){
		if((char) EEPROM.read(i) == '[') break;
		owner.concat((char) EEPROM.read(i));
	}
	outputln(owner);
}

String Decabot::sayMyName(){
	decabotName = "";
	for(int i=902;i<=906;i++){
		if((char) EEPROM.read(i) == '[') break;
		decabotName.concat((char) EEPROM.read(i));		
	}
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
	output(F("Decabot owner:"));
	Serial.println(decabotOwner);
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

void Decabot::formatROM(String name){
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
				Serial.print("RAM"); 
			} else {
				if(i<1024){
					Serial.print("ROM");
					Serial.print((i-128)/64);
				} else {
					if(i<1152){
						Serial.print("ROBOT");
					} else {
						Serial.print("Flash");
						Serial.print((i-128)/64);
					}
				}
			}
			Serial.print("\t");
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
	for(int i=400;i<1000;i++){
		tone(buzzerPin, i, 3);
		delay(3);
	}
	noTone(buzzerPin);
	delay(50);
	beep(50);
	delay(50);
	beep(50);
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
		outputln(F("New program loaded on RAM!"));
		Serial.print(F("\t"));
		int i=0;
		for(i=0;i<128;i++){
			runningCode[i] = code[i]; //fill the given code to RAM
			Serial.print(F("["));		
			Serial.print(runningCode[i]);
			Serial.print(F("]"));		
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
	if(command=='a') unknowCode();
	if(command=='B') codeGoTo(parameter);		//go to a position in code
	if(command=='b') unknowCode();
	if(command=='C') unknowCode();
	if(command=='c') unknowCode();
	if(command=='D') unknowCode();
	if(command=='d') unknowCode();
	if(command=='E') unknowCode();
	if(command=='e') unknowCode();
	if(command=='F') codeForward(parameter);	//move forward
	if(command=='f') unknowCode();
	if(command=='G') saveCodeROM(parameter);
	if(command=='g') unknowCode();
	if(command=='H') unknowCode();
	if(command=='h') unknowCode();
	if(command=='I') unknowCode();
	if(command=='i') unknowCode();
	if(command=='J') unknowCode();
	if(command=='j') unknowCode();
	if(command=='K') unknowCode();
	if(command=='k') unknowCode();
	if(command=='L') codeLeft(parameter);		//turn left
	if(command=='l') unknowCode();
	if(command=='M') codeMusic(parameter);		//play sound
	if(command=='m') unknowCode();
	if(command=='N') unknowCode();
	if(command=='n') unknowCode();
	if(command=='O') codeEnd();			//end of block of code
	if(command=='o') unknowCode();
	if(command=='P') unknowCode();
	if(command=='p') unknowCode();
	if(command=='Q') unknowCode();
	if(command=='q') unknowCode();
	if(command=='R') codeRight(parameter);		//turn right
	if(command=='r') unknowCode();
	if(command=='S') saveCodeROM(parameter);
	if(command=='s') codeSpeed(parameter);		//set temporary motor speed
	if(command=='T') unknowCode();
	if(command=='t') unknowCode();
	if(command=='U') unknowCode();
	if(command=='u') unknowCode();
	if(command=='V') unknowCode();
	if(command=='W') codeWait(parameter);		//wait 
	if(command=='w') unknowCode();
	if(command=='X') codeRepeat(parameter);
	if(command=='x') setPosition(parameter,yPos);	//define the X position in the arena
	if(command=='Y') codeStopRepeat();
	if(command=='y') setPosition(xPos,parameter);	//defune the Y position in the arena
	if(command=='Z') codeRunBlockMem(parameter);
	if(command=='z') unknowCode();
	if(command=='[') programName(runningCodeIndex);
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
	String msg = F("program [");
	msg.concat(name);
	msg.concat("]");
	outputln(msg);
	return name;
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
	//self position
	heading += degrees;
	if(heading>360) heading -= 360;
	if(heading<0) heading += 360;
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
	//self position
	heading -= degrees;
	if(heading>360) heading -= 360;
	if(heading<0) heading += 360;
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
	String msg = sayMyName();
	msg.concat(F(" position: x="));
	msg.concat(xPos);
	msg.concat(F(", y="));
	msg.concat(yPos);
	msg.concat(F(", heading="));
	msg.concat(heading);
	outputln(msg);
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
	} else {
		moving = 0;
	}
}

void Decabot::update(){
	// Fire moving functions each delay in millis, to make step motors work properlly
	// Substitute for delay in moving steps
	if(moving){
		updateSteps();
		updateMotors();
	} else {
		if(executing){
			nextCommand();
		}
	}
	if(millis()%10000==0){
		showPosition();
	}
	delay(1); //slow the code to not to run twice the update in the same millissecond
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

