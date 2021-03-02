/*  
 Black Box 
 A MATLAB Lab
 
 Reuben Strangelove
 2013
 reuben@artofmystate.com 
 */

// Buffer to store incoming commands from serial port
String inData;

#include <Timer.h>
Timer t;

#include <Servo.h> 
Servo doorservo, lockservo;

#include <EEPROM.h>

byte SERVO_DOOR_CLOSE_POS = 175;
byte SERVO_DOOR_OPEN_POS = 107;

byte SERVO_LOCK_CLOSE_POS = 173;
byte SERVO_LOCK_OPEN_POS = 150;

byte SERVO_DOOR_PIN = 19;
byte SERVO_LOCK_PIN = 18;

byte LED_CODE_OUT_HIGH = 3;
byte LED_CODE_OUT_MED = 8;

byte BUTTON_PIN = 16;


byte CODE_ENTRIES = 8;
const char* TERMINATOR = "#";

const char* CODE[] = {
  "Newton", "Turing", "Nobel", "Einstein", "Hawking", "Tesla", "Pascal", "Aristotle" };

// note: passcode generated by MATLAB passcode generation code
const char* PASSCODE[] = {
  "0110100001101001011011100101111100100011",
  "0110001001101001011001000110110100100011",
  "01100111011000000101110100100011",
  "01010111010100110110000101010111001100110010",
  "01100010011010010110010001100110010111000010",
  "01011011011001100101111100100011",  
  "0110011101011100010111100101110000100011",
  "01010011010110100101110101100001010101110010"};

char codeToSend[20]; //make the array long enough to accommodate all the characters in the CODE strings 
char passcode[50];

byte brightness;
byte brightnessNormalized;
byte brightnessBuffer;
byte openDoorCounter = 0;


byte bitValue;
byte i_char = 0;
byte i_bit = 0;
byte clockToggle = 0;
byte newlineFlag = 0;

byte terminatorFlag = 0;
byte terminatorCount = 0;

byte openFlag = 0;
byte closeFlag = 0;

byte i_decode = 0;
byte dataBuffer[64];
int correctCounter;
byte passcodeLength = 0;

byte setPassCode;

byte passcodeIndex;

void setup() {

  // setup servo pins then close and lock the door
  lockservo.attach(SERVO_LOCK_PIN); 
  lockservo.write(SERVO_LOCK_OPEN_POS); 
  delay(350);
  doorservo.attach(SERVO_DOOR_PIN);   
  doorservo.write(SERVO_DOOR_CLOSE_POS); 
  delay(350); 
  lockservo.write(SERVO_LOCK_CLOSE_POS); 

  // initialize serial:
  Serial.begin(57600);
  
  Serial.print("Black Box Starting Up"); 

  pinMode(LED_CODE_OUT_HIGH, OUTPUT);
  pinMode(LED_CODE_OUT_MED, OUTPUT); 
  pinMode(BUTTON_PIN, INPUT);  

  // open the door without inputing a code when the light sensor is blacked out and the button is held
  if (digitalRead(BUTTON_PIN) == 0 && map(analogRead(0), 0, 1023, 0, 255) < 10) { 

    // set brightness led to medium for sensor calibration 
    digitalWrite(LED_CODE_OUT_HIGH, LOW);
    digitalWrite(LED_CODE_OUT_MED, HIGH);

    openDoor();    

    while(digitalRead(BUTTON_PIN) == 0) {   
    } 

    delay(100); 

    while(digitalRead(BUTTON_PIN) == 1)  { 
    }

    closeDoor();

  }

  // set passcode
  setPasscode();

  // activate timer events
  t.every(25, updateCodeLED); // update code transmitting LED every x milliseconds
  t.every(10, passcodeCheck); 

} // end setup()


void setPasscode() {

  // read value set in EEPROM; 
  passcodeIndex = EEPROM.read(0);

  Serial.print("Passcode set to number: "); 
  Serial.println(passcodeIndex, DEC); 

  // copy CODE string to codeToSend 
  strcpy(codeToSend, CODE[passcodeIndex]);
  // add terminator to the end of codeToSend
  strcat(codeToSend, TERMINATOR);

  // copy CODE string to codeToSend 
  strcpy(passcode, PASSCODE[passcodeIndex]);
  // add terminator to the end of codeToSend
  strcat(passcode, TERMINATOR);

  // manually find passcode length
  for (int i = 0; i <= 255; i++){       
    //Serial.print(passcode[i]);
    if (passcode[i] != '#') {     
      passcodeLength++;     
    }   
    else {    
      break;     
    }      
  }

} // end setPasscode


void openDoor() {

  lockservo.write(SERVO_LOCK_OPEN_POS);  
  delay(200);
  doorservo.write(SERVO_DOOR_OPEN_POS); 

} // end openDoor()


void closeDoor() {

  doorservo.write(SERVO_DOOR_CLOSE_POS);
  delay(200);     
  lockservo.write(SERVO_LOCK_CLOSE_POS);  

} // end closeDoor()


void passcodeCheck() {

  // read brightness and scale from an int to to a byte
  brightness = map(analogRead(0), 0, 1023, 0, 255);

  // normalize brightness into binary and clock equivalents
  if (brightness > 0 && brightness < 50) brightnessNormalized = 0;
  if (brightness > 49 && brightness < 200) brightnessNormalized = 1;
  if (brightness > 199 && brightness < 255) brightnessNormalized = 2;

  // if there is a new value execute the following code
  if (brightnessNormalized != brightnessBuffer) {

    // save brightness data into the buffer for later comparison
    brightnessBuffer = brightnessNormalized;

    // when a clock signal is detected return
    if (brightnessNormalized == 2) return;

    // debug output
    //Serial.print("B - "); 
    //Serial.print(brightnessNormalized, DEC);  
    //Serial.print(" - "); 

    // store brightness value in the data buffer
    dataBuffer[passcodeLength + 1] = brightnessNormalized;

    // scan entire buffer for a matching passcode
    correctCounter = 0;    
    for (int i = 0; i <= passcodeLength; i++){

      // count macthing data
      if (dataBuffer[i] == (passcode[i] - 48)) correctCounter++;

      // debug output
      //Serial.print(dataBuffer[i], DEC); 
      //Serial.print("-");  
      //Serial.print(passcode[i] - 48, DEC); 
      //Serial.print("|");  

    } 

    // debug output
    //Serial.println(" "); 
    //Serial.print("C - "); 
    //Serial.println(correctCounter, DEC);  

    // check correct count to passcode length
    if (correctCounter == passcodeLength) {

      Serial.println("CODE FOUND");  

      digitalWrite(LED_CODE_OUT_HIGH, LOW);
      digitalWrite(LED_CODE_OUT_MED, LOW); 

      openDoor();

      // blink LED and wait for button input
      while(1){ 

        digitalWrite(LED_CODE_OUT_HIGH, LOW); 
        delay(250);
        digitalWrite(LED_CODE_OUT_HIGH, HIGH); 
        delay(250);

        if (digitalRead(BUTTON_PIN) == 0)
        {
          closeDoor();
          break;
        } 
      }

    }

    //shift dataBuffer over by one
    for (int i = 0; i <= passcodeLength; i++){

      dataBuffer[i] = dataBuffer[i + 1];
      Serial.print(dataBuffer[i], DEC);  

    } 

    Serial.println(" ");

  } // end new value

} // end passcodeCheck()


void loop() {

  // code required for timers
  t.update();

  //check for available serial data
  while (Serial.available() > 0)
  {
    char received = Serial.read();
    inData += received; 

    // Process message when new line character is recieved
    if (received == '\n')
    {
      Serial.print("Data Received: ");
      Serial.print(inData);

      // process commands
      if (inData == "bb open\n") openDoor();
      if (inData == "bb close\n") closeDoor();
      if (inData == "bb passcode\n") {
        Serial.print("Current Passcode: ");  
        Serial.print(passcodeIndex);
        Serial.print(" - ");  
        Serial.println(codeToSend);

      }

      setPassCode = 255;
      if (inData == "bb set passcode 0\n") setPassCode = 0;
      if (inData == "bb set passcode 1\n") setPassCode = 1;
      if (inData == "bb set passcode 2\n") setPassCode = 2;
      if (inData == "bb set passcode 3\n") setPassCode = 3;
      if (inData == "bb set passcode 4\n") setPassCode = 4;
      if (inData == "bb set passcode 5\n") setPassCode = 5;
      if (inData == "bb set passcode 6\n") setPassCode = 6;
      if (inData == "bb set passcode 7\n") setPassCode = 7;

      if (setPassCode != 255) {

        EEPROM.write(0, setPassCode);

        Serial.println("Resetting Arduino");  
         Serial.println(" ");       
        delay(500);       
        // reset Arduino; new passcode will take effect
        asm volatile ("  jmp 0");

      }

      inData = ""; // Clear recieved buffer
    }
  }

} // end loop()


void updateCodeLED()
{

  if (clockToggle == 0) {

    clockToggle = 1; 

    i_bit = i_bit + 1; 

    if (i_bit == 7) newlineFlag = 1;

    if (i_bit == 8) {

      i_bit = 0;  

      i_char = i_char + 1;

      // reach end of string indicated by null terminator    
      if (codeToSend[i_char] == 0) i_char = 0; 

      // debug output
      //Serial.println(codeToSend[i_char]); 

    } // end if (i_bit > 7) 

    bitValue = bitRead(codeToSend[i_char], 7 - i_bit);

    // write brightness values          
    digitalWrite(LED_CODE_OUT_HIGH, LOW); // turn off clock
    if (bitValue == 0) digitalWrite(LED_CODE_OUT_MED, LOW); 
    if (bitValue == 1) digitalWrite(LED_CODE_OUT_MED, HIGH); 

    // debug output
    //if (i_bit == 7)
    //  Serial.println(bitValue);          
    // else Serial.print(bitValue); 

  } 

  else   

  {
    clockToggle = 0;

    digitalWrite(LED_CODE_OUT_HIGH, HIGH);
    digitalWrite(LED_CODE_OUT_MED, LOW);

  } // end if clockToggle == 0

} // end updateCodeLED
