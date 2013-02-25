#include <LiquidCrystal.h>

/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

// include the library code:
#include <LiquidCrystal.h>
#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };
  
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// IO Pin Assignments.
// Button for backlight and reset
const int iBtnBklt_Rst = 6;
const int iBtnStrt_Stp = 7;
const int oPinBklt = 13;
// Backlight 
boolean bStateBklt = false;
int stateBtnBKLT;
int BKLT_ON;
int HOLD_TIME;
// Time adjustment buttons
const int iBtnUp = 9;
const int iBtnDown = 10;
int stateBtnUP;
int stateBtnDOWN;
// Timing Variables
unsigned long cPreviousSeconds;
int cIntervals = 1;
// Buzzer
int BuzzerIteration = 0;
int Buzzer;
// Start and Stop Button
boolean bStartStop = false;
int stateBtnStartStop;
int antiCYCLE; 
int lastantiCYCLE;
// Adjustable Variables
int cTIME = 60; // time in seconds. Everything is based on this.
int cHiTIME = 100; // tracks cTIME start
int HOLD_DELAY = 3; // time in seconds
int BKLT_ON_DURATION = 18; // time in seconds adds the time from when the button was pressed.


void setup() {
  // serial debugging.
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Time:");
  // IO States
  pinMode(iBtnBklt_Rst, INPUT);
  pinMode(iBtnStrt_Stp, INPUT);
  pinMode(iBtnUp, INPUT);
  pinMode(iBtnDown, INPUT);
  pinMode(oPinBklt, OUTPUT);
}

void loop() {
  // Time Display
  int cHOURS = (cTIME/60)/60;
  int cMINUTEStenths = ((cTIME/60) % 60)/10;
  int cMINUTESones = ((cTIME/60) % 60) % 10;
  int cSECONDStenths = (((cTIME%60)%60) % 60)/10;
  int cSECONDSones = (((cTIME%60)%60) % 60) % 10;
  // base clock for timing everything
  unsigned long cSeconds = millis()/1000;
  
  // read the state of the pushbutton values;
  stateBtnBKLT = digitalRead(iBtnBklt_Rst);
  stateBtnStartStop = digitalRead(iBtnStrt_Stp);
  stateBtnUP = digitalRead(iBtnUp);
  stateBtnDOWN = digitalRead(iBtnDown);
  
  if (stateBtnStartStop != antiCYCLE){
    if (stateBtnStartStop == HIGH) {
    bStartStop = !bStartStop;
    }
  }
  
  antiCYCLE = stateBtnStartStop;
  
  Serial.println(bStartStop);
  
  if (bStartStop == false && cTIME == cHiTIME){
    digitalWrite(oPinBklt, HIGH);
  }
  
  if (stateBtnUP == HIGH && cTIME == cHiTIME && bStartStop == false && (cSeconds - cPreviousSeconds <= cIntervals)){
    cTIME += 1;
    cHiTIME += 1;
  }
  
  if (stateBtnDOWN == HIGH && cTIME == cHiTIME && bStartStop == false && (cSeconds - cPreviousSeconds <= cIntervals) && cTIME > 0){
    cTIME -= 1;
    cHiTIME -= 1;
  }
  
  // Resets the timer back to the previous set time.
  if (stateBtnBKLT == HIGH && bStartStop == false && (cSeconds - cPreviousSeconds <= cIntervals)){
    cTIME = cHiTIME;
  }
  
  // Holding down the reset button for 3 seconds will turn on the lcd backlight.
  if (stateBtnBKLT == HIGH && (cSeconds - cPreviousSeconds >= cIntervals)) {
    HOLD_TIME++;
    if (HOLD_TIME == HOLD_DELAY){
    bStateBklt = true;
    digitalWrite(oPinBklt, HIGH);
    HOLD_TIME = 0;
    }
  }
  
  // Turns on the backlight
  if (bStateBklt = true && (cSeconds - cPreviousSeconds >= cIntervals) && bStartStop == true){
    BKLT_ON++;
    if (BKLT_ON == BKLT_ON_DURATION){
      digitalWrite(oPinBklt, LOW);
      BKLT_ON = 0;
      bStateBklt = false;
    }
  }
    
  
  //As the clock time goes up, cTime will increment down.
  if (cSeconds - cPreviousSeconds >= cIntervals && bStartStop == true){
    cSeconds;
    cTIME--;
  }
  
  if (cTIME <= 0 && bStartStop == true){
    cTIME = 0;
  }
  
  cPreviousSeconds = cSeconds;
  
   // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(cHOURS);
  lcd.print(":");
  lcd.print(cMINUTEStenths);
  lcd.print(cMINUTESones);
  lcd.print(":");
  lcd.print(cSECONDStenths);
  lcd.print(cSECONDSones);
  
  if (cTIME == 0 && Buzzer <= BuzzerIteration && bStartStop == true)  {
    for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
  Buzzer++;
}
}

