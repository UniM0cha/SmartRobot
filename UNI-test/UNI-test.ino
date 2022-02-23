#include <PRIZM.h>
#include <stdio.h>
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

#define RED 1;
#define GREEN 2;
#define BLUE 3;
#define TRUE 1;
#define FALSE 0;

// 센서값을 저장할 전역변수
int a1, a2, D2, D3, D4;

// 현재 자신이 몇번째 줄에 있는지 확인하는 변수
int lineCurrent = 1;
// 1 = 첫번째줄 / 2 = 두번째줄 / 3 = 세번째줄

// 처음에 블록이 놓여져 있는 위치
int objSt[3][2] = {{1, 1}, {1, 1}, {1, 1}};
//옮겨야할 오브젝트 배열 1,2,3라인의 1층과 2층의 유무를 정하고
// 0 : 없음(FALSE), 1 : 있음(TRUE) 으로 정의

// 결과적으로 이렇게 되어야함
int objEd[3][2] = {{0, 0}, {0, 0}, {0, 0}};
//옮겨진 오브젝트 배열의 색상 1,2,3라인의 1층과 2층의 유무를 정하고
// 0 : 없음(FALSE), 1 : 있음(TRUE) 으로 정의

void setup()
{
  prizm.PrizmBegin();
  prizm.resetEncoder(1);
  Serial.begin(9600);
  collectSensor();
  start();
  findRightLine();
  LineTracing();
}

void loop()
{
  // LineTracing(40);
  prizm.PrizmEnd();
}

// 처음 시작할 때 대각선으로 이동
void start()
{
  wheel(-80, 0, -20);
  delay(1500);
  wheel(-50, 0, -5);
  delay(1000);
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

// 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
void LineTracing()
{
  int speed = 50;
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

void findRightLine()
{
  Serial.println("Find Right Line...");
  while (1)
  {
    collectSensor();
    wheel(-60, 0, 0);
    if (D4 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
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
    wheel(-60, 0, 0);
    if (D3 == HIGH)
    {
      Serial.println("Line Found");
      wheel(0, 0, 0);
      break;
    }
  }
}