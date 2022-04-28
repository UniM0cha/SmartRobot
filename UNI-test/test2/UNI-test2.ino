/**
 * @file UNI-test2.ino
 * @author 박정우, 이정윤, 김승민, 전상현, 나기성, 양다운
 * @brief 두번째 Tetrix Smart Robot을 위한 코드 작성
 * @version 0.1
 * @date 2022-04-27
 *
 *  //== 연결된 센서 정보 ==//
 *  라인트레이싱에 사용되는 센서:
 *      왼쪽    : D3
 *      오른쪽  : D4
 *  칸 감지를 위한 센서:
 *      왼쪽    : D2
 *      오른쪽  : D5
 */

#include <PRIZM.h>
#include <stdio.h>
#include "Adafruit_TCS34725.h"
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

enum direction
{
    END,
    LEFT,
    RIGHT,
    CROSS
};

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
    direction dir = lineTracing();
    delay(500);
    if (dir == CROSS)
    {
        wheel(0, 0, 0);
        prizm.PrizmEnd();
    }
    else if (dir == LEFT)
    {
        turnLeft();
        delay(500);
    }
    else if (dir == RIGHT)
    {
        turnRight();
        delay(500);
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

/**
 * @brief
 * 교차로를 감지할 때까지 라인트레이싱 진행
 * 교차로를 발견하면 교차로의 중간까지 이동하고
 * 다음에 어디로 진행할 수 있는지 enum 타입인 direction 반환
 *
 * @return direction
 */
direction lineTracing()
{

    bool D2 = NULL;
    bool D3 = NULL;
    bool D4 = NULL;
    bool D5 = NULL;
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        D4 = prizm.readLineSensor(4);
        D5 = prizm.readLineSensor(5);

        //== 라인트레이싱 ==//
        // D3 = true, D4 = false
        if (D3 && !D4)
        {
            wheel(0, -30, -10);
        }

        // D3 = false, D4 = true
        else if (!D3 && D4)
        {
            wheel(0, -30, 10);
        }

        // D3 = false, D4 = false
        else if (!D3 && !D4)
        {
            wheel(0, -50, 0);
        }

        //== 교차로 감지 ==//
        // D2 또는 D5가 감지되면 앞으로 조금만 더 이동해서 한번 더 감지 (+자 모양 일수도 있기 때문)
        if (D2 || D5)
        {
            wheel(0, -50, 0);
            delay(10);
            D2 = prizm.readLineSensor(2);
            D5 = prizm.readLineSensor(5);
            delay(200);
            wheel(0, 0, 0);

            if (D2 && D5)
            {
                return CROSS;
            }
            else if (D2 && !D5)
            {
                return LEFT;
            }
            else if (!D2 && D5)
            {
                return RIGHT;
            }
            else
            {
                Serial.println("오류");
                prizm.PrizmEnd();
            }
        }
    }
}

void turnLeft()
{
    wheel(0, 0, -30);
    delay(1000);
    while (true)
    {
        if (prizm.readLineSensor(3))
        {
            delay(50);
            wheel(0, 0, 0);
            return;
        }
    }
}

void turnRight()
{
    wheel(0, 0, 30);
    delay(1000);
    while (true)
    {
        if (prizm.readLineSensor(4))
        {
            delay(130);
            wheel(0, 0, 0);
            return;
        }
    }
    return;
}