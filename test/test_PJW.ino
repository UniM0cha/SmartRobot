#include <PRIZM.h>
#include <stdio.h>
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

void wheel(int x, int y, int z);
void LineTracing();
void LineTraceing();
void start();
void turn();
int n = 0;

void setup()
{
  prizm.PrizmBegin();
  Serial.begin(9600);
}

void loop()
{
  start();
  LineTracing(50);
  lift_up();
  wheel(0, 80, 0);
  delay(600);
  //turn();
  LineTracing(50);
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

int LineTraceing()
{
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int D2 = prizm.readLineSensor(2);
  int D3 = prizm.readLineSensor(3);
  int D4 = prizm.readLineSensor(4);
  print(a1, a2, D2, D3, D4);

  if (D3 == HIGH && D4 == LOW)
  {
    // D3 감지되면 왼쪽 앞
    wheel(0, -40, -5);
    return 0;
  }

  if (D3 == LOW && D4 == HIGH)
  {
    // D4 감지되면 오른쪽 앞
    wheel(0, -40, 5);
    return 0;
  }

  if (D3 == LOW && D4 == LOW)
  {
    // D3, D4 감지 안되면 직진
    wheel(0, -40, 0);
    return 0;
  }

  if (D3 == HIGH && D4 == HIGH)
  {
    // D3, D4 감지되면 정지
    // wheel(0, 0, 0);
    return 1;
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