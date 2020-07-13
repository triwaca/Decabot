#include <Decabot.h>

Decabot robot(3,B00000000); //create object Decabot without sensors

void setup() {
  Serial.begin(9600);
  robot.boot();
}

void loop() {
  robot.update(); //update state of Code Domino machine
}

void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}
