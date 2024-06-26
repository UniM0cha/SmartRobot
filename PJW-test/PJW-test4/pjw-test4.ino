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

#define TRUE 1
#define FALSE 0

// 0 = 첫번째줄 / 1 = 두번째줄 / 2 = 세번째줄 / 3 = 네번째줄 / 4 = 다섯번째줄
// 현재 자신이 있는 줄
int currentLine = 0;

// 가운데 라인 기준 왼쪽 오른쪽
// 0 = 왼쪽 1 = 오른쪽
int currentLineFlag = 0;

// 처음 기둥 위치
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3, 노란색 = 4
int columnBlock[5][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}}; //한번 결정되면 그 자리 고정

// 오브젝트의 위치
// 없으면 = 0, 빨간색 = 1, 초록색 = 2, 파란색 = 3
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

// 배열에 저장된 기둥과 오브젝트의 합계
int columnCount = 0, objectCount = 0;

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
    finish(currentLineFlag);

    prizm.PrizmEnd();
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

void finish(int currentLineFlag)
{
    directionFind(currentLine, 0, currentLineFlag, currentLineFlag);
    if (currentLineFlag == 0)
    {
        wheel(50, 0, -1);
        delay(2400);
        wheel(0, 0, 0);
        delay(100);
        wheel(0, -50, 2);
        delay(1600);
        wheel(0, 0, 0);
        delay(100);
    }
    else
    {
        wheel(-50, 0, 1);
        delay(2400);
        wheel(0, 0, 0);
        delay(100);
        wheel(0, 50, -2);
        delay(1600);
        wheel(0, 0, 0);
        delay(100);
    }
}

/**
 * @brief 블록 가까이 다가가는 함수
 * 블록 바로 앞 : 7cm
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
                delay(10);
            }
        }
        break;
    }
}

/**
 * @brief 블록으로부터 멀어지는 함수
 * 교차로 <-> 블록 : 12cm
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
 * 회전하면서 currentLineFlag도 전환
 */
void turn()
{
    wheel(0, 0, 50);
    delay(1000);
    wheel(0, 0, 20);
    while (true)
    {
        if (analogRead(A1) > 200)
        {
            delay(300);
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
 * @param targetFace 봐야하는 방향
 */
void directionFind(int currentLine, int targetLine, int currentFace, int targetFace)
{
    Serial.println(String() + F("directionFind()\ncurrentLine : ") + currentLine + F("\ntargetLine : ") + targetLine + F("\ncurrentFace : ") + currentFace + F("\ndestFace : ") + targetFace);
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
    if (currentFace ^ targetFace)
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
    bool D2 = 0, D3 = 0, D4 = 0, D5 = 0;
    int a2, start, now;
    start = millis();

    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        // 3,0 1,1 일때는 뒷센서까지 사용하면 안된다.
        if ((currentLine == 3 && currentLineFlag == 0) || (currentLine == 1 && currentLineFlag == 1))
        {
        }
        else
        {
            D4 = prizm.readLineSensor(4);
            D5 = prizm.readLineSensor(5);
        }

        a2 = analogRead(A2);
        now = millis();

        if (a2 > 200 && now - start > 500)
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
        // D2 = true, D3 = false
        else if ((D2 && !D3) || (D4 && !D5))
        {
            wheel(-30, -5, -3);
        }

        // D2 = false, D3 = true
        else if ((!D2 && D3) || (!D4 && D5))
        {
            wheel(-30, 5, 5);
        }

        // D2 = false, D3 = false
        else if ((!D2 && !D3) || (D2 && D3) || (!D4 && !D5) || (D4 && D5))
        {
            wheel(-43, 0, 0.8);
        }
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 교차로 중간에 갈 때까지 라인트레이싱을 하면서 왼쪽 이동
 */
void leftLineTracing()
{
    bool D2 = 0, D3 = 0, D4 = 0, D5 = 0;
    int a2, start, now;
    start = millis();

    while (true)
    {
        D4 = prizm.readLineSensor(4);
        D5 = prizm.readLineSensor(5);
        // 1,0 3,1 일 때 뒷 센서까지 사용하면 안된다.
        if ((currentLine == 1 && currentLineFlag == 0) || (currentLine == 3 && currentLineFlag == 1))
        {
        }
        else
        {
            D2 = prizm.readLineSensor(2);
            D3 = prizm.readLineSensor(3);
        }
        a2 = analogRead(A2);
        now = millis();

        if (a2 > 200 && now - start > 500)
        {
            wheel(0, 0, 0);

            if (currentLineFlag == 0) // 왼쪽을 보고있을 경우엔 currentLine 증가
            {
                currentLine--;
            }
            else if (currentLineFlag == 1) // 오른쪽을 보고있을 경우엔 currentLine 감소
            {
                currentLine++;
            }

            delay(100);
            return;
        }

        //== 라인트레이싱 ==//
        // D4 = true, D5 = false
        else if ((D2 && !D3) || (D4 && !D5))
        {
            wheel(30, 5, -3);
        }

        // D4 = false, D5 = true
        else if ((!D2 && D3) || (!D4 && D5))
        {
            wheel(30, -5, 3);
        }

        // D4 = false, D5 = false
        else if ((!D2 && !D3) || (D2 && D3) || (!D4 && !D5) || (D4 && D5))
        {
            wheel(43, 0, -1);
        }
    }
}

/**
 * @brief 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
 */
void lineTracing()
{
    bool D2, D3;
    int a1, start, now;
    start = millis();

    while (true)
    {
        D2 = prizm.readLineSensor(2);
        D3 = prizm.readLineSensor(3);
        a1 = analogRead(A1);
        Serial.print("D2: ");
        Serial.print(D2);
        Serial.print(" / D3: ");
        Serial.print(D3);
        Serial.print(" / D4: ");
        Serial.println(a1);
        Serial.println(start);
        Serial.println(now);

        now = millis();

        if (a1 > 200 && now - start > 500)
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
            wheel(-20, 0, -9);
        }

        // D3 = false, D4 = true
        else if (!D2 && D3)
        {
            wheel(-20, 0, 9);
        }

        // D3 = false, D4 = false
        else if ((!D2 && !D3) || (D2 && D3))
        {
            wheel(-30, 0, 1);
        }
    }
}

/**
 * @brief 처음 배열을 채우는 함수
 */
void scanAll()
{
    // 다 못채웠을 경우를 대비하여 반복문
    while (true)
    {
        for (int i = 1; i <= 4; i++)
        {
            // 1번 줄에서 4번 줄까지
            directionFind(currentLine, i, currentLineFlag, 0);
            colorCheck();
        }

        // 조기종료
        if (objectCount == 3 && columnCount == 4)
            return;

        // 4번줄에서 턴
        directionFind(4, 4, 0, 1);
        colorCheck();

        // 조기종료
        if (objectCount == 3 && columnCount == 4)
            return;

        for (int i = 3; i >= 1; i--)
        {
            // 4번 줄에서 1번 줄까지
            directionFind(currentLine, i, currentLineFlag, 1);
            colorCheck();

            // 조기종료
            if (objectCount == 3 && columnCount == 4)
                return;
        }
        printObjectColumn();
        objectCount = 0;
        columnCount = 0;
        columnBlock[5][2] = {
            0,
        };
        objectBlock[5][2] = {
            0,
        };
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
        Serial.println(F("오브젝트 감지함"));
        // 하나만 감지 = 노란색 기둥
        if (huskylens.count() == 1)
        {
            columnBlock[currentLine][currentLineFlag] = 4;
            columnCount++;
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
                    columnCount++;
                    objectCount++;
                }
                else
                {
                    columnBlock[currentLine][currentLineFlag] = result2.ID;
                    objectBlock[currentLine][currentLineFlag] = result1.ID;
                    columnCount++;
                    objectCount++;
                }
                delay(100);
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

/**
 * @brief 노란색기둥말고 가장 앞쪽에서 가까운 기둥을 찾아서 이동하는 함수
 */
void findFirstColumn()
{
    /**
     * 지금 서있는곳의 기둥이 노란색이면,
     * 양쪽에 블록이 먼저 있는지 확인하고,
     * 둘 다 없으면 뒤쪽을 탐색한다.
     * 근데 탐색할 때 현재 줄이 4면 오버플로우가 발생한다.
     * 이거는 예외처리를 해줘야겠지.
     * 현재 줄이 4면 무조건적으로
     *
     * 지금 서있는곳의 기둥이 색깔 기둥이면,
     * 바로 잡는다.(return)
     *
     */

    if (columnBlock[currentLine][currentLineFlag] == YELLOW)
    {
        // 4번 줄에 있을 경우 둘중에 한쪽이 없다.
        if (currentLine == 4)
        {
            // 같은 방향 먼저
            if (columnBlock[3][currentLineFlag] != 0)
            {
                objectFlag = objectBlock[3][currentLineFlag];
                objectBlock[3][currentLineFlag] = 0;
                directionFind(currentLine, 3, currentLineFlag, currentLineFlag);
                return;
            }
            // 그 다음에 다른 방향
            else if (columnBlock[3][!currentLineFlag] != 0)
            {
                objectFlag = objectBlock[3][!currentLineFlag];
                objectBlock[3][!currentLineFlag] = 0;
                directionFind(currentLine, 3, currentLineFlag, !currentLineFlag);
                return;
            }
        }

        // 4번 줄이 아닐 경우 양쪽에 모두 있을 수 있다.
        else
        {
            if (columnBlock[currentLine + 1][currentLineFlag] != 0)
            {
                objectFlag = objectBlock[currentLine + 1][currentLineFlag];
                objectBlock[currentLine + 1][currentLineFlag] = 0;
                directionFind(currentLine, currentLine + 1, currentLineFlag, currentLineFlag);
                return;
            }
            else if (columnBlock[currentLine - 1][currentLineFlag] != 0)
            {
                objectFlag = objectBlock[currentLine - 1][currentLineFlag];
                objectBlock[currentLine - 1][currentLineFlag] = 0;
                directionFind(currentLine, currentLine - 1, currentLineFlag, currentLineFlag);
                return;
            }
            else if (columnBlock[currentLine + 1][!currentLineFlag] != 0)
            {
                objectFlag = objectBlock[currentLine + 1][!currentLineFlag];
                objectBlock[currentLine + 1][!currentLineFlag] = 0;
                directionFind(currentLine, currentLine + 1, currentLineFlag, !currentLineFlag);
                return;
            }
            else if (columnBlock[currentLine - 1][!currentLineFlag] != 0)
            {
                objectFlag = objectBlock[currentLine - 1][!currentLineFlag];
                objectBlock[currentLine - 1][!currentLineFlag] = 0;
                directionFind(currentLine, currentLine - 1, currentLineFlag, !currentLineFlag);
                return;
            }
        }
    }
    // 현재 보고있는 블록이 노란색이 아니면 바로 들어올리면 된다.
    else
    {
        objectFlag = objectBlock[currentLine][currentLineFlag];
        objectBlock[currentLine][currentLineFlag] = 0;
        return;
    }
}

/**
 * @brief 노란색 기둥을 찾아서 이동하는 함수
 */
void findYellowColumn()
{
    int forflag = 0, targetLine, targetLineFlag;
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
    int forflag = 0, targetLine, targetLineFlag;
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
    int forflag = 0, targetLine, targetLineFlag;
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

/**
 * @brief 그랩 초기화
 */
void startGrab()
{
    prizm.setMotorSpeed(2, 600);
    delay(600);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

/**
 * @brief 오브젝트 잡는 함수
 */
void objectGrab()
{
    prizm.setMotorSpeed(2, -600);
    delay(500);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

/**
 * @brief 오브젝트 놓는 함수
 */
void objectDrop()
{
    prizm.setMotorSpeed(2, 600);
    delay(500);
    prizm.setMotorSpeed(2, 0);
    delay(100);
}

//리프트 들기 함수
void lift_up()
{
    prizm.setMotorSpeed(1, -600);
    delay(800);
    prizm.setMotorSpeed(1, 0);
    delay(100);
}

//리프트 내리기 함수
void lift_down()
{
    prizm.setMotorSpeed(1, 600);
    delay(800);
    prizm.setMotorSpeed(1, 0);
    delay(100);
}