#include <Decabot.h>

Decabot robot(9600); //create object Decabot

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  robot.boot();
}

void loop() {
  // put your main code here, to run repeatedly:
  robot.output("ok");
  delay(2000);
}
