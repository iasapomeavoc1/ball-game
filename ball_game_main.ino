#include <TimedAction.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//GLOBALS
//pins
int s1pin = 9;
int s2pin = 13;
int s3pin = 11;
int p1pin = 11;
int p2pin = 0;
int p3pin = 8;
int led0pin = 32;
int led1pin = 34;int led2pin = 36;int led3pin = 38;int led4pin = 40;int led5pin = 42;int led6pin = 44;int led7pin = 46;int led8pin = 48;int led9pin = 50;int led10pin = 52;
int ledLRpin = 37;int ledURpin = 33;int ledLLpin = 35;int ledULpin = 39;
int lrLRpin = 4;int lrURpin = 5;int lrLLpin = 6;int lrULpin = 7;
int lr0pin = 3;int lr1pin = 1;int lr2pin = 2;int lr3pin = 3;int lr4pin = 5;int lr5pin = 4;int lr6pin = 6;int lr7pin = 7;int lr8pin = 8;int lr9pin = 9;int lr10pin = 10;


int startTime = 0;
int elapsedTime = 0;
int lifeCount = 3;
int currTarget = 1;
int targetBool = 0;
int currScore = 0;
int highScore = 0;
int gameState = 0; 

//gameState 0 --> Pre-Game Standby, ready to play, waiting for start button to be pushed. (player name input, high score displays, previous high score)
//gameState 1 --> Ball Return State, subtract 1 from lifeCount if start/mistake,increase score and target if applicable,
                                    //auto lower bar until lower endstops reached, push ball out when available
//gameState 2 --> Game Play, toggle switches will actuator bar, red button does nothing
//gameState 3 --> End Game, record score, reset lifeCount, return to Pre-Game Standby,

int returnBallState = 0;
//returnBallState = 0 --> do nothing
//returnBallState = 1 --> button pressed, timer started, push ball out
//returnBallState = 2 --> pull arm in

float returnStartTime = 0.0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

Servo s1;     
Servo s2;
Servo s3;
int p1; //toggle switch left value
int p2; //toggle switch right value 
int p3; //start button value
int p4; //returned ball switch value
int m1=1; //bottom left endstop value
int m2=1; //top left endstop value
int m3=1; //top right endstop value
int m4=1; //bottom right endstop value

//FUNCTIONS
void playNote(){
//  if(gameState==0){analogWrite(2,255);}
//  if(gameState==1){analogWrite(2,200);}
//  if(gameState==2){analogWrite(2,150);}
//  if(gameState==3){analogWrite(2,100);}
//  if(gameState==4){analogWrite(2,50);}
//  if(gameState==5){analogWrite(2,10);}
}

void updateDisplay(){  
  if(gameState==0){lcd.setCursor(0, 0); lcd.print("Press Button to");lcd.setCursor(0, 1);lcd.print("Start Game!");}
  if(gameState==1){lcd.setCursor(0, 0); lcd.print("Setting...");lcd.setCursor(0, 1);lcd.print("Score: "+String(currScore)+"pts");}
  if(gameState==2){lcd.setCursor(0, 0); lcd.print("Target: "+String(currTarget));lcd.setCursor(0, 1);lcd.print("Lives Left: "+String(lifeCount));}
  if(gameState==3){lcd.setCursor(0, 0); lcd.print("Final Score:");lcd.setCursor(0, 1);lcd.print(String(currScore)+"pts");}
  if(gameState==4){lcd.setCursor(0, 0); lcd.print("Nice Job!");lcd.setCursor(0, 1);lcd.print("");}
  if(gameState==5){lcd.setCursor(0, 0); lcd.print("Whoops!");lcd.setCursor(0, 1);lcd.print("");}

}

void clearDisplay(){
  lcd.setCursor(0, 0); lcd.print("                ");lcd.setCursor(0, 1);lcd.print("                ");
}
void controlRightBar(){
  if(gameState==1&&returnBallState==0){m1=digitalRead(lrLRpin);if(m1){s1.write(100);}else{s1.write(90);}
                  }
  else if(gameState==2){p1 = analogRead(p1pin);m1=digitalRead(lrLRpin);m2=digitalRead(lrURpin);
                   if(p1>1015 && m1){s1.write(180);}
                   else if(p1<20){s1.write(0);}
                   else{s1.write(90);}
                  }
  else{s1.write(90);}
}
void controlLeftBar(){
  if(gameState==1&&returnBallState==0){m4=digitalRead(lrLLpin);if(m4){s2.write(80);}else{s2.write(90);}
                  }
  else if(gameState==2){p2 = analogRead(p2pin);m3=digitalRead(lrULpin);m4=digitalRead(lrLLpin);
                   if(p2>1015 && m4){s2.write(0);}
                   else if(p2<20){s2.write(180);}
                   else{s2.write(90);}
                  }
  else{s2.write(90);}
}
void controlBallReturn(){
  if(returnBallState==1){s3.write(20);}
  else if(returnBallState==2){s3.write(160);}
}

TimedAction displayThread = TimedAction(300,updateDisplay);
TimedAction rightBarThread = TimedAction(100,controlRightBar);
TimedAction leftBarThread = TimedAction(100,controlLeftBar);
TimedAction ballReturnThread = TimedAction(100,controlBallReturn);
TimedAction musicThread = TimedAction(50,playNote);


void setup() {
  Serial.begin(9600);
  lcd.begin();
  pinMode(2,OUTPUT);
  pinMode(led0pin,OUTPUT);pinMode(led1pin,OUTPUT);pinMode(led2pin,OUTPUT);pinMode(led3pin,OUTPUT);pinMode(led4pin,OUTPUT);pinMode(led5pin,OUTPUT);
  pinMode(led6pin,OUTPUT);pinMode(led7pin,OUTPUT);pinMode(led8pin,OUTPUT);pinMode(led9pin,OUTPUT);pinMode(led10pin,OUTPUT);
  pinMode(ledLRpin,OUTPUT);pinMode(ledURpin,OUTPUT);pinMode(ledLLpin,OUTPUT);pinMode(ledULpin,OUTPUT);
  digitalWrite(led0pin,HIGH);digitalWrite(led1pin,LOW); digitalWrite(led2pin,LOW); digitalWrite(led3pin,LOW); digitalWrite(led4pin,LOW);digitalWrite(led5pin,LOW);    
  digitalWrite(led6pin,LOW);digitalWrite(led7pin,LOW); digitalWrite(led8pin,LOW); digitalWrite(led9pin,LOW); digitalWrite(led10pin,LOW);  
  digitalWrite(ledLRpin,HIGH);digitalWrite(ledURpin,LOW); digitalWrite(ledLLpin,HIGH); digitalWrite(ledULpin,LOW);
  
  //digitalWrite(lr0pin,HIGH);
  pinMode(lrLRpin,INPUT);pinMode(lrURpin,INPUT);pinMode(lrLLpin,INPUT);pinMode(lrULpin,INPUT);

}


void loop() {
//Serial.print(gameState);Serial.print(' ');Serial.print(lifeCount);Serial.print(' ');Serial.print(returnBallState);Serial.print(' ');Serial.print(millis()/1000.0-returnStartTime);
//Serial.print(' '); Serial.print(m1); Serial.print(' '); Serial.print(m4); 
//Serial.print(analogRead(lr2pin));
//Serial.println(' ');

  if(gameState==0){     displayThread.check();
                        musicThread.check();
                        if(digitalRead(p3pin)==1){gameState=1;s1.attach(s1pin);s2.attach(s2pin);s1.write(0);s2.write(180);delay(100);s1.write(90);s2.write(90);
                                                startTime=millis()/1000.0;clearDisplay();}
  }
  else if(gameState==1){displayThread.check();
                        rightBarThread.check();
                        leftBarThread.check();
                        musicThread.check();
                        ballReturnThread.check();
                        if(!m1&&!m4){
                          if(returnBallState==0){s1.write(90);s2.write(90);s1.detach();s2.detach();returnStartTime=millis()/1000.0;returnBallState=1;s3.attach(s3pin);delay(100);}
                          else if(millis()/1000.0-returnStartTime>5&&returnBallState==1){gameState=2;s1.attach(s1pin);s2.attach(s2pin);clearDisplay();}
                          }
                        if(lifeCount==0){gameState=3;}
                        if(currTarget==1){digitalWrite(led1pin,HIGH);}
                        else if(currTarget==2){digitalWrite(led2pin,HIGH);}
                        else if(currTarget==3){digitalWrite(led3pin,HIGH);}
                        else if(currTarget==4){digitalWrite(led4pin,HIGH);}
                        else if(currTarget==5){digitalWrite(led5pin,HIGH);}
                        else if(currTarget==6){digitalWrite(led6pin,HIGH);}
                        else if(currTarget==7){digitalWrite(led7pin,HIGH);}
                        else if(currTarget==8){digitalWrite(led8pin,HIGH);}
                        else if(currTarget==9){digitalWrite(led9pin,HIGH);}
                        else if(currTarget==10){digitalWrite(led10pin,HIGH);}
                        delay(100);

  }
  else if(gameState==2){
                        displayThread.check();
                        rightBarThread.check();
                        leftBarThread.check();
                        ballReturnThread.check();
                        musicThread.check();
                        if(millis()/1000.0-returnStartTime>10&&returnBallState==1){returnBallState=2;}
                        else if(millis()/1000.0-returnStartTime>15&&returnBallState==2){returnBallState=3;s3.detach();}
                        else if(digitalRead(lr0pin)==0&&returnBallState==3){returnBallState=0;
                                                                        if(targetBool==1){
                                                                        if(currTarget==1){currScore+=2000.0/(millis()/1000.0-returnStartTime)+100;gameState=4;}
                                                                        else if(currTarget==2){currScore+=2000.0/(millis()/1000.0-returnStartTime)+500;gameState=4;}
                                                                        else if(currTarget==3){currScore+=2000.0/(millis()/1000.0-returnStartTime)+1000;gameState=4;}
                                                                        else if(currTarget==4){currScore+=5000.0/(millis()/1000.0-returnStartTime)+1000;gameState=4;}
                                                                        else if(currTarget==5){currScore+=5000.0/(millis()/1000.0-returnStartTime)+3000;gameState=4;}
                                                                        else if(currTarget==6){currScore+=5000.0/(millis()/1000.0-returnStartTime)+5000;gameState=4;}
                                                                        else if(currTarget==7){currScore+=10000.0/(millis()/1000.0-returnStartTime)+10000;gameState=4;}
                                                                        else if(currTarget==8){currScore+=30000.0/(millis()/1000.0-returnStartTime)+20000;gameState=4;}
                                                                        else if(currTarget==9){currScore+=50000.0/(millis()/1000.0-returnStartTime)+30000;gameState=4;}
                                                                        else if(currTarget==10){currScore+=100000.0/(millis()/1000.0-returnStartTime)+50000+lifeCount*10000;gameState=3;}                                                                        
                                                                        returnStartTime=millis()/1000.0;currTarget+=1;clearDisplay();}
                                                                        else{gameState=5; lifeCount=lifeCount-1; returnStartTime=millis()/1000.0;clearDisplay();}
                        }
                        if(lifeCount==0){gameState=3;}
  
                        if(currTarget==1){if(analogRead(lr1pin)<10){targetBool=1;digitalWrite(led1pin,LOW);}}
                        else if(currTarget==2){if(analogRead(lr2pin)<10){targetBool=1;digitalWrite(led2pin,LOW);}}
                        else if(currTarget==3){if(analogRead(lr3pin)<10){targetBool=1;digitalWrite(led3pin,LOW);}}
                        else if(currTarget==4){if(analogRead(lr4pin)<20){targetBool=1;digitalWrite(led4pin,LOW);}}
                        else if(currTarget==5){if(analogRead(lr5pin)<10){targetBool=1;digitalWrite(led5pin,LOW);}}
                        else if(currTarget==6){if(analogRead(lr6pin)<20){targetBool=1;digitalWrite(led6pin,LOW);}}
                        else if(currTarget==7){if(analogRead(lr7pin)<10){targetBool=1;digitalWrite(led7pin,LOW);}}
                        else if(currTarget==8){if(analogRead(lr8pin)<20){targetBool=1;digitalWrite(led8pin,LOW);}}
                        else if(currTarget==9){if(analogRead(lr9pin)<10){targetBool=1;digitalWrite(led9pin,LOW);}}
                        else if(currTarget==10){if(analogRead(lr10pin)<10){targetBool=1;digitalWrite(led10pin,LOW);}}
                        
    }
  else if(gameState==3){displayThread.check();    
                        rightBarThread.check();
                        leftBarThread.check();
                        ballReturnThread.check();
                        musicThread.check();
                        currScore=0;
                        if(digitalRead(p3pin)==1){gameState=1;s1.attach(s1pin);s2.attach(s2pin);s1.write(0);s2.write(180);delay(100);s1.write(90);s2.write(90);
                                                startTime=millis()/1000.0;lifeCount=3;currTarget=1;clearDisplay();}
                        digitalWrite(led1pin,LOW); digitalWrite(led2pin,LOW); digitalWrite(led3pin,LOW); digitalWrite(led4pin,LOW);digitalWrite(led5pin,LOW);    
  digitalWrite(led6pin,LOW);digitalWrite(led7pin,LOW); digitalWrite(led8pin,LOW); digitalWrite(led9pin,LOW); digitalWrite(led10pin,LOW);  
  }
  else if(gameState==4){displayThread.check();
                        musicThread.check();    
                        if(millis()/1000.0-returnStartTime>2){gameState=1;targetBool=0;clearDisplay();}
  }
  else if(gameState==5){displayThread.check();
                        musicThread.check();    
                        if(millis()/1000.0-returnStartTime>2){gameState=1;targetBool=0;clearDisplay();}
  }
}
