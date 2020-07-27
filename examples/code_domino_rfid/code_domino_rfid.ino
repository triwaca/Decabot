/*
 *  Decabot with RFID Code Domino interpreter 
 *  
 */

//libraries
#include <Decabot.h>
#include <SPI.h>
#include <MFRC522.h>

//pins
int sck = 13; //SPI
int miso = 12; //SPI
int mosi = 11; //SPI
int resetPin = 9;
int ssPin = 10;

//variables
byte rfidBuffer[18];      //data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(rfidBuffer);
uint8_t pageAddr = 4;  //0x06 for mifare ultra

//objects
Decabot robot(4,B00001010); //create object Decabot with RFID and Led Matrix 
MFRC522 mfrc522(ssPin, resetPin);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status; //variável para pegar status do cartão

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  robot.boot();
}

void loop() {
  robot.update(); //update state of Code Domino machine
  if(millis()%1000==0){
    //read RFID each second
    readRfid();
  }
}

void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    robot.inputSerial((char)Serial.read());
  }
}

void readRfid() {
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  robot.beep(50);
  String readData = "";
  if(mfrc522.uid.size == 4){
    //Serial.println("Mifare 1K tag detected");
    //read 32 bytes on 16B blocks 1 and 2
    for(int i=1;i<3;i++){
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, i, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(i, rfidBuffer, &size);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failiure!"));
        Serial.println(mfrc522.GetStatusCodeName(status));
        robot.abort();
        return;
      }
      //throw data to string readData
      for(int i=0;i<16;i++){
        if (rfidBuffer[i] != 32){
          readData.concat((char)rfidBuffer[i]);
        }
        rfidBuffer[i] = ' ';
      }
    }
  }
  if(mfrc522.uid.size == 7){
      //Serial.println("Mifare Ultralight tag detected");
      //read 32 bytes on 4B blocks 5, 6, 7, 8, 9, 10, 11 and 12
      for (int i=0; i < 8; i++) {
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(5+i, rfidBuffer, &size);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Read() failiure: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          robot.abort();
          return;
        }
        //throw data to string readData
        for(int i=0;i<4;i++){
          if (rfidBuffer[i] != 32){
            readData.concat((char)rfidBuffer[i]);
          }
          rfidBuffer[i] = ' ';
        }
      }
  }
  if(readData.substring(0,1)=="("){
    robot.injectRFIDposition(readData);
  } else {
    robot.injectRFID(readData);
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
