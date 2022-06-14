#include <PRIZM.h>
#include <stdio.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
HUSKYLENS huskylens;
// HUSKYLENS green line >> SDA; blue line >> SCL
void printResult(HUSKYLENSResult result);

PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

#define RED 1
#define GREEN 2
#define BLUE 3

#define RIGHT 1
#define LEFT -1
#define STOP 0

#define TRUE 1
#define FALSE 0

// 현재 리프트 높이
int n = 0;

// 0 = 첫번째줄 / 1 = 두번째줄 / 2 = 세번째줄 / 3 = 네번째줄 / 4 = 다섯번째줄
// 현재 자신이 있는 줄
int currentLine = 0;
// 목적지 줄
int targetLine = 0;
// 가운데 라인 기준 왼쪽 오른쪽
// 0 = 왼쪽 1 = 오른쪽
int targetLineFlag = 0;
int currentLineFlag = 0;

int objectFlagCount = 0;

int objectFlag = 0;

int A = 0;

// 처음 기둥 위치
// {1번줄{왼쪽, 오른쪽}, 2번줄{왼쪽, 오른쪽}, 3번줄{왼쪽, 오른쪽}, 4번줄{왼쪽, 오른쪽}, 5번줄{왼쪽, 오른쪽}}
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3, 노란색 = 4
int columnBlock[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}; //한번 결정되면 그 자리 고정

// 오브젝트의 위치
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3
int objectBlock[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

// 적재해야하는 오브젝트의 색 배열
// 빨간색 = 1, 초록색 = 2, 파란색 = 3
int objectColumnFlag[4] = {0, 0, 0, 0};

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(115200);
    Wire.begin();
    // while (!huskylens.begin(Wire))
    // {
    //   Serial.println(F("Begin failed!"));
    //   Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    //   Serial.println(F("2.Please recheck the connection."));
    //   delay(100);
    // }
}

void loop()
{
    // start();
    rightLineTracing();
    // leftLineTracing();
    Direction_find(0, 3, 0, 1);
    Direction_find(3, 4, 1, 1);
    Direction_find(4, 0, 1, 0);
    prizm.PrizmEnd();
    // Serial.println(analogRead(A1));
}

/**
 * 은쪽이
 * @param x +좌 / -우
 * @param y +후 / -전
 * @param z +우회전 / -좌회전
 *
 */
void wheel(int x, int y, int z)
{
    int A = 0, B = 0, C = 0, D = 0;

    A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
    B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
    C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
    D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

    exc1.setMotorPowers(1, A, B);
    exc2.setMotorPowers(3, C, D);
}

// /**
//  * 금쪽이
//  * @param x +좌 / -우
//  * @param y +후 / -전
//  * @param z +우회전 / -좌회전
//  */
// void wheel(int x, int y, int z)
// {
//     int A = 0, B = 0, C = 0, D = 0;

//     A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
//     B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
//     C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
//     D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

//     exc1.setMotorPowers(1, A, B);
//     exc2.setMotorPowers(2, C, D);
// }

/**
 * @brief 시작지점 -> 경기장 이동하는 코드
 */
void start()
{
    wheel(-30, -30, 0);
    delay(1200);
    wheel(-50, 0, 0);
    delay(500);
    bool D2 = false;
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        if (D2)
        {
            delay(300);
            wheel(0, -50, 0);
            break;
        }
    }

    D2 = false;
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        if (D2)
        {
            // delay(100);
            wheel(0, 0, 0);
            break;
        }
    }
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

/**
 * @brief 180도 회전하는 코드
 */
void turn()
{
    wheel(0, 0, 50);
    delay(900);
    wheel(0, 0, 30);
    while (true)
    {
        if (prizm.readLineSensor(3))
        {
            delay(150);
            wheel(0, 0, 0);
            delay(100);
            return;
        }
    }
}

/**
 * @brief 경로를 찾아서 이동하는 함수
 *
 * @param now_line 현재 서있는 라인
 * @param dest_line 목적지 라인
 * @param now_face 현재 보고있는 방향
 * @param dest_face 봐야하는 방향
 */
void Direction_find(int now_line, int dest_line, int now_face, int dest_face)
{
    // 0번쪽을 보고 있으면 우측이동 = 줄 증가
    // 1번쪽을 보고 있으면 우측이동 = 줄 감소

    // move 음수 = 오른쪽 이동
    // move 양수 = 왼쪽 이동
    int move = 0;
    // 0번쪽을 보고 있을 때
    if (now_face == 0)
    {
        move = now_line - dest_line;
    }
    // 1번쪽을 보고 있을 때
    else
    {
        move = dest_line - now_line;
    }
    int moveAbs = abs(move);

    // 지금 보는 방향과 봐야하는 방향이 다를 경우
    if (now_face ^ dest_face)
    {
        // 우측 이동
        if (move < 0)
        {
            for (int i = 0; i < moveAbs; i++)
            {
                rightLineTracing();
            }
        }

        // 좌측 이동
        else if (move > 0)
        {
            for (int i = 0; i < moveAbs; i++)
            {
                leftLineTracing();
            }
        }
        turn();
        return;
    }

    // 지금 보는 방향과 봐야하는 방향이 같은 경우
    else
    {
        // 우측 이동
        if (move < 0)
        {
            for (int i = 0; i < moveAbs; i++)
            {
                rightLineTracing();
            }
        }

        // 좌측 이동
        else if (move > 0)
        {
            for (int i = 0; i < moveAbs; i++)
            {
                leftLineTracing();
            }
        }
        return;
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 교차로 중간에 갈 때까지 라인트레이싱을 하면서 오른쪽 이동
 */
void rightLineTracing()
{
    bool D2, D3;
    int a1;

    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        a1 = analogRead(A1);

        // 교차로 만나면 라인트레이싱 끝
        if (a1 > 300)
        {
            wheel(-40, 0, 0);
            delay(350);
            wheel(0, 0, 0);
            delay(100);
            return;
        }

        //== 라인트레이싱 ==//
        // D2 = true, D3 = false
        else if (D2 && !D3)
        {
            wheel(-20, 0, -9);
        }

        // D2 = false, D3 = true
        else if (!D2 && D3)
        {
            wheel(-20, 0, 9);
        }

        // D2 = false, D3 = false
        else if (!D2 && !D3)
        {
            wheel(-40, 0, 0);
        }
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 교차로 중간에 갈 때까지 라인트레이싱을 하면서 왼쪽 이동
 */
void leftLineTracing()
{
    bool D4, D5;
    int a2;

    while (true)
    {
        D4 = prizm.readLineSensor(4);
        D5 = prizm.readLineSensor(5);
        a2 = analogRead(A2);

        // 교차로 만나면 라인트레이싱 끝
        if (a2 > 300)
        {
            wheel(40, 0, 0);
            delay(420);
            wheel(0, 0, 0);
            delay(100);
            return;
        }

        //== 라인트레이싱 ==//
        // D4 = true, D5 = false
        else if (D4 && !D5)
        {
            wheel(20, 0, -9);
        }

        // D4 = false, D5 = true
        else if (!D4 && D5)
        {
            wheel(20, 0, 9);
        }

        // D4 = false, D5 = false
        else if (!D4 && !D5)
        {
            wheel(40, 0, 0);
        }
    }
}

void scanAll()
{
    for (int i = 0; i < 4; i++)
    {
        Direction_find(i, i + 1, 0, 0);
    }
    Direction_find(4, 4, 0, 1);
    for (int i = 4; i > 1; i--)
    {
        Direction_find(i, i - 1, 1, 1);
    }
}