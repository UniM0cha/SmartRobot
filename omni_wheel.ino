#include <PRIZM.h>
#include <stdio.h>
#include "Adafruit_TCS34725.h"
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(9600);
}

void loop()
{
    // 직진
    //
    wheel(0, -60, 0); //오브젝트 적재를 위한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);
    wheel(0, 60, 0); //오브젝트 적재를 위한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);

    wheel(-60, 0, 0); //오브젝트 적재를 위 한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);
    wheel(60, 0, 0); //오브젝트 적재를 위 한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);

    wheel(-60, -60, 0); //오브젝트 적재를 위한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);
    wheel(60, 60, 0); //오브젝트 적재를 위한 전진
    delay(2000);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진
    delay(500);

    wheel(0, 0, 60); //오브젝트 적재를 위한 전진
    delay(2600);
    wheel(0, 0, 0); //오브젝트 적재를 위한 전진

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
