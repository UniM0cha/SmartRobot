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

enum CrossType
{
    CROSS,
    LEFT,
    RIGHT
};

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(9600);
}

void loop()
{

    int a1 = prizm.readSonicSensorCM(A1);
    Serial.println(a1);

    // // 대충 함수 사용법
    // start();
    // findNextCross();
    // fowardToBlock();
    // delay(1000);
    // backwardFromBlock();
    // delay(1000);
    // turnRight();
    // turnRight();
    // findNextCross();
    // prizm.PrizmEnd();
}

/**
 * @param x +좌 / -우
 * @param y +후 / -전
 * @param z +우회전 / -좌회전
 */
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
 * @brief 교차로를 감지할 때까지 라인트레이싱 진행
 */
void lineTracing()
{
    bool D2, D3, D4, D5;
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
            wheel(-20, 0, -9);
        }

        // D3 = false, D4 = true
        else if (!D3 && D4)
        {
            wheel(-20, 0, 9);
        }

        // D3 = false, D4 = false
        else if (!D3 && !D4)
        {
            wheel(-40, 0, 0);
        }

        // 교차로 만나면 라인트레이싱 끝
        if (D2 || D5)
        {
            wheel(0, 0, 0);
            return;
        }
    }
}

/**
 * @brief 만난 교차로가 어떤 모양의 교차로인지 반환하고 교차로의 중간으로 이동
 *
 * @return CrossType
 */
CrossType checkCross()
{
    bool D2 = false, D5 = false, left = false, right = false;

    int start = millis();
    int end = millis();

    // 교차로의 중간까지 이동하면서 교차로 탐색
    wheel(-40, 0, 0);
    while (end - start <= 400)
    {
        end = millis();
        D2 = prizm.readLineSensor(2);
        D5 = prizm.readLineSensor(5);
        if (D2)
            left = true;
        if (D5)
            right = true;
    }
    wheel(0, 0, 0);
    delay(100);

    if (left && right)
    {
        return CROSS;
    }
    else if (left && !right)
    {
        return LEFT;
    }
    else if (!left && right)
    {
        return RIGHT;
    }
    else
    {
        Serial.println("오류");
        prizm.PrizmEnd();
    }
}

/**
 * @brief 수직인 선을 만날 때까지 왼쪽 회전
 */
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
            delay(100);
            return;
        }
    }
}

/**
 * @brief 수직인 선을 만날 때까지 오른쪽 회전
 */
void turnRight()
{
    wheel(0, 0, 30);
    delay(1000);
    while (true)
    {
        if (prizm.readLineSensor(4))
        {
            delay(150);
            wheel(0, 0, 0);
            delay(100);
            return;
        }
    }
    return;
}

/**
 * @brief 시작지점 -> 경기장 이동하는 코드
 */
void start()
{
    wheel(-30, -30, 0);
    delay(1200);
    wheel(-40, 0, 0);
    bool D3 = false;
    while (true)
    {
        D3 = prizm.readLineSensor(3);
        if (D3)
        {
            delay(300);
            wheel(0, -40, 0);
            break;
        }
    }

    bool D2 = false;
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        if (D2)
        {
            delay(100);
            wheel(0, 0, 0);
            break;
        }
    }
}

/**
 * @brief 다음 교차로까지 이동하는 함수
 */
CrossType findNextCross()
{
    lineTracing();
    return checkCross();
}

/**
 * @brief 블록 가까이 다가가는 함수
 * 교차로 <-> 블록 : 14cm
 * 블록 바로 앞 : 4cm
 */
void fowardToBlock()
{
    wheel(0, -30, 0);
    int a1 = prizm.readSonicSensorCM(A1);
    while (true)
    {
        // 초음파센서 감지
        a1 = prizm.readSonicSensorCM(A1);
        if (a1 <= 4)
        {
            wheel(0, 0, 0);
            break;
        }
    }
}

/**
 * @brief 블록으로부터 멀어지는 함수
 * 교차로 <-> 블록 : 14cm
 * 블록 바로 앞 : 4cm
 */
void backwardFromBlock()
{
    wheel(0, 30, 0);
    int a1 = prizm.readSonicSensorCM(A1);
    while (true)
    {
        // 초음파센서 감지
        a1 = prizm.readSonicSensorCM(A1);
        if (a1 >= 14)
        {
            wheel(0, 0, 0);
            break;
        }
    }
}