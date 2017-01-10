/*
Grombass rev2.34
Made by OnkartGromt; Øyvind Mjanger 2016
Open Source but not open HW.
Code for Arduino Nano v3.0 fit to Grombass PCB GB2.3
*/

#include <EEPROM.h>

  // set pin numbers:
int CV1 = A0; //boost
int CV2 = A1; //CV expression pedal
int Input = A6; //input sound
int Output = A7; //Output sound

int PWM1 = 5; //output volume
int PWM2 = 6; //input gain 

int DIO1 = 7; //ON/Off LED
int DIO2 = 8; //BOOST LED
int DIO3 = 9; //MIDDLE LED

int RELAY = 10; // RELAY
int EXP_ON = 11; // EXP sense

int PB_0 = 2; //INT0 - footswitches actually connected to pin 2/ON/OFF
int PB_1 = 3; //INT1 - footswitches actually connected to pin 3/BOOST

//END PIN def. 

double CV1Input = 0; // variable to store the value coming from CV1
double CV2Input = 0; // variable to store the value coming from CV2

bool Toggle1 = 0; // variable to store the value coming from BP0
bool Toggle2 = 0; // variable to store the value coming from BP1
bool EXPon = 0;

int PWMGain = 150; // variable for controlling the level before the gain/clipping stage
int PWMLevel = 150; // variable for controlling the level at the output
int PWMLED = 150; // Variable for the Blue LED
int PWMGain_old; // 
int PWMLevel_old; // 
int PWMLED_old; //
int PWMGainMax = 0;
int PWMGainMin = 255;
int PWMLevelMax = 0;
int PWMLevelMin = 255;
int margin = 1;
int thresG = 120;
int thresL = 120;
int ExpRangeG = 90;
int ExpRangeL = 90;
signed int CV2InputG = 0;
signed int CV2InputL = 0;

// Variables 
int ledStateONOFF = HIGH; // the current state of the output pin
int ledStateONOFF_old = HIGH;
int buttonState0 = HIGH; // the current reading from the input pin
int lastButtonState0 = HIGH; // the previous reading from the input pin
int ledStateBOOST = HIGH; // the current state of the output pin
int ledStateBOOST_old = HIGH;
int buttonState1 = HIGH; // the current reading from the input pin
int lastButtonState1 = HIGH; // the previous reading from the input pin

int reading0_old = 0;
int reading1_old = 0;
int Tap1Timer = 0;
boolean temp = 0;

long lastDebounceTime0 = 1; // the last time the output pin was toggled
long lastDebounceTime1 = 1; // the last time the output pin was toggled
long debounceDelay = 5; // the debounce time; increase if the output flickers

void setup() {

  Serial.begin(115200);
  delay(100);

  pinMode(PB_0, INPUT);
  pinMode(PB_1, INPUT);
  pinMode(CV1, INPUT);
  pinMode(CV2, INPUT);
  pinMode(DIO1, OUTPUT);
  pinMode(DIO2, OUTPUT);
  pinMode(DIO3, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(EXP_ON, INPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);

  // set initial LED state
  ledStateONOFF = EEPROM.read(2);//Read last setting from EEPROM
  ledStateONOFF_old = ledStateONOFF;
  Serial.print("EEPROM: ");
  Serial.print(ledStateONOFF);
  ledStateBOOST = EEPROM.read(3);
  ledStateBOOST_old = ledStateBOOST;
  Serial.println(ledStateBOOST);
  digitalWrite(DIO1, ledStateONOFF);    
  digitalWrite(RELAY, ledStateONOFF);    
  digitalWrite(DIO2, ledStateBOOST);
  EXPon = digitalRead(EXP_ON);
  thresG = EEPROM.read(0);
  thresL = EEPROM.read(1);
  ExpRangeG = EEPROM.read(4);  
  ExpRangeL = EEPROM.read(5);    

  if (thresG == 0 || thresG == 255) {
    EEPROM.write(0, 37);
    thresG = 37;
  }
  if (thresL == 0 || thresL == 255) {
    EEPROM.write(1, 37);
    thresL = 37;
  }
  if (ExpRangeG == 0 || ExpRangeG == 255) {
    EEPROM.write(4, 80);
    ExpRangeG = 80;
  }
  if (ExpRangeL == 0 || ExpRangeL == 255) {
    EEPROM.write(5, 60);
    ExpRangeL = 60;
  }  
  Serial.println("Grombass v2 20170110 HW 2.34");
  //Change PWM speed:
  TCCR1B = (TCCR1B & 0b11111000) | 0x01; //Set PWM on pin 9 to 31kHz
  TCCR0B = (TCCR0B & 0b11111000) | 0x01; //Set PWM on pin 9 to 62,5kHz
}

void loop() {
  // read the state of the switch 
  int reading0 = digitalRead(PB_0); //Normally high
  int reading1 = digitalRead(PB_1);
  EXPon = digitalRead(EXP_ON);  //Active low

  // If the switch changed, due to noise or pressing:
  if (reading0 != lastButtonState0) {
    // reset the debouncing timer
    lastDebounceTime0 = millis();
  }
  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime0) > debounceDelay) {
    if (reading0 != buttonState0) {
      buttonState0 = reading0;
      // only toggle the state if the new button state is HIGH
      if (buttonState0 == LOW) {
        ledStateONOFF = !ledStateONOFF;
      }
    }
  }
  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;
      // only toggle the state if the new button state is HIGH
      if (buttonState1 == LOW) {
        ledStateBOOST = !ledStateBOOST;
      }
    }
  }

  // set the outputs:
  if (ledStateONOFF != ledStateONOFF_old){
    analogWrite(PWM1, 255);
    analogWrite(PWM2, 255);
    delay(100);
    digitalWrite(DIO1, ledStateONOFF);
    digitalWrite(RELAY, ledStateONOFF); //turn on or off effect
    Serial.println("Write to EEPROM");
    EEPROM.write(2, ledStateONOFF);
  }
   
  if (ledStateBOOST != ledStateBOOST_old){
    digitalWrite(DIO2, ledStateBOOST);
    Serial.println("Write to EEPROM");
    EEPROM.write(3, ledStateBOOST);
  }

  lastButtonState0 = reading0; //save readings to next round
  lastButtonState1 = reading1;

  //*******READ INPUTS:*************  

  CV1Input = analogRead(CV1); //boost potmeter

  //*******EXPRESSION PEDAL ACTIVE:*************   

  if (EXPon == 0) { //Expression pedal is active
    CV2Input = analogRead(CV2); //read exp pedal input
    int ExpThresholdG = thresG/2;
    int ExpThresholdL = thresL/2;   
    int usedRangeG = ExpRangeG*((255-ExpThresholdG))*0.01;
    int usedRangeL = ExpRangeL*((255-ExpThresholdL))*0.01;   
    Serial.println(usedRangeG);
    Serial.println(usedRangeL);
    CV2InputG = map(CV2Input, 0, 1023, 0, 2*usedRangeG); 
    CV2InputL = map(CV2Input, 0, 1023, 0, 2*usedRangeL);
     
    if (ledStateBOOST) { //boost active
      int PWMGainBoost = map(CV1Input, 0, 1024, 0, ExpThresholdG);
      int PWMLevelBoost = map(CV1Input, 0, 1024, 0, ExpThresholdL);      
      PWMGain = ExpThresholdG + usedRangeG - CV2InputG-PWMGainBoost;
      PWMLevel = ExpThresholdL + usedRangeL + CV2InputL-PWMLevelBoost; 
    } 
    else {
      PWMGain = ExpThresholdG + usedRangeG - CV2InputG;
      PWMLevel = ExpThresholdL + usedRangeL + CV2InputL; 
    }
    PWMLED = (PWMGain);
  }
  //********NO EXP:PEDAL:************     
  else {
    if (ledStateBOOST) { //Boost is active
      PWMLevel = map(CV1Input, 0, 1023, thresL, 0); // mapped to 0-thresL dependent of the boost potmeter value
      PWMGain = map(CV1Input, 0, 1023, thresG, 0); //Also adjust the gain stage
    } 
    else { // When boost is not activated
      PWMLevel = thresL;
      PWMGain = thresG;
    }

    PWMLED = 185 - PWMLevel; //Blue indicator LED
  }
  //************Change thershold value in EEPROM when both buttons are hold long

  if ((!reading1 && (reading1 == reading1_old))&&(EXPon == 1)) {//Hold right button and exp pedal is not connected
    Serial.print("Set Level: ");
    Serial.println(Tap1Timer);
    Tap1Timer++;
    if (Tap1Timer > 300) {
      temp = !temp;
      thresL = analogRead(CV1);
      thresL = map(thresL, 0, 1023, 255, 0);
      delay(5000);
      analogWrite(DIO1, temp * 255);
      analogWrite(DIO2, temp * 255);
      analogWrite(DIO2, temp * 255);
      PWMLevel = thresL;
      PWMGain = thresG;
      ledStateONOFF = HIGH;
    }
  } else if ((!reading0 && (reading0 == reading0_old))&&(EXPon == 1)) {//Hold left button and exp pedal is not connected
    Serial.print("Set Gain: ");
    Serial.println(Tap1Timer);
    Tap1Timer++;
    if (Tap1Timer > 300) {
      temp = !temp;
      thresG = analogRead(CV1);
      thresG = map(thresG, 0, 1023, 255, 0);
      delay(5000);
      analogWrite(DIO1, temp * 255);
      analogWrite(DIO2, temp * 255);
      analogWrite(DIO2, temp * 255);
      PWMLevel = thresL;
      PWMGain = thresG;
      ledStateONOFF = HIGH;
    }
  } else if ((!reading0 && (reading0 == reading0_old))&&(EXPon == 0)) {  //Hold left button and exp pedal is connected
    Serial.print("Set rangeG: ");
    Serial.println(Tap1Timer);
    Tap1Timer++;
    if (Tap1Timer > 300) {
      temp = !temp;
      ExpRangeG = analogRead(CV1);
      ExpRangeG = map(ExpRangeG, 0, 1023, 1, 100);
      delay(5000);
      analogWrite(DIO1, temp * 255);
      analogWrite(DIO2, temp * 255);
      analogWrite(DIO2, temp * 255);
      PWMLevel = thresL;
      PWMGain = thresG;
      ledStateONOFF = HIGH;
    }
  }
    else if ((!reading1 && (reading1 == reading1_old))&&(EXPon == 0)) {  //Hold right button and exp pedal is connected
    Serial.print("Set rangeL: ");
    Serial.println(Tap1Timer);
    Tap1Timer++;
    if (Tap1Timer > 300) {
      temp = !temp;
      ExpRangeL = analogRead(CV1);
      ExpRangeL = map(ExpRangeL, 0, 1023, 1, 100);
      delay(5000);
      analogWrite(DIO1, temp * 255);
      analogWrite(DIO2, temp * 255);
      analogWrite(DIO2, temp * 255);
      PWMLevel = thresL;
      PWMGain = thresG;
      ledStateONOFF = HIGH;
    }
  }  
  else if ((Tap1Timer > 75) && thresG != EEPROM.read(0)) { //Write to EEPROM once when button is released
    EEPROM.write(0, thresG);
    Serial.print("thresGhold: ");
    Serial.println(thresG);
  } else if ((Tap1Timer > 75) && thresL != EEPROM.read(1)) { //Write to EEPROM once when button is released
    EEPROM.write(1, thresL);
    Serial.print("thresLhold: ");
    Serial.println(thresL);
  } else if ((Tap1Timer > 75) && ExpRangeG != EEPROM.read(4)) { //Write to EEPROM once when button is released
    EEPROM.write(4, ExpRangeG);
    Serial.print("ExpRangeG: ");
    Serial.println(ExpRangeG);
  } else if ((Tap1Timer > 75) && ExpRangeL != EEPROM.read(5)) { //Write to EEPROM once when button is released
    EEPROM.write(5, ExpRangeL);
    Serial.print("ExpRangeL: ");
    Serial.println(ExpRangeL);
  }
  else if ((Tap1Timer > 75) && thresL == EEPROM.read(1) && thresG == EEPROM.read(0)) Tap1Timer = 0;

  //***********WRITE TO OUTPUTS**only when change has occoured*************

  PWMGain = constrain(PWMGain,0,255);
  PWMLevel = constrain(PWMLevel,0,255);
  analogWrite(PWM1, PWMGain);
  PWMGain_old = PWMGain;
  analogWrite(PWM2, PWMLevel);
  PWMLevel_old = PWMLevel;
  analogWrite(DIO3, PWMLED);
  PWMLED_old = PWMLED;

  reading0_old = reading0;
  reading1_old = reading1;
  ledStateONOFF_old = ledStateONOFF;
  ledStateBOOST_old = ledStateBOOST;

  //*********Serial printout********************************************

  Serial.print(" PWMGain: ");
  Serial.print(PWMGain);
  Serial.print(" PWMLevel: ");
  Serial.print(PWMLevel);
  Serial.print(" thresG: ");
  Serial.print(thresG);
  Serial.print(" thresL: ");
  Serial.print(thresL);
  Serial.print(" CV2Input: ");
  Serial.print(CV2InputG);
  Serial.print(" ONOFF: ");
  Serial.print(ledStateONOFF);
  Serial.print(" BOOST: ");
  Serial.print(ledStateBOOST);  
  Serial.print(" ExpRangeG: ");
  Serial.print(ExpRangeG);
  Serial.print(" ExpRangeL: ");
  Serial.print(ExpRangeL);  
  Serial.print(" EXPon: ");
  Serial.println(EXPon);
}
