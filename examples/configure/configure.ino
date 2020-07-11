#include <Decabot.h>

Decabot robot(3); //create object Decabot

void setup() {
  Serial.begin(9600);
  robot.boot();
  robot.formatROM();
  robot.yourNameIs("C3PO");
  robot.yourOwnerIs("anakin@tatooine.net");
}

void loop() {
  robot.update(); //update state of Code Domino machine
}

void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}
