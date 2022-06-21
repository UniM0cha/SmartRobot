#include <PRIZM.h>
#include <stdio.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
HUSKYLENS huskylens;
// HUSKYLENS green line >> SDA; blue line >> SCL
void printResult(HUSKYLENSResult result);
int xLeft = 160 - 40;
int xRight = 160 + 40;
int8_t turn_speed = 0;

PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4

// 금쪽이면 1, 은쪽이면 2
#define GOLD 1
#define SILVER 2
#define ROBOT GOLD

#define RIGHT 1
#define LEFT -1
#define STOP 0

#define TRUE 1
#define FALSE 0

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

// 오브젝트랑 기둥 색 다 잡으면 그 즉시 돌아오게 하기

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
    scanAll();
    //  int D2 = prizm.readLineSensor(2);
    //  int D3 = prizm.readLineSensor(3);
    //  int D4 = prizm.readLineSensor(4);
    //  Serial.print("D2: ");
    //  Serial.print(D2);
    //  Serial.print(" / D3: ");
    //  Serial.print(D3);
    //  Serial.print(" / D4: ");
    //  Serial.println(D4);
    //    start();
    // scanAll();
    //    findFirstColumn();  // 노란색 기둥을 제외한 가장 앞 쪽 기둥 탐색 후 이동
    //    columnStack();      // 바로 앞에 있는 기둥의 색을 저장해놓기
    //    objectLiftup();     // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
    //    findYellowColumn(); // 노란색 기둥을 향해 이동
    //    objectLiftdown();   // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
    //    for (int i = 0; i < 3; i++)
    //    {
    //        Serial.println("for문");
    //        flagColorLine(objectColumnFlag[i]); // 처음기둥 색과 맞는 오브젝트를 찾아서 이동 //
    //        Serial.println("1-1");
    //        columnStack(); // 현재 서 있는 라인과 방향의 기둥색 가져오기
    //        Serial.println("1-2");
    //        objectLiftup(); // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
    //        Serial.println("1-3");
    //        findTargetColumn(objectFlag); // 잡고있는 오브젝트와 같은 색의 기둥을 찾아서 이동 //
    //        Serial.println("1-4");
    //        objectLiftdown(); // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
    //        Serial.println("1-5");
    //    }
    //    finish(); // TODO:
    //    prizm.PrizmEnd();
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
    exc2.setMotorPowers(3, C, D);
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
 * 블록 바로 앞 : 4cm
 */
void fowardToBlock()
{
    while (1)
    {
        if (!huskylens.request())
        {
        }
        else if (!huskylens.available())
        {
            wheel(0, 0, 0);
        }
        else
        {
            while (huskylens.available())
            {
                int a3 = prizm.readSonicSensorCM(A3);
                HUSKYLENSResult result = huskylens.read();
                if (result.xCenter > xRight && result.xCenter < xLeft)
                {
                    turn_speed = 0;
                }
                else
                {
                    turn_speed = (result.xCenter - 160) / 4;
                }
                wheel(0, -30, turn_speed);
                if (a3 <= 7)
                {
                    wheel(0, 0, 0);
                    break;
                }
            }
        }
        break;
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
 */
void turn()
{
    wheel(0, 0, 50);
    delay(1000);
    wheel(0, 0, 30);
    while (true)
    {
        if (analogRead(A1) > 200)
        {
            if (ROBOT == SILVER)
            {
                delay(20);
            }
            else if (ROBOT == GOLD)
            {
                delay(200);
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
 * @param now_line 현재 서있는 라인
 * @param next 목적지 라인
 * @param currentFlag 현재 보고있는 방향
 * @param targetFlag 봐야하는 방향
 */
void directionFind(int now_line, int next, int currentFlag, int targetFlag)
{
    // 0번쪽을 보고 있으면 우측이동 = 줄 증가
    // 1번쪽을 보고 있으면 우측이동 = 줄 감소

    int moveCnt;
    // 0번쪽을 보고 있을 때
    if (!currentFlag)
    {
        moveCnt = next - now_line;
    }
    // 1번쪽을 보고 있을 때
    else
    {
        moveCnt = now_line - next;
    }
    int absCnt = abs(moveCnt);

    // 지금 보는 방향과 봐야하는 방향이 다를 경우
    if (currentFlag ^ targetFlag)
    {
        // 우측 이동
        if (moveCnt > 0)
        {
            for (int i = 0; i < absCnt; i++)
            {
                lineTracing();
            }
            turn();
            return;
        }

        // 좌측 이동
        else if (moveCnt < 0)
        {
            turn();
            for (int i = 0; i < absCnt; i++)
            {
                lineTracing();
            }
            return;
        }
    }

    // 지금 보는 방향과 봐야하는 방향이 같은 경우
    else
    {
        // 우측 이동
        if (moveCnt >= 0)
        {
            for (int i = 0; i < absCnt; i++)
            {
                lineTracing();
            }
            return;
        }

        // 좌측 이동
        else
        {
            turn();
            for (int i = 0; i < absCnt; i++)
            {
                lineTracing();
            }
            turn();
            return;
        }
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
 */
void lineTracing()
{
    bool D2, D3, D4;
    int a2, start, now, v;
    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        D4 = prizm.readLineSensor(4);
        now = millis();

        Serial.print("d2 = ");
        Serial.println(D2);
        Serial.print("d3 = ");
        Serial.println(D3);
        Serial.print("d4 = ");
        Serial.println(D4);

        if (D4 && now - start > 300)
        {
            wheel(0, 0, 0);

            if (currentLineFlag == 0) // 왼쪽을 보고있을 경우엔 currentLine 증가
            {
                currentLine++;
            }
            else if (currentLineFlag == 1) // 오른쪽을 보고있을 경우엔 currentLine 감소
            {
                currentLine--;
            }

            delay(100);
            return;
        }

        //== 라인트레이싱 ==//
        // D3 = true, D4 = false
        if (D2 && !D3)
        {
            wheel(-30, 0, -9);
        }

        // D3 = false, D4 = true
        else if (!D2 && D3)
        {
            wheel(-30, 0, 9);
        }

        // D3 = false, D4 = false
        else if ((!D2 && !D3) || (D2 && D3))
        {
            wheel(-40, 0, 0);
        }
    }
}

void scanAll()
{
    for (int i = 1; i <= 4; i++)
    {
        Serial.println("for 1");
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
    printObjectColumn();
}
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
        // 하나만 감지 = 노란색 기둥
        if (huskylens.count() == 1)
        {
            columnBlock[currentLine][currentLineFlag] = 4;
        }
        else
        {
            while (huskylens.available())
            {
                HUSKYLENSResult result1 = huskylens.read();
                HUSKYLENSResult result2 = huskylens.read();
                if (result1.yCenter >= result2.yCenter)
                {
                    columnBlock[currentLine][currentLineFlag] = result1.ID;
                    objectBlock[currentLine][currentLineFlag] = result2.ID;
                }
                else
                {
                    columnBlock[currentLine][currentLineFlag] = result2.ID;
                    objectBlock[currentLine][currentLineFlag] = result1.ID;
                }
                // if (result.yCenter >= 160)
                // {
                //     columnBlock[currentLine][currentLineFlag] = result.ID;
                //     Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 기둥 배열상태 = ") + columnBlock[currentLine][currentLineFlag]);
                // }
                // if (result.yCenter < 160)
                // {
                //     objectBlock[currentLine][currentLineFlag] = result.ID;
                //     Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 오브젝트 배열상태 = ") + objectBlock[currentLine][currentLineFlag]);
                // }
                delay(100);
                // printResult(result);
            }
        }
    }
}

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
    lift_up();
    backwardFromBlock();
    // 오브젝트 리프트업을 위한 전진과 리프트업 하고 뒤에까지 오기
}

void objectLiftdown()
{
    fowardToBlock();
    lift_down();
    objectDrop();
    backwardFromBlock();
    // 오브젝트 리프트다운을 위한 전진과 리프트다운 하고 뒤에까지 오기
}

void startGrab()
{
    prizm.setMotorSpeed(2, 600);
    delay(600);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

void objectGrab()
{
    // TODO: 모터 회전
    prizm.setMotorSpeed(2, -600);
    delay(500);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

void objectDrop()
{
    // TODO: 모터 회전
    prizm.setMotorSpeed(2, 600);
    delay(500);
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