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
  robot.boot();           //initialize Decabot
  delay(1000);
  robot.soundAfirmative();
  for(int i=1;i<=4;i++){  //repeat forward and left 4 times to draw a square
    robot.forward(10);    //move forward 10 centimeters
    delay(200);
    robot.left(90);       //turn left 90°
    delay(200);
  }
  robot.soundEnd();
}

void loop() {
  //nothing here! The Decabot draws only once.
}
