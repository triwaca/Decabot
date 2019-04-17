#include <Decabot.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Ultrasonic.h>
#include <EEPROM.h>
#include <Servo.h>

// Code Domino
// by Daniel Chagas <http://www.simiano.net>
// Demostrates the use of a Decabot robot with code domino RFID pieces.
// Created 17 April 2019


//pinout
int servo = 19;
int sck = 13; //SPI
int miso = 12; //SPI
int mosi = 11; //SPI
#define SS_PIN 10
#define RST_PIN 9
int latchPin = 8; //Pin connected to ST_CP of 74HC595
int clockPin = 7; //Pin connected to SH_CP of 74HC595
int dataPin = 6; ////Pin connected to DS of 74HC595
int trig = 5; //ultrasonic trigger
int echo = 4; //ultrasonic echo
int buzzer = 3; //sound
int led = 2; //status led
//Digital 0 and 1 are for serial communications
//A0 connects all 6 buttons

//instances
Decabot decabot = Decabot();
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte buffer[18]; 
byte size = sizeof(buffer);
uint8_t pageAddr = 0x06;
Ultrasonic ultrasonic(5, 4);
Servo pen;

void setup()
{
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(14, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  SPI.begin(); 
  mfrc522.PCD_Init();
  pen.attach(servo);
  decabot.penUp();
  decabot.resetMotors();
  decabot.startSound();
}

void loop()
{
  decabot.readRfid();
  decabot.readButtons();
  decabot.readDistance();
  delay(5);
}

