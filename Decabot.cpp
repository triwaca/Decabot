/*
  Decabot.h
  Daniel Chagas
*/


#include"Decabot.h"

Decabot::Decabot(int nothing) 
{
	//define pin modes for shift register, buzzer and LED
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
	output(F("Initializing Decabot..."));
	resetMotor();
	whoami();
	output(F("READY!"));
	soundBegin();
}

void Decabot::resetMotor() {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, B00000000); //Send all off to 74HC595
	digitalWrite(latchPin, HIGH);
	output(F("Reset motors"));
}

void Decabot::whoami() {
      //OUtputs tag name and owner e-mail of Decabot
      UniqueIDdump(Serial);
      String tmp1 = F("Decabot Name: ");
      for(int i=896;i<=900;i++){
        tmp1.concat((char) EEPROM.read(i));
        decabotName[i - 896] = EEPROM.read(i);
      }
      output(tmp1);
      String tmp2 = F("Owner: ");
      for(int i=901;i<950;i++){
        tmp2.concat((char) EEPROM.read(i));
      }
      output(tmp2);
}

void Decabot::yourNameIs(String parameter){
      //change Decabot's tag name on EEPROM
      recordingSound();
      parameter.toCharArray(decabotName,6);
      for(int i=0;i<=5;i++){
        EEPROM.write(i + 896,decabotName[i]);
      }
      output(F("new name:"));
      output(decabotName);
}

void Decabot::yourOwnerIs(String parameter){
      //change decabot emails owner on EEPROM
      recordingSound();
      parameter.toCharArray(decabotOwner,50);
      for(int i=0;i<=50;i++){
        EEPROM.write(i + 902,decabotOwner[i]);
      }
      output(F("Decabot owner: "));
      output(decabotOwner);
}

void Decabot::output(String message) {
	//Terminal-like output, with timestamp
	//User must start serial in setup() to work
	String msg = "";
	msg.concat(String(millis()/1000.0,2));
	msg.concat(F(">"));
	msg.concat(message);
	Serial.println(msg);
}

void Decabot::beep(int time){
	tone(buzzerPin, 1000, 50);
	delay(time);
	noTone(buzzerPin);
	delay(time);
}

void Decabot::soundBegin() {
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

void Decabot::recordingSound() {
      //sound played everytime something is recorded in EEPROM
      for(int x=0;x<3;x++){
        beep(50);
        delay(50);
      }
}

