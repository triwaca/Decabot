#include <Decabot.h>

// Calibration
// by Daniel Chagas <http://www.simiano.net>

// Demostrates the movement of a Decabot robot doing a 10 x 10 cm square, and moving to the center of it.

// Created 17 April 2019


//create instance of Decabot
Decabot decabot = Decabot();

void setup()
{
  delay(2000);
  decabot.left();
  for(int i=0;i<4;i+){
    decabot.forward();
    decabot.right();
  }
  decabot.right(45);
  decabot.forward(7);	
  decabot.right(45);
}

void loop()
{
}

