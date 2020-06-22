# Decabot
Decabot is a low-cost robotic platform designed for those interested in educational robotics to build, customize and adapt their own robots at home, in their school, college or community. A Decabot consists of a laser-cut chassis (MDF, acrylic, PVC, etc.) measuring exactly 10 centimeters in width, height and length (around 3,9 inches). The robot is designed to work with the Arduino UNO platform. Access www.decabot.com to know more.

Decabot is the first robot compatible with the tangible programming language Code Domino (www.codedomino.org) being accessible to children and beginners in programming. However its ease of use allows learner and experienced users to program it directly in the Arduino IDE interface, through the C language.

## Hardware
To build a basic Decabot robot you must get a laser cut chassis in 2.5mm MDF or acrylic, assemble it (intructions on www.decabot.com) and install the following components:
* Arduino Uno or compatible board;
* 2 x 28BYJ-48 step motor, with 2 x Driver ULN2003;
* A decabot shield with a 74HC595 shift register;
* Power source;
* optional HC-SR04 ultrasonic sensor;
* optional MFRC522 RFID sensor (to read phisical Code Domino pieces);
* optional HC-06 or HC-05 bluetooth module.

## Basic commands
To use the library you must declare your Decabot object:
```
#include <Decabot.h>
Decabot robot(4);
void setup() {
  Serial.begin(9600);
  robot.boot();           //initialize Decabot
}
```
The parameter number in the object (in the example, 4) reffers to the delay time in each motor step. A safe number is 4. Higher delay will make the robot to walk slowly. Lower delay will make it run faster, but the 28BYJ-48 probabily will not run with a delay less than 3. Serial.begin on setup is optional, but it will show the telemetry messages from the robot.

To move forward, you can use:
```
robot.forward(10);
```
To draw a 10cm square, you can use:
```
for(int i=1;i<=4;i++){  //repeat forward and left 4 times to draw a square
    robot.forward(10);    //move forward 10 centimeters
    robot.left(90);       //turn left 90°
}
```
## Code Domino
The best way to play with your Decabot is using Code Domino language. To use it by typing:
```
#include <Decabot.h>
Decabot robot(2); //create object Decabot with 4 millis between motor steps
void setup() {
  Serial.begin(9600);
  robot.boot();           //initialize Decabot
  robot.codeDomino("[square]X4FLYO"); //draw a square
  robot.run();
}

void loop() {
  robot.update();         //update state of Code Domino machine
}
```
Where the codeDomino() function must contain the String code. Commands are represented by letters, and parameters are numbers:
* **[ ]** text between simply brackets is the Code Domino's name, and is ignored by the Decabot;
* **X4** repeats the following commands 4 times;
* **F** goes forward. As it doesn't have a numeric parameter, the standard value is 10cm;
* **L** goes left. As it doesn't have a numeric parameter, the standard value is 90°;
* **Y** ends the repeat loop;
* **O** ends the code.

A list of Code Domino commands can be found at documentation folder. 

### Send CD codes through Serial

You can send Code Domino commands through Serial, by including this serialEvent() function after the loop():
```
void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}
```
See the documentation for other ways to run Code Domino on your Decabot!
