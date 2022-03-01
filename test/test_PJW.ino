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
void LineTracing();
void LineTraceing();
void lift_up(int s);
void lift_down(int s);
void start();
void turn();
int ColorCheck();
int n = 0;

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
  lift_up(1000);
  Serial.println(ColorCheck());
  lift_down(n);
  
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
    int a1 = analogRead(A1);
    int a2 = analogRead(A2);
    int D2 = prizm.readLineSensor(2);
    int D3 = prizm.readLineSensor(3);
    int D4 = prizm.readLineSensor(4);


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
