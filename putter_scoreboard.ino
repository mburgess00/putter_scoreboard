/*
 Arduino pin 6 -> CLK (Green on the 6-pin cable)
 5 -> LAT (Blue)
 7 -> SER on the IN side (Yellow)
 5V -> 5V (Orange)
 Power Arduino with 12V and connect to Vin -> 12V (Red)
 GND -> GND (Black)

 There are two connectors on the Large Digit Driver. 'IN' is the input side that should be connected to
 your microcontroller (the Arduino). 'OUT' is the output side that should be connected to the 'IN' of addtional
 digits.

 Each display will use about 150mA with all segments and decimal point on.

*/

#include <EEPROM.h>

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;

//Remote receiver
byte aPin = 10;
byte bPin = 11;
byte cPin = 12;
byte dPin = 13;

//Buzzer
byte buzzer = 8;

//Reset button
byte resetButton = 9;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned long previousMillis = 0;
boolean timerRunning = false;
boolean currentGame = false;
int timer = 30;
int score = 0;
int highscore = 0;
int currentButton = 0; //none=0, A=1, B=2, C=3, D=4, reset=-1
int previousButton =0;
unsigned long presstime = 0;

const int eeAddress = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Putter Scoreboard");

  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);

  //remote receiver
  pinMode(aPin, INPUT);
  pinMode(bPin, INPUT);
  pinMode(cPin, INPUT);
  pinMode(dPin, INPUT);

  //buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  delay(250);
  digitalWrite(buzzer, LOW);

  //reset
  pinMode(resetButton, INPUT_PULLUP);

  //read highscore from eeprom
  EEPROM.get(eeAddress, highscore);
  if ((highscore > 200) || (highscore < 0))
  {
    highscore = 0;
  }
}


void loop()
{
  //handle buttons
  if (digitalRead(aPin) == HIGH)
  
  if (digitalRead(resetButton) == LOW)
  {
    if (previousButton != -1)
    {
      presstime = millis();
    }
    else if (millis() - presstime > 2000)
    {
      highscore = 0;
      //write 0 to eeprom 
    }
    currentButton = -1;
  }
  
  if timerRunning
  {
    if (digitalRead(aPin) == HIGH)
    {
      timerRunning = false;
    }
    if (millis() - previousMillis >= 1000)
    {
      //increment timer
      timer -= 1;
      previousMillis = millis();
      updateDisplay(timer, score);
      if (timer == 0)
      {
        //end of game logic
        currentGame = false;
      }
    }
  }
  else
  {
    if (digitalRead(aPin) == HIGH)
    {
      timerRunning = true;
      previousMillis = millis();
      if currentGame
      {
        
      }
    }
  }
}

void updateDisplay(int curtime, int curscore)
{
  showNumber(curscore);
  showNumber(curtime);
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value)
{
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

