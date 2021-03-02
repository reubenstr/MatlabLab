/*

Sensor system for Black Box 
 
PWM Lib : https://code.google.com/p/arduino-pwm-frequency-library/downloads/list
PWM Lib Forum Link : http://forum.arduino.cc/index.php/topic,117425.0.html
 
 
*/


// declare hardware pins of peripherals
const byte LED_MIRROR = 11;
const byte LED_CODE_OUT_HIGH = 3; 
const byte LED_CODE_OUT_MED = 8;
const byte LIGHT_SENSOR = 0;

byte brightness;


void setup() {
  
  // initialize serial:
  Serial.begin(57600);

  // initialize pins for outputs
  pinMode(LED_MIRROR, OUTPUT);
  pinMode(LED_CODE_OUT_HIGH, OUTPUT);
  pinMode(LED_CODE_OUT_MED, OUTPUT);
  
} // end setup


void loop() {

  /*
  while(1) {
    
   
   delay(250);
    digitalWrite(LED_CODE_OUT_MED, HIGH);
   digitalWrite(LED_CODE_OUT_HIGH, LOW); 
    delay(250);
    digitalWrite(LED_CODE_OUT_HIGH, HIGH); 
    digitalWrite(LED_CODE_OUT_MED, LOW);
  }
  */
  
  // read brightness from light sensor and scale the int to a btye
  brightness = map(analogRead(LIGHT_SENSOR), 0, 1023, 0, 255);

  // send brightness data out the serial port with a terminator
  Serial.println(brightness , DEC);   

  // replicate detected brightness on output led for user verification
  analogWrite(LED_MIRROR, brightness);     

  // delay reading sensor to minimize amount of sent raw data
  delay(10);

} // end loop()


void serialEvent() {
  
  while (Serial.available()) {

    char inChar = (char)Serial.read(); 

    digitalWrite(LED_CODE_OUT_MED, LOW);
    digitalWrite(LED_CODE_OUT_HIGH, LOW);
    
    if (inChar == '0') { /* do nothing */ }
    if (inChar == '1') digitalWrite(LED_CODE_OUT_MED, HIGH);
    if (inChar == '2') digitalWrite(LED_CODE_OUT_HIGH, HIGH);

  } // end while
  
} // end serialEvent()



