#include <PRIZM.h>
#include <stdio.h>
#include "Adafruit_TCS34725.h"
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

void wheel(int x, int y, int z);
void LineTracing();

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(9600);
}

void loop()
{
    LineTracing();
    prizm.PrizmEnd();
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
    while (true)
    {
        // D3 = true, D4 = true
        if (prizm.readLineSensor(3) && prizm.readLineSensor(4))
        {
            wheel(0, 0, 0);
            return;
        }

        // D3 = true, D4 = false
        else if (prizm.readLineSensor(3) && !prizm.readLineSensor(4))
        {
            wheel(0, -50, -10);
        }

        // D3 = false, D4 = true
        else if (!prizm.readLineSensor(3) && prizm.readLineSensor(4))
        {
            wheel(0, -50, 10);
        }

        // D3 = false, D4 = false
        else if (!prizm.readLineSensor(3) && !prizm.readLineSensor(4))
        {
            wheel(0, -50, 0);
        }
    }
}

// // 센서 값 읽는 함수, 아날로그는 a2 값 기준
// void collectSensor()
// {
//     analogRead(A1);
//     analogRead(A2);
//     prizm.readLineSensor(2);
//     prizm.readLineSensor(3);
//     prizm.readLineSensor(4);
// }
