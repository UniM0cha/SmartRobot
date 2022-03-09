#include <Wire.h>
#include <PRIZM.h>
#include <stdio.h>
#include "Adafruit_TCS34725.h"
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;


#define RED 1;
#define GREEN 2;
#define BLUE 3;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

void wheel(int x, int y, int z);
void Direction_find(int now_line, int next);
void Direction_move(String direc, int cnt);
void LineTracing();
void LineTraceing();
void lift_up(int s);
void lift_down(int s);
void findRightLine();
void findLeftLine();
void start();
void turn();
void collectSensor();
int ColorCheck();

int destination = 0;
int count = 0;
int n = 0;
int TargetLine = 0;
int CurrentLine = 1;  // 1 레드  2 그린  3 블루
int a1, a2, D2, D3, D4;

void setup()
{
  prizm.PrizmBegin();
  Serial.begin(9600);
}

void loop()
{
  //start();
  //LineTracing(40);
  //lift_up(4700);
  //LineTracing(40);
  //lift_up(4400); //내려놓는 위치
  //lift_down(350);

  LineTracing(50);
  lift_up(4700);
  Serial.println(ColorCheck());
  TargetLine = ColorCheck();
  wheel(0, 60, 0);
  delay(1000);
  turn();
  Direction_find(CurrentLine, TargetLine);
  LineTracing(50);
  lift_down(400);
  wheel(0, 60, 0);
  delay(1000);

  //wheel(0,50,0);
  //delay(1000);
  //wheel(0, 80, 0);
  //delay(600);
  //turn();
  //LineTracing(50);
  prizm.PrizmEnd();
}

 void start(){
  wheel(0, 0, -35); // 오른쪽으로 돌기
  delay(1000);
  wheel(-60, 60, 0); // 왼쪽 앞 대각선으로 이동
  delay(900);
  while (1)
  {
    wheel(-60, 0, 0); // 왼쪽으로 이동
    if (prizm.readLineSensor(4) == HIGH)
      break;
  }
}

void line(int l)
{
  while (prizm.readLineSensor(2) == LOW)
  {
  }
}

void turn()
{
  while (1)
  {
      wheel(0, 0, 30);
    if (prizm.readLineSensor(4) == HIGH)
    {
      break;
    }
  }
}

void LineTracing(int speed)
{
  while (1)
  {
    Serial.println("Line Tracing...");
    
    collectSensor();


    if (D3 == HIGH && D4 == LOW)
    {
      // D3 감지되면 왼쪽 앞
      wheel(0, -speed, -speed * 0.15);
    }

    if (D3 == LOW && D4 == HIGH)
    {
      // D4 감지되면 오른쪽 앞
      wheel(0, -speed, speed * 0.15);
    }

    if (D3 == LOW && D4 == LOW)
    {
      // D3, D4 감지 안되면 직진
      wheel(0, -speed, 0);
    }

    if (D3 == HIGH && D4 == HIGH)
    {
      // D3, D4 감지되면 정지
      wheel(0, 0, 0);
      break;
    }
  }
}

void wheel(int x, int y, int z)
{
  int A = 0, B = 0, C = 0, D = 0;

  A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
  B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
  C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
  D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

  exc1.setMotorPowers(1, A, B);
  exc2.setMotorPowers(2, C, D);
}

void lift_up(int s){    //리프트 up 함수
    n += s;
    prizm.setMotorSpeed(1, -300);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}

void lift_down(int s){  //리프트 down 함수
    n -= s;
    prizm.setMotorSpeed(1, 300);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}

int ColorCheck() {
uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    int r_cr;
    Serial.print("r : ");   Serial.print(r);
    Serial.print(", g : "); Serial.print(g);
    Serial.print(", b : "); Serial.print(b);
    Serial.print(", c : "); Serial.print(c);
    Serial.println();
    if (c > 3000) {
        Serial.println("color 인식 error");
        tcs.setInterrupt(true);
        r_cr = 0;
    }
    else {
        tcs.setInterrupt(false);
        if(r>=440 && r<=500 && g>=220 && g<=280 && b>=200 && b<=260){
            Serial.println("RED");
            r_cr = 1;
        }
        else if(r>=180 && r<=240 && g>=430 && g<=490 && b>=240 && b<=300){
            Serial.println("GREEN");
            r_cr = 2;
        }
        else if(r>=100 && r<=160&& g>=230 && g<=290 && b>=350 && b<=410){
            Serial.println("BLUE");
            r_cr = 3;
        }
        else if(r>=800 && r<=940&& g>=810 && g<=950 && b>=370 && b<=510){
            Serial.println("YELLOW");
            r_cr = 4;
        }
    }
    return r_cr;
}
void Direction_find(int now_line, int next){//목적지 라인 찾기
    int destination = now_line - next;//음수면 왼쪽, 양수면 오른쪽
    int cnt = abs(destination);//라인 이동 횟수
    String direc = "";

    if(destination < 0){
        direc = "LEFT";  
    }else if(destination > 0){
        direc = "RIGHT";
    }else{
        direc = "STOP";
    }
    Direction_move(direc, cnt);//목적지 라인으로 이동
}

void Direction_move(String direc, int cnt){//목적지 라인으로 이동하는 함수
    for(int i=0; i<cnt; i++){//이동 횟수에 따른 반복문
        if(direc.equals("LEFT")){
            findLeftLine();
            CurrentLine += 1;
        }else if(direc.equals("RIGHT")){
            findRightLine();
            CurrentLine -= 1;
        }else{
            wheel(0,0,0);
        }
        delay(200);
    }
    while(prizm.readLineSensor(5)==LOW){
        exc2.setMotorSpeeds(2, -50, -50);
        if(prizm.readLineSensor(5)==HIGH){break;}
    }
    wheel(0,0,0);
}

void findRightLine()
{
  Serial.println("Find Right Line...");
  while (1)
  {
    collectSensor();
    wheel(-45, 0, 0);
    if (D4 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      CurrentLine += 1;
      break;
    }
  }
}

void findLeftLine()
{
  Serial.println("Left Right Line...");
  while (1)
  {
    collectSensor();
    wheel(45, 0, 0);
    if (D3 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      break;
    }
  }
}

void collectSensor()
{
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  D2 = prizm.readLineSensor(2);
  D3 = prizm.readLineSensor(3);
  D4 = prizm.readLineSensor(4);
  Serial.print("A1: ");
  Serial.print(a1);
  Serial.print(" / A2: ");
  Serial.print(a2);
  Serial.print(" / D2: ");
  Serial.print(D2);
  Serial.print(" / D3: ");
  Serial.print(D3);
  Serial.print(" / D4: ");
  Serial.println(D4);
}