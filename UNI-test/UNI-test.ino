#include <PRIZM.h>
#include <stdio.h>
#include "Adafruit_TCS34725.h"
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

#define RED 0;
#define GREEN 1;
#define BLUE 2;

#define RIGHT 1;
#define LEFT -1;
#define STOP 0;

#define TRUE 1;
#define FALSE 0;

// 센서값을 저장할 전역변수
int a1, a2, D2, D3, D4, diff;

// 현재 리프트 높이
int n = 0;

// 0 = 첫번째줄 / 1 = 두번째줄 / 2 = 세번째줄
// 현재 자신이 있는 줄
int currentLine = 0;
// 목적지 줄
int targetLine = 0;

int A = 0;

/**
 * 옮겨야할 오브젝트 배열 1, 2, 3라인의 1층과 2층의 유무를 정의할 변수들
 * 0 : 없음(FALSE), 1 : 있음(TRUE) 으로 정의
 */

// 처음 블록이 놓여있는 곳
int startBlock[3][2] = {{1, 1}, {1, 1}, {1, 1}}; // {1번줄{2층, 1층}, 2번줄{2층, 1층}, 3번줄{2층, 1층}}
// 블록을 놓아야 하는 곳
int endBlock[3][2] = {{0, 0}, {0, 0}, {0, 0}};

void setup()
{
  prizm.PrizmBegin();
  prizm.resetEncoder(1);
  Serial.begin(9600);
  setDiff();
  collectSensor();
}

void loop()
{
  findRightLine();
  for (int k = 0; k < 6; k++)
  {
    LineTracing();
    back(1000);
    turn();
  }
  prizm.PrizmEnd();
}

// 처음 시작할 때 대각선으로 이동
void start()
{
  collectSensor();
  wheel(-90, 10, -28);
  delay(1800);
  while (1)
  {
    collectSensor();
    wheel(-45, 0, 0);
    if (D4 == HIGH)
    {
      Serial.println("Line Found");
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
      currentLine++;
      break;
    }
  }
}

void findLeftLine()
{
  Serial.println("Find Left Line...");
  while (1)
  {
    collectSensor();
    wheel(45, 0, 0);
    if (D3 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      currentLine--;
      break;
    }
  }
}

void lift_up(int s)
{ //리프트 up 함수
  n += s;
  prizm.setMotorSpeed(1, -300);
  delay(s);
  prizm.setMotorSpeed(1, 0);
}

void lift_down(int s)
{ //리프트 down 함수
  n -= s;
  prizm.setMotorSpeed(1, 300);
  delay(s);
  prizm.setMotorSpeed(1, 0);
}

int ColorCheck()
{
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  int r_cr;
  Serial.print("r : ");
  Serial.print(r);
  Serial.print(", g : ");
  Serial.print(g);
  Serial.print(", b : ");
  Serial.print(b);
  Serial.print(", c : ");
  Serial.print(c);
  Serial.println();
  if (c > 3500)
  {
    Serial.println("color 인식 error");
    tcs.setInterrupt(true);
    r_cr = 4;
  }
  else
  {
    tcs.setInterrupt(false);
    if (r >= 525 && r <= 605 && g >= 245 && g <= 325 && b >= 230 && b <= 310)
    {
      Serial.println("RED");
      r_cr = RED;
    }
    else if (r >= 210 && r <= 290 && g >= 525 && g <= 605 && b >= 290 && b <= 370)
    {
      Serial.println("GREEN");
      r_cr = GREEN;
    }
    else if (r >= 120 && r <= 200 && g >= 275 && g <= 355 && b >= 425 && b <= 505)
    {
      Serial.println("BLUE");
      r_cr = BLUE;
    }
    else if (r >= 1100 && r <= 1360 && g >= 1130 && g <= 1400 && b >= 550 && b <= 670)
    {
      Serial.println("YELLOW");
      r_cr = 3;
    }
    else
    {
      r_cr = 0;
    }
  }
  return r_cr;
}

void turn()
{
  wheel(0, 0, 40);
  delay(500);
  while (1)
  {
    collectSensor();
    if (a2 - 500 >= a1)
    {
      wheel(0, 0, 0);
      break;
    }
  }
}

void Direction_find(int now_line, int next)
{                                    //목적지 라인 찾기
  int destination = now_line - next; //음수면 오른쪽, 양수면 왼쪽
  int cnt = abs(destination);        //라인 이동 횟수
  int direc = 0;

  if (destination < 0)
  {
    direc = RIGHT;
  }
  else if (destination > 0)
  {
    direc = LEFT;
  }
  else
  {
    direc = STOP;
  }
  Serial.println(direc);
  Serial.println(cnt);
  Direction_move(direc, cnt); //목적지 라인으로 이동
}

void Direction_move(int direc, int cnt)
{ //목적지 라인으로 이동하는 함수
  for (int i = 0; i < cnt; i++)
  { //이동 횟수에 따른 반복문
    if (direc == 1)
    {
      findRightLine();
    }
    else if (direc == -1)
    {
      findLeftLine();
    }
    else
    {
      wheel(0, 0, 0);
    }
    delay(300);
  }
  wheel(0, 0, 0);
}

void firstHamsu()
{
  if (startBlock[currentLine][1] == 0)
  {
    for (int i = 0; i < 3; i++)
    {
      if (startBlock[i][1] == 1)
      {
        Direction_find(currentLine, i);
      }
    }
  }
  if (startBlock[currentLine][0] == 0 && startBlock[currentLine][1] == 1)
  {
    // lift_up(/*1층높이*/);    // 사실상 무쓸모 지워도 OK
    startBlock[currentLine][1] = 0;
  }
  else if (startBlock[currentLine][0] == 1)
  {
    lift_up(1100);
    startBlock[currentLine][0] = 0;
  }
}

void secondHamsu()
{
  if (endBlock[targetLine][1] == 1)
  {
    A = 700; // 단상위 2층에 내리기 위해
    endBlock[targetLine][0] = 1;
  }
  else if (endBlock[targetLine][1] == 0)
  {
    A = 1800; // 단상위 1층에 내리기 위해
    endBlock[targetLine][1] = 1;
  }
}

/////////////////////////// 정윤 /////////////////////////////

// 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
void LineTracing()
{
  int frontSpeed = 40;
  int errorRange = 0;

  while (1)
  {
    Serial.println("Line Tracing...");
    collectSensor();

    // 도착하면 정지
    if (D3 == HIGH && D4 == HIGH)
    {
      wheel(0, 0, 0);
      break;
    }
    // 중앙에 선이 있을 때
    if (D2 == HIGH)
    {
      // 디지털 감지
      if (D3 == HIGH && D4 == LOW)
      {
        // 왼쪽 회전
        wheel(0, -frontSpeed, -5);
      }
      else if (D3 == LOW && D4 == HIGH)
      {
        // 오른쪽 회전
        wheel(0, -frontSpeed, 5);
      }
      // 아날로그 감지
      else if (D3 == LOW && D4 == LOW)
      {
        if (a1 > a2 + errorRange)
        {
          // 왼쪽 회전
          wheel(0, -frontSpeed, -4);
        }

        else if (a1 + errorRange < a2)
        {
          // 오른쪽 회전
          wheel(0, -frontSpeed, 4);
        }
      }
    }

    // 중앙에 선이 없을 때
    else if (D2 == LOW)
    {
      // 디지털 감지
      if (D3 == HIGH && D4 == LOW)
      {
        // 왼쪽 횡이동
        wheel(5, -frontSpeed, -5);
      }
      else if (D3 == LOW && D4 == HIGH)
      {
        // 오른쪽 횡이동
        wheel(-5, -frontSpeed, 5);
      }
      // 아날로그 감지
      else if (D3 == LOW && D4 == LOW)
      {
        if (a1 > a2 + errorRange)
        {
          // 왼쪽 횡이동
          wheel(4, -frontSpeed, -4);
        }

        else if (a1 + errorRange < a2)
        {
          // 오른쪽 횡이동
          wheel(-4, -frontSpeed, 4);
        }
      }
    }
  }
}

// 뒤로 간다음 중간 맞춤
void back(int time)
{
  int frontSpeed = 0;
  int analogSpeed = 30;
  int digitalSpeed = 40;
  int errorRange = 40;

  wheel(0, 50, 0);
  delay(time);

  while (1)
  {
    collectSensor();
    if (D2 == LOW)
    {
      // 디지털 감지
      if (D3 == HIGH && D4 == LOW)
      {
        // 왼쪽 횡이동
        wheel(digitalSpeed, -frontSpeed, 0);
      }
      else if (D3 == LOW && D4 == HIGH)
      {
        // 오른쪽 횡이동
        wheel(-digitalSpeed, -frontSpeed, 0);
      }
      // 아날로그 감지
      else if (D3 == LOW && D4 == LOW)
      {
        if (a1 > a2 + errorRange)
        {
          // 왼쪽 횡이동
          wheel(analogSpeed, -frontSpeed, 0);
        }

        else if (a1 + errorRange < a2)
        {
          // 오른쪽 횡이동
          wheel(-analogSpeed, -frontSpeed, 0);
        }
      }
    }
    else if (D2 == HIGH)
    {
      wheel(0, 0, 0);
      break;
    }
  }
}

void setDiff()
{
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  diff = a2 - a1;
}

// 센서 값 읽는 함수
void collectSensor()
{
  a1 = analogRead(A1) + diff;
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

////////////////////////// 정윤 /////////////////////////