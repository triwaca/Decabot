/*
 * Configure Decabot
 * www.decabot.com
 *  
 * Shows how to format EEPROM, configure name and owner, and setup sensors and actuators.
 * 
 * To configure sensors, you must change the second byte parameter of Decabot constructor:
 *
 *     B    - Indicates it's a byte
 *   0 or 1 - PIR sensor on A2
 *   0 or 1 - OLED on i2c
 *   0 or 1 - Laser distance sensor on i2c
 *   0 or 1 - Gyroscope on i2c
 *   0 or 1 - Led Matrix on 4,5,A2
 *   0 or 1 - Servo on A2
 *   0 or 1 - RFID on 9-13
 *   0 or 1 - Ultrasonic on 4,5
 *      \
 *       \___ B00000101 means Decabot with Servo and Ultrasonic sensor
 *           
 */

#include <Decabot.h>

Decabot robot(3,B00000000); //create object Decabot with 3 millis between motor steps, and no sensors

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
