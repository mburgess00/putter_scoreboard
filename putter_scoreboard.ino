
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
byte aPin = 13;
byte bPin = 12;
byte cPin = 11;
byte dPin = 10;

//Buzzer
byte buzzer = 8;

//Reset button
byte resetButton = 9;

const int timerLength = 30;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//for remote control, set this to HIGH
//for switches, set this to LOW
boolean on = HIGH;


unsigned long previousMillis = 0;
unsigned long gameEndMillis = 0;
boolean timerRunning = false;
boolean showingHighScore = false;
int timer = timerLength + 1;
int score = 0;
int highscore = 0;
int currentButton = 0; //none=0, A=1, B=2, C=3, D=4, reset=-1
int previousButton = 0;
unsigned long pressTime = 0;

const int eeAddress = 0;

boolean noDecimals[4] = {false, false, false, false};

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

  if (on == LOW)
  {
    pinMode(aPin, INPUT_PULLUP);
    pinMode(bPin, INPUT_PULLUP);
    pinMode(cPin, INPUT_PULLUP);
    pinMode(dPin, INPUT_PULLUP);
  }

  //buzzer
  pinMode(buzzer, OUTPUT);
  //digitalWrite(buzzer, HIGH);
  //delay(250);
  //digitalWrite(buzzer, LOW);
  tone(buzzer, 1200, 250);

  //reset
  pinMode(resetButton, INPUT_PULLUP);

  //read highscore from eeprom
  EEPROM.get(eeAddress, highscore);
  if ((highscore > 200) || (highscore < 0))
  {
    highscore = 0;
  }

  sendString(' ', ' ', ' ', ' ', noDecimals);
}


void loop()
{
  unsigned long currentMillis = millis();
  int timer1 = 0;
  int timer2 = 0;
  
  if (timer >= timerLength)
  {
    timer1 = (timer - 2) % 10;
    timer2 = (timer - 2) / 10;
  }
  else
  {
    timer1 = timer % 10;
    timer2 = timer / 10;
  }
  
  int score1 = score % 10;
  int score2 = score / 10;

  int highscore1 = highscore % 10;
  int highscore2 = highscore / 10;
  
  previousButton = currentButton;
  if (digitalRead(aPin) == on)
  {
    currentButton = 1;
    Serial.println("got a");
  }
  else if (digitalRead(bPin) == on)
  {
    currentButton = 2;
    Serial.println("got b");
  }
  else if (digitalRead(cPin) == on)
  {
    currentButton = 3;
    Serial.println("got c");
  }
  else if (digitalRead(dPin) == on)
  {
    currentButton = 4;
    Serial.println("got d");
  }
  //else if (digitalRead(resetButton) == HIGH)
  //{
  //  currentButton = -1;
  //  Serial.println("got reset");
  //}
  else
  {
    currentButton = 0;
  }
  
    
  if (currentButton != previousButton)
  {
    if (currentButton != 0)
    {
      //button has been pressed
      Serial.print("Button: ");
      Serial.print(currentButton);
      
      switch (currentButton)
      {
        case 1: 
          if (timerRunning)
          {
            timerRunning = false;
          }
          else if (timer > 0)
          {
            startTimer();
          }
          else
          {
            timer = timerLength + 1;
            score = 0;
            startTimer();
          }
          break;
        case 2: 
          if (!timerRunning)
          {
            //reset condition
            timer = timerLength + 1;
            Serial.print("timer = ");
            Serial.println(timer);
            Serial.println("score = 0");
            score = 0;
            timer1 = timer % 10;
            timer2 = timer / 10;
            score1 = 0;
            score2 = 0;
            sendString(score2, score1, timer2, timer1, noDecimals);
          }
          break;
        case 3: 
          //subtract from score
          if (score > 0)
          {
            score--;
            score1 = score % 10;
            score2 = score / 10;
            sendString(score2, score1, timer2, timer1, noDecimals);
          }
          break;
        case 4: 
          //add to score
          score ++;
          score1 = score % 10;
          score2 = score / 10;
          sendString(score2, score1, timer2, timer1, noDecimals);
          if ((timer <= 0) && (score > highscore))
          {
            //new high score
            highscore = score;
            EEPROM.write(eeAddress, highscore);
          }
          break;
        case -1: 
          highscore = 0;  
          EEPROM.write(eeAddress, highscore);
          break;
      }
    }
  }



  if (timerRunning)
  {
    //Serial.println(currentMillis - previousMillis);
    if (currentMillis - previousMillis >= 1000)
    {
      timer--;
      Serial.print("timer=");
      Serial.println(timer);
      timer1 = timer % 10;
      timer2 = timer / 10;
      sendString(score2, score1, timer2, timer1, noDecimals);
      if (timer == 0)
      {
        tone(buzzer, 1000, 2000);
        timerRunning = 0;
        if (score > highscore)
        {
          highscore = score;
          EEPROM.write(eeAddress, highscore);
        }
        gameEndMillis = currentMillis;
      }
      previousMillis = currentMillis;

    }
  }
  else if ((timer >= timerLength) || (timer <= 0))
  {
    //display high score alternating with current score
    if (currentMillis - gameEndMillis > 3000)
    {
      gameEndMillis = currentMillis;
      if (showingHighScore)
      {
        sendString(score2, score1, timer2, timer1, noDecimals);
        showingHighScore = false;
      }
      else
      {
        sendString(highscore2, highscore1, 'h', 1, noDecimals);
        showingHighScore = true;
      }

    }
  }
  
}

void startTimer()
{
  int timer1 = 0;
  int timer2 = 0;
  if (timer >= timerLength)
  {
    timer1 = (timer - 1) % 10;
    timer2 = (timer - 1) / 10;
  }
  else
  {
    timer1 = timer % 10;
    timer2 = timer / 10;
  }
  sendString(' ', 3, timer2, timer1, noDecimals);
  tone(buzzer, 1000, 250);
  delay(750);
  sendString(' ', 2, timer2, timer1, noDecimals);
  tone(buzzer, 1000, 250);
  delay(750);
  sendString(' ', 1, timer2, timer1, noDecimals);
  tone(buzzer, 1000, 250);
  delay(750);
  sendString(0, 0, timer2, timer1, noDecimals);
  tone(buzzer, 1500, 1000);
  timer++;
  timerRunning = true;
}


void sendString(byte a, byte b, byte c, byte d, boolean decimals[4])
{
  byte segment1 = getSegments(d, decimals[3]);
  byte segment2 = getSegments(c, decimals[3]);
  byte segment3 = getSegments(b, decimals[3]);
  byte segment4 = getSegments(a, decimals[3]);
  postChar(segment1);
  postChar(segment2);
  postChar(segment3);
  postChar(segment4);
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH);  
}

byte getSegments(byte digit, boolean decimal)
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

  //Serial.print("sending the following character: ");
  //Serial.println(digit);

  byte segments;
  Serial.print("digit: ");
  //Serial.println(digit);

  switch (digit)
  {
    case 1: segments = b | c; Serial.println("1"); break;
    case 2: segments = a | b | d | e | g; Serial.println("2"); break;
    case 3: segments = a | b | c | d | g; Serial.println("3"); break;
    case 4: segments = f | g | b | c; Serial.println("4"); break;
    case 5: segments = a | f | g | c | d; Serial.println("5"); break;
    case 6: segments = a | f | g | e | c | d; Serial.println("6"); break;
    case 7: segments = a | b | c; Serial.println("7"); break;
    case 8: segments = a | b | c | d | e | f | g; Serial.println("8"); break;
    case 9: segments = a | b | c | d | f | g; Serial.println("9"); break;
    case 0: segments = a | b | c | d | e | f; Serial.println("0"); break;
    case ' ': segments = 0; Serial.println(" "); break;
    case 'a': segments = a | b | c | e | f | g; Serial.println("a"); break;
    case 'b': segments = c | d | e | f | g; Serial.println("b"); break;
    case 'c': segments = g | e | d; Serial.println("c"); break;
    case 'd': segments = b | c | d | e | g; Serial.println("d"); break;
    case 'e': segments = a | d | e | f | g; Serial.println("e"); break;
    case 'f': segments = a | e | f | g; Serial.println("f"); break;
    case 'g': segments = a | b | c | d | f | g; Serial.println("g"); break;
    case 'h': segments = c | e | f | g; Serial.println("h"); break;
    case 'o': segments = a | b | f | g; Serial.println("o"); break;
    case '-': segments = g; Serial.println("-"); break;
    default: segments = 0;
  }

  if (decimal) segments |= dp;
  //Serial.print("segments: ");
  //Serial.println(segments);

  return segments;

}

//Given a number, or '-', shifts it out to the display
void postChar(byte segments)
{
  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }

}
