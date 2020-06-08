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
	for(int i=896;i<=900;i++){
		tmp1.concat((char) EEPROM.read(i));
		decabotName[i - 896] = EEPROM.read(i);
	}
	outputln(tmp1);
	String tmp2 = F("Owner: ");
	for(int i=901;i<950;i++){
		tmp2.concat((char) EEPROM.read(i));
	}
	outputln(tmp2);
}

void Decabot::yourNameIs(String parameter){
      //change Decabot's tag name on EEPROM
      soundRecording();
      parameter.toCharArray(decabotName,6);
      for(int i=0;i<=5;i++){
        EEPROM.write(i + 896,decabotName[i]);
      }
      outputln(F("new name:"));
      outputln(decabotName);
}

void Decabot::yourOwnerIs(String parameter){
      //change decabot emails owner on EEPROM
      soundRecording();
      parameter.toCharArray(decabotOwner,50);
      for(int i=0;i<=50;i++){
        EEPROM.write(i + 902,decabotOwner[i]);
      }
      outputln(F("Decabot owner: "));
      outputln(decabotOwner);
}

void Decabot::outputln(String message) {
	//Terminal-like output, with timestamp
	//User must start serial in setup() to work
	String msg = "";
	msg.concat(String(millis()/1000.0,2));
	msg.concat(F("\t"));
	msg.concat(message);
	Serial.println(msg);
}

void Decabot::output(String message) {
	//Terminal-like output, with timestamp
	//Doesnt break line at the end
	String msg = "";
	msg.concat(String(millis()/1000.0,2));
	msg.concat(F("\t"));
	msg.concat(message);
	Serial.print(msg);
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
	//loads codeDomino on RAM
	int i=0;
	outputln(F("New program loaded on RAM!"));
	Serial.print(F("\t"));
	for(i=0;i<64;i++){
		runningCode[i] = code[i]; //fill the given code to RAM
		Serial.print(F("["));		
		Serial.print(runningCode[i]);
		Serial.print(F("]"));		
		if(runningCode[i]=='O') break;
	}
	Serial.println("");
	for(i;i<128;i++) runningCode[i]='O'; //fill the rest of the array with End command
}

void Decabot::run(){
	soundBegin();
	runningCodeIndex = 0;
	executing = 1;
	nextCommand();
}

void Decabot::nextCommand(){
	//call commands using the running Code Index variable and the runningCode array
	if(!isDigit(runningCode[runningCodeIndex])){
		output(F("("));
		Serial.print(runningCodeIndex+1);
		Serial.print(F(")"));
		int parameterDigits = 0;
		int parameterData = 0;
		//check how many numbers are after the command
		while(isDigit(runningCode[runningCodeIndex+1+parameterDigits])){
			parameterDigits++;
		}
		int j=0;
		for(parameterDigits;parameterDigits>0;parameterDigits--){
			parameterData = parameterData + ((int)(runningCode[runningCodeIndex+parameterDigits]) - 48) * poten(10,j); 
			j++;
		}
		if(runningCode[runningCodeIndex]=='F') codeForward(parameterData);
		if(runningCode[runningCodeIndex]=='L') codeLeft(parameterData);
		if(runningCode[runningCodeIndex]=='R') codeRight(parameterData);
		if(runningCode[runningCodeIndex]=='S') codeSpeed(parameterData);
		if(runningCode[runningCodeIndex]=='O') codeEnd();
		runningCodeIndex++;
	} else {
		runningCodeIndex++;
	}
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
}

void Decabot::codeRight(int degrees){
	if(degrees == 0) degrees = 90; //set default
	leftSpeed = rightSpeed = millisDelay;
	leftDirection = 0;
	rightDirection = 1;
	String msg = F("[left][");
	msg.concat(degrees);
	msg.concat(F("]°"));
	Serial.println(msg);
	turningLeft = 0;
	moving = 1;
	stepsToMove = degrees * 7.8;
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

void Decabot::codeEnd(){
	stepsToMove = 0; //stop moving
	executing = 0;
	Serial.println(F("[end]"));
	soundEnd();
	resetMotors();
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
	/*
	actualMillis = millis();
	if(actualMillis - lastMillis >= millisDelay){
		lastMillis = actualMillis;
		if(moving){
			updateSteps();
			updateMotors();
		} else {
			if(executing){
				nextCommand();
			}
		}
	}
	*/
	if(moving){
		updateSteps();
		updateMotors();
	} else {
		if(executing){
			nextCommand();
		}
	}
	if(millis()%5000==0){
		//debug();
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
