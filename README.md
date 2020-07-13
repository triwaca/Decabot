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
* optional HC-06 or HC-05 bluetooth module;
* optional MAX7219 8 x 8 led display module.

## Instalation
Just install in your Arduino IDE as a normal library. Download the ZIP file, go to **sketch** -> **include library** -> **add library .ZIP** and point to the Decabot.ZIP. 

### Dependencies
You must install the following libraries to compile. 
* **UniqueID** library, from Luiz Cassetari (https://github.com/ricaun/ArduinoUniqueID). Tested with version 1.0.9;
* **MFRC522** library, from Miguel Balboa (https://github.com/miguelbalboa/rfid). Tested with version 1.4.6.

## Basic commands
To use the library you must declare your Decabot object:
```
#include <Decabot.h>
Decabot robot(4,0);
void setup() {
  Serial.begin(9600);
  robot.boot();           //initialize Decabot
}
```
The first parameter number in the object (in the example, 4) reffers to the delay time in each motor step. A safe number is 4. Higher delay will make the robot to walk slowly. Lower delay will make it run faster, but the 28BYJ-48 probabily will not run with a delay less than 3. 

The seccond parameter is a binary number, indicating to the code the Decabot configuration. To turn on a sensor, you must put 1 on the sensor position in the binary number:

1. Ultrasonic on 4,5
2. RFID on 9-13
4. Servo on A2
8. Led Matrix on 4,5,A2
16. Gyroscope on i2c
32. Laser distance sensor on i2c
64. Light sensors on A1,A3
128. PIR sensor on A2

For example, if your Decabot is installed with Ultrasonic, RFID and Gyroscope, you must turn on the bytes 1, 2 and 16:
```
|128|64|32|16|8|4|2|1
|0  |0 |0 |1 |0|0|1|1 = B00010011
```
Another example: If your Decabot is using only a frontal led matrix, your decabot declaration is:
```
Decabot robot(4,B00001000);
```
### Serial Communication
Serial.begin on setup is optional, but it will show the telemetry messages from the robot. If you want to send messages via serial, you must include the Serial input function after your loop() function. This is extremey important if you are going to control your Decabot using the Decabot App:
```
void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}
```
### Basic movement functions inside loop()
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
Decabot robot(4,B00000000); //create object Decabot with 4 millis between motor steps
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

You can send Code Domino commands through Serial, by including this serialEvent() function after the loop(). To send a program to go forward 20cm to RAM, you must type on Serial console:
```
[]F20O
```
Decabot will store the program on RAM. To execute, just type ***r*** on Serial console, and press enter. 
See the documentation for other ways to run Code Domino on your Decabot!
