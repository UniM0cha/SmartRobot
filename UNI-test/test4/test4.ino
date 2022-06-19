/**
 *  허스키렌즈 학습 순서 : 빨간색(1), 초록색(2), 파란색(3), 노란색(4)
 * 아날로그 센서로 turn
 */

#include <PRIZM.h>
#include <stdio.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
HUSKYLENS huskylens;
// HUSKYLENS green line >> SDA; blue line >> SCL

PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

// 금쪽이면 1, 은쪽이면 2
#define GOLD 1
#define SILVER 2
#define ROBOT GOLD

#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4

#define TRUE 1
#define FALSE 0

// 현재 리프트 높이
int n = 0;
// 현재 Grab 넓이
int m = 0;

// 0 = 첫번째줄 / 1 = 두번째줄 / 2 = 세번째줄 / 3 = 네번째줄 / 4 = 다섯번째줄
// 현재 자신이 있는 줄
int currentLine = 0;
// 목적지 줄
int targetLine = 0;
// 가운데 라인 기준 왼쪽 오른쪽
// 0 = 왼쪽 1 = 오른쪽
int targetLineFlag = 0;
int currentLineFlag = 0;

// 처음 기둥 위치
// {1번줄{왼쪽, 오른쪽}, 2번줄{왼쪽, 오른쪽}, 3번줄{왼쪽, 오른쪽}, 4번줄{왼쪽, 오른쪽}, 5번줄{왼쪽, 오른쪽}}
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3, 노란색 = 4
// int columnBlock[5][2] = {
//     {0, 0},
//     {0, 4},
//     {1, 0},
//     {0, 2},
//     {0, 3}}; //한번 결정되면 그 자리 고정
int columnBlock[5][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}}; //한번 결정되면 그 자리 고정

// 오브젝트의 위치
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3
// int objectBlock[5][2] = {
//     {0, 0},
//     {0, 0},
//     {2, 0},
//     {0, 3},
//     {0, 1}};
int objectBlock[5][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}};

// 적재해야하는 오브젝트의 색 배열
// 빨간색 = 1, 초록색 = 2, 파란색 = 3
int objectColumnFlag[4] = {0, 0, 0, 0};
// 위 배열의 인덱스
int objectFlagCount = 0;

// 현재 잡고 있는 오브젝트의 색깔을 저장할 변수
int objectFlag = 0;

void setup()
{
    prizm.PrizmBegin();
    prizm.resetEncoder(1);
    Serial.begin(115200);
    Wire.begin();

    // 허스키렌즈 연결될 때까지 로깅
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }
    Serial.println(F("허스키렌즈 연결 완료"));
}

void loop()
{
    // printHusky();
    // frontLineTracing();
    // backLineTracing();
    // lift_up();
    // lift_down();
    // lift_up();
    // lift_down();
    // startGrab();
    // fowardToBlock();
    // delay(500);
    // backwardFromBlock();
    // Serial.println(prizm.readSonicSensorCM(A3));
    // delay(10);
    // turn();
    // rightLineTracing();
    // delay(1000);
    // leftLineTracing();

    start();
    scanAll();
    findFirstColumn();  // 노란색 기둥을 제외한 가장 앞 쪽 기둥 탐색 후 이동
    columnStack();      // 바로 앞에 있는 기둥의 색을 저장해놓기
    objectLiftup();     // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
    findYellowColumn(); // 노란색 기둥을 향해 이동
    objectLiftdown();   // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
    for (int i = 0; i < 3; i++)
    {
        Serial.println("for문");
        flagColorLine(objectColumnFlag[i]); // 처음기둥 색과 맞는 오브젝트를 찾아서 이동 //
        Serial.println("1-1");
        columnStack(); // 현재 서 있는 라인과 방향의 기둥색 가져오기
        Serial.println("1-2");
        objectLiftup(); // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
        Serial.println("1-3");
        findTargetColumn(objectFlag); // 잡고있는 오브젝트와 같은 색의 기둥을 찾아서 이동 //
        Serial.println("1-4");
        objectLiftdown(); // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
        Serial.println("1-5");
    }
    finish(); // TODO:

    prizm.PrizmEnd();
}

/**
 * @param x +좌 / -우
 * @param y +후 / -전
 * @param z +우회전 / -좌회전
 *
 * @brief
 * 은쪽이
 * 전직진 (0, -40, 0)
 * 후직진 (0, 40, 0)
 * 좌직진 (40, 0, -2)
 * 우직진 (-40, 0, 2)
 *
 * 금쪽이
 * 전직진 (0, -40, 2)
 * 후직진 (0, 40, -1)
 * 좌직진 (40, 0, -1)
 * 우직진 (-40, 0, 1)
 */
void wheel(int x, int y, int z)
{
    if (ROBOT == GOLD)
    {
        int A = 0, B = 0, C = 0, D = 0;

        A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
        B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
        C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
        D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

        exc1.setMotorPowers(1, A, B);
        exc2.setMotorPowers(2, C, D);
    }
    else if (ROBOT == SILVER)
    {
        int A = 0, B = 0, C = 0, D = 0;

        A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
        B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
        C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
        D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

        exc1.setMotorPowers(1, A, B);
        exc2.setMotorPowers(3, C, D);
    }
}

/**
 * @brief 시작지점 -> 경기장 이동하는 코드
 */
void start()
{
    startGrab();
    wheel(-30, -30, 0);
    delay(1200);
    wheel(-50, 0, 0);
    delay(500);

    // bool D2, crossFlag = false;
    // int a1, a2, start, now, v;
    bool D2;

    // 줄 만날 때까지 우직진
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        if (D2)
        {
            delay(300);
            break;
        }
    }

    // 교차로 만날 때까지 직진
    wheel(0, -40, 2);
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        if (D2 == true)
        {
            wheel(0, 0, 0);
            return;
        }
    }

    // 전진 라인트레이싱
    // while (true)
    // {
    //     a1 = analogRead(A1);
    //     a2 = analogRead(A2);
    //     D2 = prizm.readLineSensor(2);

    //     // 교차로 중간으로 가기 위한 타이머
    //     if (crossFlag == true)
    //     {
    //         now = millis();
    //         v = (ROBOT == SILVER ? 300 : 0);
    //         if (now - start >= v)
    //         {
    //             wheel(0, 0, 0);
    //             delay(100);
    //             wheel(-40, 0, 1);
    //             delay(1000);
    //             return;
    //         }
    //     }

    //     // 교차로 만나면 라인트레이싱 끝
    //     if (D2 == true)
    //     {
    //         crossFlag = true;
    //         start = millis();
    //     }

    //     //== 라인트레이싱 ==//
    //     // D4 = true, D5 = false
    //     else if (a1 > 200 && a2 <= 200)
    //     {
    //         if (ROBOT == SILVER)
    //         {
    //             wheel(0, -20, -8);
    //         }
    //         else if (ROBOT == GOLD)
    //         {
    //             wheel(0, -20, -9);
    //         }
    //     }

    //     // D4 = false, D5 = true
    //     else if (a1 <= 200 && a2 > 200)
    //     {
    //         if (ROBOT == SILVER)
    //         {
    //             wheel(0, -20, 4);
    //         }
    //         else if (ROBOT == GOLD)
    //         {
    //             wheel(0, -20, 9);
    //         }
    //     }

    //     // D4 = false, D5 = false
    //     else if ((a1 <= 200 && a2 <= 200) || (a1 > 200 && a2 > 200))
    //     {
    //         if (ROBOT == SILVER)
    //         {
    //             wheel(0, -40, -2);
    //         }
    //         else if (ROBOT == GOLD)
    //         {
    //             wheel(0, -40, 2);
    //         }
    //     }
    // }
}

void finish()
{
    directionFind(currentLine, 0, currentLineFlag, 0);
    wheel(50, 0, -1);
    delay(2400);
    wheel(0, 0, 0);
    delay(100);
    wheel(0, -50, 2);
    delay(1600);
    wheel(0, 0, 0);
    delay(100);
}

/**
 * @brief 블록 가까이 다가가는 함수
 * 교차로 <-> 블록 : 14cm
 * 블록 잡는 위치 : 8cm
 */
void fowardToBlock()
{
    int a3;
    bool D2;
    wheel(0, -40, 2);
    while (true)
    {
        a3 = prizm.readSonicSensorCM(A3);
        if (a3 <= 8)
        {
            wheel(0, 0, 0);
            return;
        }
        delay(10);
    }
}

/**
 * @brief 블록으로부터 멀어지는 함수
 * 교차로 <-> 블록 : 14cm
 * 블록 바로 앞 : 4cm
 */
void backwardFromBlock()
{
    int a3;
    bool D2;
    wheel(0, 40, -1);
    while (true)
    {
        a3 = prizm.readSonicSensorCM(A3);
        if (a3 >= 12)
        {
            wheel(0, 0, 0);
            return;
        }
        delay(10);
    }
}

/**
 * @brief 180도 회전하는 코드
 * 회전하면서 currentLineFlag도 전환
 */
void turn()
{
    wheel(0, 0, 50);
    delay(1000);
    wheel(0, 0, 30);
    while (true)
    {
        if (analogRead(A2) > 200)
        {
            if (ROBOT == SILVER)
            {
                delay(20);
            }
            else if (ROBOT == GOLD)
            {
                delay(100);
            }

            wheel(0, 0, 0);
            delay(100);
            currentLineFlag = !currentLineFlag;
            Serial.println(String() + F("currentLineFlag = ") + currentLineFlag);
            return;
        }
    }
}

/**
 * @brief 경로를 찾아서 이동하는 함수
 *
 * @param currentLine 현재 서있는 라인
 * @param targetLine 목적지 라인
 * @param currentFace 현재 보고있는 방향
 * @param destFace 봐야하는 방향
 */
void directionFind(int currentLine, int targetLine, int currentFace, int destFace)
{
    Serial.println(String() + F("directionFind()\ncurrentLine : ") + currentLine + F("\ntargetLine : ") + targetLine + F("\ncurrentFace : ") + currentFace + F("\ndestFace : ") + destFace);
    // 0번쪽을 보고 있으면 우측이동 = 줄 증가
    // 1번쪽을 보고 있으면 우측이동 = 줄 감소

    // move 음수 = 오른쪽 이동
    // move 양수 = 왼쪽 이동
    int move = 0;
    // 0번쪽을 보고 있을 때
    if (currentFace == 0)
    {
        move = currentLine - targetLine;
    }
    // 1번쪽을 보고 있을 때
    else
    {
        move = targetLine - currentLine;
    }
    int moveAbs = abs(move);

    // 지금 보는 방향과 봐야하는 방향이 다를 경우
    if (currentFace ^ destFace)
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
 * 이동하면서 currentLine 값도 수정함
 */
void rightLineTracing()
{
    bool D2, D3, crossFlag = false;
    int a2, start, now, v;

    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        a2 = analogRead(A2);

        // 교차로 중간으로 가기 위한 타이머
        if (crossFlag == true)
        {
            now = millis();
            v = (ROBOT == SILVER ? 350 : 30);
            if (now - start >= v)
            {
                wheel(0, 0, 0);
                delay(100);

                if (currentLineFlag == 0) // 왼쪽을 보고있을 경우엔 currentLine 증가
                {
                    currentLine++;
                }
                else if (currentLineFlag == 1) // 오른쪽을 보고있을 경우엔 currentLine 감소
                {
                    currentLine--;
                }
                return;
            }
        }

        // 교차로 만나면 타이머 작동
        if (a2 > 200)
        {
            crossFlag = true;
            start = millis();
        }

        //== 라인트레이싱 ==//
        // D2 = true, D3 = false
        else if (D2 && !D3)
        {
            if (ROBOT == SILVER)
            {
                wheel(-20, 0, -4);
            }
            else if (ROBOT == GOLD)
            {
                wheel(-20, 0, -9);
            }
        }

        // D2 = false, D3 = true
        else if (!D2 && D3)
        {
            if (ROBOT == SILVER)
            {
                wheel(-20, 0, 8);
            }
            else if (ROBOT == GOLD)
            {
                wheel(-20, 0, 9);
            }
        }

        // D2 = false, D3 = false
        else if ((!D2 && !D3) || (D2 && D3))
        {
            if (ROBOT == SILVER)
            {
                wheel(-40, 0, 2);
            }
            else if (ROBOT == GOLD)
            {
                wheel(-40, 0, 1);
            }
        }
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 교차로 중간에 갈 때까지 라인트레이싱을 하면서 왼쪽 이동
 */
void leftLineTracing()
{
    bool D4, D5, crossFlag = false;
    int a1, start, now, v;

    while (true)
    {
        D4 = prizm.readLineSensor(4);
        D5 = prizm.readLineSensor(5);
        a1 = analogRead(A1);

        // 교차로 중간으로 가기 위한 타이머
        if (crossFlag == true)
        {
            now = millis();
            v = (ROBOT == SILVER ? 300 : 30);
            if (now - start >= v)
            {
                wheel(0, 0, 0);
                delay(100);

                if (currentLineFlag == 0) // 왼쪽을 보고있을 경우엔 currentLine 증가
                {
                    currentLine--;
                }
                else if (currentLineFlag == 1) // 오른쪽을 보고있을 경우엔 currentLine 감소
                {
                    currentLine++;
                }
                return;
            }
        }

        // 교차로 만나면 라인트레이싱 끝
        if (a1 > 200)
        {
            crossFlag = true;
            start = millis();
        }

        //== 라인트레이싱 ==//
        // D4 = true, D5 = false
        else if (D4 && !D5)
        {
            if (ROBOT == SILVER)
            {
                wheel(20, 0, -8);
            }
            else if (ROBOT == GOLD)
            {
                wheel(20, 0, -9);
            }
        }

        // D4 = false, D5 = true
        else if (!D4 && D5)
        {
            if (ROBOT == SILVER)
            {
                wheel(20, 0, 4);
            }
            else if (ROBOT == GOLD)
            {
                wheel(20, 0, 9);
            }
        }

        // D4 = false, D5 = false
        else if ((!D4 && !D5) || (D4 && D5))
        {
            if (ROBOT == SILVER)
            {
                wheel(40, 0, -2);
            }
            else if (ROBOT == GOLD)
            {
                wheel(40, 0, 0);
            }
        }
    }
}

void printHusky()
{
    while (true)
    {
        Serial.println(F("Front Line Tracing..."));
        if (!huskylens.request())
            Serial.println(F("허스키렌즈와 연결 실패!"));
        else if (!huskylens.isLearned())
            Serial.println(F("학습되지 않음!"));
        else if (!huskylens.available())
        {
            Serial.println(F("오브젝트 감지하지 못함!"));
        }
        else
        {
            Serial.println(F("오브젝트 감지함"));
            // Serial.println(huskylens.count());  // 허스키렌즈에 감지된 오브젝트의 개수
            while (huskylens.available())
            {
                HUSKYLENSResult result = huskylens.read();
                if (result.command == COMMAND_RETURN_BLOCK)
                {
                    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
                }
            }
        }
    }
}

/**
 * @brief 처음 배열을 채우는 함수
 */
void scanAll()
{
    for (int i = 1; i <= 4; i++)
    {
        // 0번 줄에서 4번 줄까지
        directionFind(currentLine, i, currentLineFlag, 0);
        colorCheck();
    }

    // 4번줄에서 턴
    directionFind(4, 4, 0, 1);
    colorCheck();

    for (int i = 3; i >= 1; i--)
    {
        // 4번 줄에서 1번 줄까지
        directionFind(currentLine, i, currentLineFlag, 1);
        colorCheck();
    }
}

/**
 * @brief 허스키렌즈를 사용하여 색깔 판별 후 columnBlock과 objectBlock에 값 채워넣는 함수
 */
void colorCheck()
{
    Serial.println("Color Checking...");
    if (!huskylens.request())
        Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if (!huskylens.isLearned())
        Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if (!huskylens.available())
    {
        columnBlock[currentLine][currentLineFlag] = 0;
        objectBlock[currentLine][currentLineFlag] = 0;
    }
    else
    {
        Serial.println(F("###########"));
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            if (result.command == COMMAND_RETURN_BLOCK)
            {
                if (result.yCenter >= 160)
                {
                    columnBlock[currentLine][currentLineFlag] = result.ID;
                    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 기둥 배열상태 = ") + columnBlock[currentLine][currentLineFlag]);
                }
                if (result.yCenter < 160)
                {
                    objectBlock[currentLine][currentLineFlag] = result.ID;
                    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 오브젝트 배열상태 = ") + objectBlock[currentLine][currentLineFlag]);
                }
            }
            delay(100);
            // printResult(result);
        }
    }
}

/**
 * @brief 허스키 렌즈로 얻은 result를 시리얼 출력하는 함수
 */
void printResult(HUSKYLENSResult result)
{
    if (result.command == COMMAND_RETURN_BLOCK)
    {
        Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 오브젝트 배열상태 = ") + objectBlock[0][0] + F(", 기둥 배열상태 = ") + columnBlock[0][0]);
    }
    else
    {
        Serial.println("Object unknown!");
    }
}

/**
 * @brief objectBlock 배열에 들어있는 값 출력
 */
void printObjectColumn()
{
    Serial.println("오브젝트배열 : ");
    for (int j = 0; j <= 4; j++)
    {
        for (int k = 0; k <= 1; k++)
        {
            Serial.println(objectBlock[j][k]);
        }
    }
    Serial.println("기둥배열 : ");
    for (int j = 0; j <= 4; j++)
    {
        for (int k = 0; k <= 1; k++)
        {
            Serial.println(columnBlock[j][k]);
        }
    }
}

/**
 * @brief 노란색기둥말고 가장 앞쪽에서 가까운 기둥을 찾아서 이동하는 함수
 */
void findFirstColumn()
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            // 기둥이 빨간색, 초록색, 파란색인 경우
            if (columnBlock[i][j] == RED || columnBlock[i][j] == GREEN || columnBlock[i][j] == BLUE)
            {
                targetLine = i;
                targetLineFlag = j;
                objectFlag = objectBlock[i][j];
                objectBlock[i][j] = 0;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
        {
            break;
        }
    }
    directionFind(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

/**
 * @brief 노란색 기둥을 찾아서 이동하는 함수
 */
void findYellowColumn()
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (columnBlock[i][j] == YELLOW)
            {
                targetLine = i;
                targetLineFlag = j;
                objectBlock[i][j] = objectFlag;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
        {
            break;
        }
    }
    directionFind(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

/**
 * @brief 매개변수로 주어진 기둥을 찾아서 이동하는 함수
 *
 * @param targetColumn 찾을 기둥 값
 */
void findTargetColumn(int targetColumn)
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (columnBlock[i][j] == targetColumn)
            {
                targetLine = i;
                targetLineFlag = j;
                objectBlock[i][j] = targetColumn;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
        {
            break;
        }
    }
    directionFind(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

/**
 * @brief 매개변수로 주어진 오브젝트를 찾아서 이동하는 함수
 *
 * @param targetObject 찾을 오브젝트 값
 */
void flagColorLine(int targetObject)
{
    Serial.println(targetObject);
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (objectBlock[i][j] == targetObject)
            {
                targetLine = i;
                targetLineFlag = j;
                objectFlag = objectBlock[i][j];
                objectBlock[i][j] = 0;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
        {
            break;
        }
    }
    directionFind(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

/**
 * @brief 현재 바라보고 있는 오브젝트의 색깔을 판별하여
 * 다음으로 이동할 기둥 배열에 저장
 */
void columnStack()
{
    objectColumnFlag[objectFlagCount] = columnBlock[currentLine][currentLineFlag];
    Serial.println(objectColumnFlag[objectFlagCount]);
    objectFlagCount++;
}

void objectLiftup()
{
    fowardToBlock();
    objectGrab();
    lift_up(); // lift 아주 살짝 올려주기
    backwardFromBlock();
    // 오브젝트 리프트업을 위한 전진과 리프트업 하고 뒤에까지 오기
}

void objectLiftdown()
{
    fowardToBlock();
    lift_down(); // lift 올린만큼 내려주기
    objectDrop();
    backwardFromBlock();
    // 오브젝트 리프트다운을 위한 전진과 리프트다운 하고 뒤에까지 오기
}

/**
 * @brief 그랩 초기화
 */
void startGrab()
{
    prizm.setMotorSpeed(2, -600);
    delay(700);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

/**
 * @brief 오브젝트 잡는 함수
 */
void objectGrab()
{
    // TODO: 모터 회전
    prizm.setMotorSpeed(2, 600);
    delay(600);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

/**
 * @brief 오브젝트 놓는 함수
 */
void objectDrop()
{
    // TODO: 모터 회전
    prizm.setMotorSpeed(2, -600);
    delay(600);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

void lift_up()
{ //리프트 up 함수
    prizm.setMotorSpeed(1, -600);
    delay(400);
    prizm.setMotorSpeed(1, 0);
    delay(100);
}

void lift_down()
{ //리프트 down 함수
    prizm.setMotorSpeed(1, 600);
    delay(400);
    prizm.setMotorSpeed(1, 0);
    delay(100);
}