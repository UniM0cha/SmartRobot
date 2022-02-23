#include <PRIZM.h>
#include <stdio.h>
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

void wheel(int x, int y, int z);
int LineTraceing();

void setup()
{
  prizm.PrizmBegin();
  prizm.resetEncoder(1);
  Serial.begin(9600);
}

void loop()
{
  int state = LineTraceing();
  if (state == 1)
  {
    prizm.PrizmEnd();
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

void print(int A1, int A2, int D2, int D3, int D4)
{
  Serial.print("A1: ");
  Serial.print(A1);
  Serial.print(" / A2: ");
  Serial.print(A2);
  Serial.print(" / D2: ");
  Serial.print(D2);
  Serial.print(" / D3: ");
  Serial.print(D3);
  Serial.print(" / D4: ");
  Serial.println(D4);
}