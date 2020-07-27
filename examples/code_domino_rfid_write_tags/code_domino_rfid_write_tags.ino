/*
 * Decabot RFID recorder
 * 
 * Creates Code Dominó pieces recording data on RFID tags.
 * Works with mftc522 connected to the Decabot.  
 * 
 * Usage:
 * 1 - Load the sketch on Decabot
 * 2 - Type the CD command to save on TAG (Eg: F to forward piece)
 * 3 - Put the piece on mfrc522 and wait for the OK feedback. Do it as many pieces you have.
 * 4 - Change the code using *. Eg: *R will change the code to write to R (right).
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

String codeToWrite;
byte buffer[34];

//ledDisplay const
int ledLatchPin = 16;  //Pin connected to Max7219 module CS
int ledClockPin = 4;  //Pin connected to Max7219 module CLK
int ledDataPin = 5;   //Pin connected to Max7219 module DIN
int buzzerPin = 3;

// the MAX7219 address map (datasheet table 2)
#define MAX7219_DECODE_REG      (0x09)
#define MAX7219_INTENSITY_REG   (0x09)
#define MAX7219_SCANLIMIT_REG   (0x0B)
#define MAX7219_SHUTDOWN_REG    (0X0C)
#define MAX7219_DISPLAYTEST_REG (0x0F)
#define MAX7219_DIGIT_REG(pos)  ((pos) + 1)
#define MAX7219_COLUMN_REG(pos) MAX7219_DIGIT_REG(pos)

// shutdown mode (datasheet table 3)
#define MAX7219_OFF             (0x0)
#define MAX7219_ON              (0x1)


// number of columns of the display matrx
#define NUM_OF_COLUMNS  (8)
// for each character bitmap, it consumes 4 bytes
#define BYTE_PER_MAP    (4)

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  pinMode(ledLatchPin, OUTPUT); 
  pinMode(ledClockPin, OUTPUT);   
  pinMode(ledDataPin, OUTPUT);
  ledMatrixInit();
  Serial.println(F("Code Domino RFID writer"));
  Serial.println(F("Type the code to write on RFID tags, and put the tan on reader/writer."));
  Serial.println(F("(Max. 64 commands/parameters)"));
  ledsReading();
}

void loop() {
  ledsReading();
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  
  byte block;
  MFRC522::StatusCode status;
  byte len;

  Serial.println(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    ledsCard();
  }
  if(mfrc522.uid.size == 4){
    Serial.println("1k tag detected");
    block = 1;
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      ledsError();
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(2000);
      return;
    }
    else Serial.println(F("PCD_Authenticate() success: "));
    
    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      ledsError();
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(2000);
      return;
    } 
    else Serial.println(F("MIFARE_Write() success block 1 "));

    block = 2;
    //Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      ledsError();
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(2000);
      return;
    }
  
    // Write block
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
      ledsError();
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(2000);
      return;
    }
    else Serial.println(F("MIFARE_Write() success: block 2"));
    ledsCorrect();
    delay(2000);
  } else {
    if(mfrc522.uid.size == 7){
      Serial.println("Ultralight tag detected");
      
      block = 5;
      for (int i=0; i < 8; i++) {
        //data is writen in blocks of 4 bytes (4 bytes per page)
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Ultralight_Write(block+i, &buffer[i*4], 4);
        if (status != MFRC522::STATUS_OK) {
          ledsError();
          Serial.print(F("MIFARE_Read() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          delay(2000);
          return;
        } else {
          Serial.print(F("MIFARE_Ultralight_Write() block "));
          Serial.print(block+i);
          Serial.println(" OK");
        }
      }
      ledsCorrect();
      delay(2000);
    } else {
      ledsError();
      Serial.println("Error! Unknow RFID tag");
      delay(2000);
      return;
    }
  }
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}

void serialEvent() {      //allows the robot to receive data from Serial
  while (Serial.available()) {
    char tempChar = (char)Serial.read();
    if(tempChar == '?'){
      break;
    }
    if(tempChar == '*'){
      codeToWrite = "";
    } else {
      if(isAlphaNumeric(tempChar)||tempChar=='['||tempChar==']'||tempChar=='?'||tempChar=='#'||tempChar=='$'||tempChar=='('||tempChar==')'){
        codeToWrite += tempChar;
      }
    }
  }
  Serial.print("Code to write on RFID's: ");
  for(int i=0;i<32;i++){
    if(i<codeToWrite.length()){
      buffer[i] = codeToWrite[i];
    } else {
      buffer[i] = ' ';
    }
    Serial.print((char)buffer[i]);
  }
  Serial.println("");
  Serial.println("(Type * to clear)");
  delay(500);
}

void ledMatrixInit(){
  Serial.println(F("Initializing led matrix..."));
  // disable test mode. datasheet table 10
  ledMatrixSetRegister(MAX7219_DISPLAYTEST_REG, MAX7219_OFF);
  // set medium intensity. datasheet table 7
  ledMatrixSetRegister(MAX7219_INTENSITY_REG, 0x1);
  // turn off display. datasheet table 3
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF);
  // drive 8 digits. datasheet table 8
  ledMatrixSetRegister(MAX7219_SCANLIMIT_REG, 7);
  // no decode mode for all positions. datasheet table 4
  ledMatrixSetRegister(MAX7219_DECODE_REG, B00000000);

  // clear matrix display
  ledMatrixClear();
}

void ledMatrixSetRegister(byte address, byte value){
  digitalWrite(ledLatchPin, LOW);
  shiftOut(ledDataPin, ledClockPin, MSBFIRST, address);
  shiftOut(ledDataPin, ledClockPin, MSBFIRST, value);
  digitalWrite(ledLatchPin, HIGH);
}

void ledMatrixClear(){
  // clear the dot matrix
  for (int i = 0; i < NUM_OF_COLUMNS; i++)
  {
    ledMatrixSetRegister(MAX7219_COLUMN_REG(i), B00000000);
  }
}

void ledsError(){
  ledMatrixClear();
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
  ledMatrixSetRegister(2,B01100110);
  ledMatrixSetRegister(3,B00111100);
  ledMatrixSetRegister(4,B00011000);
  ledMatrixSetRegister(5,B00011000);
  ledMatrixSetRegister(6,B00111100);
  ledMatrixSetRegister(7,B01100110);
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
}

void ledsReading(){
  ledMatrixClear();
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
  ledMatrixSetRegister(1,B00011000);
  ledMatrixSetRegister(2,B00000000);
  ledMatrixSetRegister(3,B00100100);
  ledMatrixSetRegister(4,B00011000);
  ledMatrixSetRegister(5,B01000010);
  ledMatrixSetRegister(6,B00111100);
  ledMatrixSetRegister(7,B10000001);
  ledMatrixSetRegister(8,B01111110);
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
}

void ledsCard(){
  ledMatrixClear();
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
  ledMatrixSetRegister(1,B00001100);
  ledMatrixSetRegister(2,B00000010);
  ledMatrixSetRegister(3,B00001101);
  ledMatrixSetRegister(4,B01110101);
  ledMatrixSetRegister(5,B10001000);
  ledMatrixSetRegister(6,B10001000);
  ledMatrixSetRegister(7,B10001000);
  ledMatrixSetRegister(8,B01110000);
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
}

void ledsCorrect(){
  tone(buzzerPin,880);
  delay(50);
  noTone(buzzerPin);
  ledMatrixClear();
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_OFF); //turn off
  ledMatrixSetRegister(2,B00000001);
  ledMatrixSetRegister(3,B00000110);
  ledMatrixSetRegister(4,B01001100);
  ledMatrixSetRegister(5,B11111000);
  ledMatrixSetRegister(6,B01110000);
  ledMatrixSetRegister(7,B00100000);
  ledMatrixSetRegister(MAX7219_SHUTDOWN_REG, MAX7219_ON); //turn on
}
