/*
 *  Upload Code Domino codes to ROM
 *  
 *  Will reccord 3 programs on ROM 0, 1 and 2, and will execute them in 
 *  sequence, drawing a square, a circle and a triangle. You can check
 *  the memory in the end typing 'm' on console (to dump memory).
 * 
 */

#include <Decabot.h>

Decabot robot(3); //create object Decabot

void setup() {
  Serial.begin(9600);
  robot.boot();
  //save 3 codes on decabot's memory blocks
  robot.codeDomino("S0[square]X4FLYO");
  robot.run();
  robot.codeDomino("S1[circle]X36F1L10YO");
  robot.run();
  robot.codeDomino("S2[triangle]X3FL120YO");
  robot.run();
  
  //execute (Z) the 3 drawings
  robot.codeDomino("[3draws]Z0W1Z1W1Z2O"); 
  robot.run();
}

void loop() {
  robot.update(); //update state of Code Domino machine
}

void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}
