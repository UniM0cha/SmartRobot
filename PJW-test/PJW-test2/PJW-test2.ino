#include "HUSKYLENS.h"
#include <stdio.h>
#include "SoftwareSerial.h"
#include <PRIZM.h>
HUSKYLENS huskylens;
// HUSKYLENSResult Result_data;
PRIZM prizm;
// EXPANSION ex;
// HUSKYLENS green line >> SDA; blue line >> SCL
void printResult(HUSKYLENSResult result);
int8_t turn_speed = 0;
int xLeft = 160 - 40;
int xRight = 160 + 40;
void wheel(int x, int y, int z) // Omni wheel 이동공식
{
    int A = 0, B = 0, C = 0, D = 0;
    A = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471 * -1);
    B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471 * -1);
    C = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471 * -1);
    D = (x * 0.5) + (y * 0.5) + (z * 0.841471 * -1);
    // ex.setMotorPowers(1, A, B);
    // ex.setMotorPowers(2, C, D);
}

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(115200);
    Wire.begin();

    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }
}

void loop()
{
    if (!huskylens.request())
    {
    }
    else if (!huskylens.available())
    {
        wheel(0, 0, 0);
        Serial.println("Wheel 000 if");
    }
    else
    {
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            Serial.println(result.xCenter);
            // Serial.println(re sult.xCenter);
            Serial.println("------------------");
            if (result.xCenter > xRight && result.xCenter < xLeft)
            {
                Serial.println("if 1");
                turn_speed = 0;
            }
            else
            {
                Serial.println("else 1");
                turn_speed = (result.xCenter - 160) / 4;
            }
            Serial.println("while 00");
            wheel(0, 0, turn_speed);
        }
    }
}