/*
 *  Decabot Project
 *  www.decabot.com
 *  
 *  Code to create a 10cm square. Put a whiteboard pen on the center of 
 *  Decabot to see him drawing the square.
 *  
 *  Need to use the Decabot Shield, who contains a 74HC595 shift register
 *  on pins 8, 7 and 6, connected to the step motor controlers ULN2003.
 *  The shield also contains a buzzer, connected on pin 3.
 *  
 *  2020 - Daniel Chagas
 * 
 */

#include <Decabot.h>

Decabot robot(4); //create object Decabot with 4 millis between motor steps

void setup() {
  Serial.begin(9600);
  robot.boot();           //initialize Decabot
  //save 3 codes on decabot's memory blocks
  robot.codeDomino("S0[square]X4FLYO");
  robot.codeDomino("S1[circle]X36F1L10YO");
  robot.codeDomino("S2[triangle]X3FL120YO");
  //execute (Z) the 3 drawings
  robot.codeDomino("[3draws]Z0FZ1FZ2O"); 
  robot.run();
}

void loop() {
  robot.update();         //update state of Code Domino machine
}
