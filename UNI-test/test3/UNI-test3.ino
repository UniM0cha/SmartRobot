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

// 센서값을 저장할 전역변수
int a1, a2, D2, D3, D4, diff;

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
    // Direction_find(4, 3, 1, 1);
    start();
    scanAll();
    prizm.PrizmEnd();

    //  start();              // 구현해야함 //정윤이가 구현해놓음
    //  qodufcodnrl();        // 카메라 모듈값 받아야함 // 카메라 관련 새로운 함수 만들기 // 계속 테스트 해보기
    //  shfkstorakfrhwjrwo(); // 노란색 기둥을 제외한 가장 끝 쪽 기둥 탐색 후 이동 //
    //  vkseks();             // 바로 앞에 있는 기둥의 색을 저장해놓기 //
    //  objectLiftup();       // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수 // 세부 조절
    //  findYellowColumn();   // 노란색 기둥을 향해 이동 //
    //  objectLiftdown();     // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
    //  for (int i = 0; i < 3; i++)
    //  {
    //   flagColorLine(objectColumnFlag[i]);     // 처음기둥 색과 맞는 오브젝트를 찾아서 이동 //
    //    vkseks();                              // 현재 서 있는 라인과 방향의 기둥색 가져오기
    //    objectLiftup();                        // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
    //    findTargetColumn(objectFlag);          // 잡고있는 오브젝트와 같은 색의 기둥을 찾아서 이동 //
    //    objectLiftdown();                      // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
    //  }
    //  finish(); // 구현해야함
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
 * @brief 시작지점 -> 경기장 이동하는 코드
 */
void start()
{
    wheel(-30, -30, 0);
    delay(1200);
    wheel(-50, 0, 0);
    delay(500);
    bool D3 = false;
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
void findNextCross()
{
    lineTracing();
    checkCross();
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
        if (prizm.readLineSensor(4))
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
 * @param next 목적지 라인
 * @param currentFlag 현재 보고있는 방향
 * @param targetFlag 봐야하는 방향
 */
void Direction_find(int now_line, int next, int currentFlag, int targetFlag)
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
        if (moveCnt >= 0)
        {
            for (int i = 0; i < absCnt; i++)
            {
                findNextCross();
            }
            turn();
            return;
        }

        // 좌측 이동
        else
        {
            turn();
            for (int i = 0; i < absCnt; i++)
            {
                findNextCross();
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
                findNextCross();
            }
            return;
        }

        // 좌측 이동
        else
        {
            turn();
            for (int i = 0; i < absCnt; i++)
            {
                findNextCross();
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
 */
void checkCross()
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

    if (left || right)
    {
        return;
    }
    else
    {
        Serial.println("오류");
        prizm.PrizmEnd();
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