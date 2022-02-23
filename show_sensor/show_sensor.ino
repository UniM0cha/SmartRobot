#include <PRIZM.h>
#include <stdio.h>
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

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

void setup()
{
  prizm.PrizmBegin();
  prizm.resetEncoder(1);
  Serial.begin(9600);
}

void loop()
{
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int D2 = prizm.readLineSensor(2);
  int D3 = prizm.readLineSensor(3);
  int D4 = prizm.readLineSensor(4);
  print(a1, a2, D2, D3, D4);
}