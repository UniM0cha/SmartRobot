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
  start();
  for (int k = 0; k < 6; k++)
  {
    firstHamsu();
    LineTracing();
    lift_up(5800 - n);
    targetLine = ColorCheck();
    Serial.println(ColorCheck()); // Color값 체크
    secondHamsu();
    back(900);
    Direction_find(currentLine, targetLine);
    turn();
    LineTracing();
    lift_down(A);
    back(900);
    turn();
    select_lift_down();
  }
  // Yellow //
  Direction_find(currentLine, 2);
  LineTracing();
  secondstart();
  lift_up(4000);
  LineTracing();
  wheel(0, -50, 0); //오브젝트 적재를 위한 전진
  delay(100);
  wheel(0, 0, 0);
  lift_up(1700);
  back(500);
  wheel(-60, 20, 0);
  delay(300);
  while (1)
  {
    collectSensor();
    wheel(-40, 10, 0);
    if (D4 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      break;
    }
  }
  LineTracing();
  wheel(0, -50, 0); //오브젝트 적재를 위한 전진
  delay(100);
  wheel(0, 0, 0);
  lift_down(700);
  back(1100);
  lift_down(1000);
  LineTracing();
  wheel(0, -50, 0); //오브젝트 적재를 위한 전진
  delay(100);
  wheel(0, 0, 0);
  lift_up(1700);
  back(1300);
  firstend();
  Direction_find(currentLine, 0);
  LineTracing();
  secondend();
  prizm.PrizmEnd();
}

// 처음 시작할 때 대각선으로 이동
void start()
{
  collectSensor();
  wheel(-90, 10, -28);
  delay(1700);
  setDiff();
  while (1)
  {
    collectSensor();
    wheel(-45, 0, 0);
    if (D2 == HIGH)
    {
      Serial.println("Line Found");
      wheel(45, 0, 0);
      delay(230);
      wheel(0, 0, 0);
      delay(200);
      center();
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
  collectSensor();
  wheel(-45, 0, 0);
  delay(200);
  while (1)
  {
    collectSensor();
    wheel(-45, 0, 0);
    if (D2 == HIGH)
    {
      Serial.println("Line Found");
      wheel(45, 0, 0);
      delay(230);
      wheel(0, 0, 0);
      delay(200);
      currentLine++;
      break;
    }
  }
}

void findLeftLine()
{
  collectSensor();
  Serial.println("Find Left Line...");
  wheel(45, 0, 0);
  delay(200);
  while (1)
  {
    collectSensor();
    wheel(45, 0, 0);
    if (D2 == HIGH)
    {
      Serial.println("Line Found");
      wheel(-45, 0, 0);
      delay(230);
      wheel(0, 0, 0);
      delay(200);
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
    prizm.PrizmEnd();
  }
  else
  {
    tcs.setInterrupt(false);
    if (r >= 260 && r <= 605 && g >= 220 && g <= 325 && b >= 180 && b <= 310)
    {
      Serial.println("RED");
      r_cr = RED;
    }
    else if (r >= 180 && r <= 260 && g >= 360 && g <= 580 && b >= 240 && b <= 355)
    {
      Serial.println("GREEN");
      r_cr = GREEN;
    }
    else if (r >= 110 && r <= 200 && g >= 240 && g <= 355 && b >= 360 && b <= 505)
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
      r_cr = 4;
      Serial.println("색깔 값 다시 구하기");
      prizm.PrizmEnd();
    }
  }
  return r_cr;
}

// 뒤로 돌아서 중간 맞추는 함수
void turn()
{
  int errorRange = 600;
  // 처음엔 빠른 속도로 돌다가
  wheel(0, 0, 40);
  delay(1200);
  // 느린속도로 센서 감지
  wheel(0, 0, 25);
  while (1)
  {
    collectSensor();
    // 아날로그 센서 사용
    if (a2 - errorRange >= a1)
    {
      wheel(0, 0, 0);
      break;
    }
  }
  center();
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
        break;
      }
    }
  }
  if (startBlock[currentLine][0] == 0 && startBlock[currentLine][1] == 1)
  {
    startBlock[currentLine][1] = 0;
  }
  else if (startBlock[currentLine][0] == 1 && n == 0)
  {
    lift_up(1100);
    startBlock[currentLine][0] = 0;
  }
  else if (startBlock[currentLine][0] == 1 && n != 0)
  {
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
    A = 1700; // 단상위 1층에 내리기 위해
    endBlock[targetLine][1] = 1;
  }
}

// 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
void LineTracing()
{
  int frontSpeed = 45;
  int errorRange = 0;

  while (true)
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
          wheel(0, -frontSpeed, -3);
        }

        else if (a1 + errorRange < a2)
        {
          // 오른쪽 회전
          wheel(0, -frontSpeed, 3);
        }
      }
    }

    // 중앙에 선이 없을 때
    else if (D2 == LOW)
    {
      // 디지털 감지
      if (D3 == HIGH && D4 == LOW)
      {
        // 왼쪽 횡이동 & 왼쪽 회전
        wheel(5, -frontSpeed, -5);
      }
      else if (D3 == LOW && D4 == HIGH)
      {
        // 오른쪽 횡이동 & 오른쪽 회전
        wheel(-5, -frontSpeed, 5);
      }
      // 아날로그 감지
      else if (D3 == LOW && D4 == LOW)
      {
        if (a1 > a2 + errorRange)
        {
          // 왼쪽 횡이동 & 왼족 회전
          wheel(5, -frontSpeed, -3);
        }

        else if (a1 + errorRange < a2)
        {
          // 오른쪽 횡이동 & 오른쪽 회전
          wheel(-5, -frontSpeed, 3);
        }
      }
    }
  }
}
// 뒤로 간다음 중간 맞춤
void back(int time)
{
  Serial.println("back...");
  wheel(0, 50, 0);
  delay(time);
  wheel(0, 0, 0);
  center();
}
// 아날로그 센서값 통일
void setDiff()
{
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  diff = a2 - a1;
}

// 가운데 맞추는 코드
void center()
{
  int errorRange = 80;
  int onLine = 250;      // 아날로그 센서가 줄 위에 있을 때의 센서 최소값
  int checkTime = 500;   // 이 숫자를 올리면 줄을 찾는 반경이 넓어짐
  bool isCenter = false; // 중앙(D2) 확인
  bool isMiddle = false; // 중간(a1, a2)확인

  collectSensor();

  //== 1. 중앙 맞추기 ==//
  while (!isCenter)
  {
    if (D2 == HIGH)
    {
      isCenter = true;
    }
    else if (D2 == LOW)
    {
      // 오른쪽 탐색
      int start = millis();
      int end = millis();
      while (1)
      {
        end = millis();
        if (end - start >= checkTime || isCenter)
        {
          wheel(0, 0, 0);
          break;
        }
        wheel(30, 0, 0);
        collectSensor();
        if (D2 == HIGH)
        {
          isCenter = true;
        }
      }

      // 왼쪽 탐색
      start = millis();
      end = millis();
      while (1)
      {
        end = millis();
        if (end - start >= checkTime * 2 || isCenter)
        {
          wheel(0, 0, 0);
          break;
        }
        wheel(-30, 0, 0);
        collectSensor();
        if (D2 == HIGH)
        {
          isCenter = true;
        }
      }
    }
  }

  //== 2. 중간 맞추기 ==//

  while (!isMiddle)
  {
    collectSensor();

    // 두 센서중에 하나라도 줄을 감지한다면
    if (a1 >= onLine || a2 >= onLine)
    {

      if (a1 >= a2 - errorRange && a1 - errorRange <= a2) // 두 센서 사이에 줄이 있음 => 성공
      {
        isMiddle = true;
        Serial.println("완성");
        wheel(0, 0, 0);
        break;
      }

      else if (a1 < a2 - errorRange) // 오른쪽 센서 값이 더 클 때
      {
        Serial.println("D2 감지 | A2 감지 됨");
        wheel(0, 0, 15);
      }

      else if (a1 - errorRange > a2) // 왼쪽 센서 값이 더 클 때
      {
        Serial.println("D2 감지 | A1 감지 됨");
        wheel(0, 0, -15);
      }
    }

    else // 줄에서 벗어나 있는 경우
    {
      // 오른쪽 탐색
      bool isFindLine = false;
      int start = millis();
      int end = millis();
      while (!isFindLine)
      {
        while (1)
        {
          if (a2 > onLine)
          {
            isFindLine = true;
          }
          end = millis();

          if (end - start >= checkTime || isFindLine)
          {
            break;
          }

          wheel(0, 0, 25);
          collectSensor();
        }

        // 왼쪽 탐색
        start = millis();
        end = millis();
        while (1)
        {
          if (a1 > onLine)
          {
            isFindLine = true;
          }
          end = millis();
          if (end - start >= checkTime * 2 || isFindLine)
          {
            break;
          }
          wheel(0, 0, -25);
          collectSensor();
        }
      }
    }
  }
}

// 센서 값 읽는 함수, 아날로그는 a2 값 기준
void collectSensor()
{
  a1 = analogRead(A1) + diff;
  a2 = analogRead(A2);
  D2 = prizm.readLineSensor(2);
  D3 = prizm.readLineSensor(3);
  D4 = prizm.readLineSensor(4);
  Serial.print("a1: ");
  Serial.print(a1);
  Serial.print(" / a2: ");
  Serial.print(a2);
  Serial.print(" / D2: ");
  Serial.print(D2);
  Serial.print(" / D3: ");
  Serial.print(D3);
  Serial.print(" / D4: ");
  Serial.println(D4);
}

void secondstart()
{
  collectSensor();
  wheel(-90, -10, 28);
  delay(1700);
  setDiff();
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

void firstend()
{
  collectSensor();
  wheel(90, 20, -25);
  delay(1700);
  setDiff();
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
void secondend()
{
  collectSensor();
  wheel(90, 10, -27);
  delay(1600);
  wheel(40, 0, 0);
  delay(700);
  setDiff();

  while (1)
  {
    collectSensor();
    wheel(0, -50, 0);
    if (D3 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      lift_down(n);
      wheel(0, 50, 0);
      delay(1300);
      wheel(0, 0, 0);
      delay(200);
      lastend();
      break;
    }
  }
  Serial.println("BREAK");
}
void lastend()
{
  Serial.println("last함수 시작");
  collectSensor();
  wheel(0, 0, 45);
  delay(1550);
  wheel(70, 20, 0);
  delay(1630);
  setDiff();
  while (1)
  {
    collectSensor();
    wheel(0, 60, 0);
    if (D4 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 60, 0);
      delay(100);
      wheel(0, 0, 0);
      break;
    }
  }
}

void select_lift_down()
{
  if (startBlock[currentLine][0] == 1)
  {
    lift_down(n - 1100);
  }
  else
  {
    lift_down(n);
  }
}
