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
    collectSensor();
}

void loop()
{
    Direction_find(3, 0, 0, 0);
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
 * @brief 수직인 선을 만날 때까지 왼쪽 회전
 */
// void turnLeft()
// {
//   wheel(0, 0, -30);
//   delay(1000);
//   while (true)
//   {
//     if (prizm.readLineSensor(3))
//     {
//       delay(50);
//       wheel(0, 0, 0);
//       delay(100);
//       return;
//     }
//   }
// }

/**
 * @brief 수직인 선을 만날 때까지 오른쪽 회전
 */
// void turnRight()
// {
//   wheel(0, 0, 30);
//   delay(1000);
//   while (true)
//   {
//     if (prizm.readLineSensor(4))
//     {
//       delay(150);
//       wheel(0, 0, 0);
//       delay(100);
//       return;
//     }
//   }
//   return;
// }

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

void findRightLine()
{
    Serial.println("Find Right Line...");
    collectSensor();
    wheel(-45, 0, 0);
    delay(200);
    while (1)
    {
        collectSensor();
        wheel(-45, 0, 0);
        if (D2 == HIGH)
        {
            Serial.println("Line Found");
            wheel(45, 0, 0);
            delay(230);
            wheel(0, 0, 0);
            delay(200);
            currentLine++;
            break;
        }
    }
}

void findLeftLine()
{
    collectSensor();
    Serial.println("Find Left Line...");
    wheel(45, 0, 0);
    delay(200);
    while (1)
    {
        collectSensor();
        wheel(45, 0, 0);
        if (D2 == HIGH)
        {
            Serial.println("Line Found");
            wheel(-45, 0, 0);
            delay(230);
            wheel(0, 0, 0);
            delay(200);
            currentLine--;
            break;
        }
    }
}

void lift_up(int s)
{ //리프트 up 함수
    n += s;
    prizm.setMotorSpeed(1, -600);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}

void lift_down(int s)
{ //리프트 down 함수
    n -= s;
    prizm.setMotorSpeed(1, 600);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}

// // 뒤로 돌아서 중간 맞추는 함수
// void turn()
// {
//   int errorRange = 600;
//   // 처음엔 빠른 속도로 돌다가
//   wheel(0, 0, 40);
//   delay(1200);
//   // 느린속도로 센서 감지
//   wheel(0, 0, 25);
//   while (1)
//   {
//     collectSensor();
//     // 아날로그 센서 사용
//     if (a2 - errorRange >= a1)
//     {
//       wheel(0, 0, 0);
//       break;
//     }
//   }
//   center();
// }

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
    // 양수면 우측으로 이동, 음수면 좌측으로 이동인데, 좌측 이동이라면 회전 후 이동해야함 (우측 이동만 가능하기 때문)
    int moveCnt = next - now_line;
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
        }

        // 좌측 이동
        else
        {
            turn();
            for (int i = 0; i < absCnt; i++)
            {
                findNextCross();
            }
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
        }
    }
}

void Direction_move(int direc, int cnt)
{ //목적지 라인으로 이동하는 함수
    for (int i = 0; i < cnt; i++)
    { //이동 횟수에 따른 반복문
        if (direc == 1)
        {
            findRightLine();
        }
        else if (direc == -1)
        {
            findLeftLine();
        }
        else
        {
            wheel(0, 0, 0);
        }
        delay(300);
    }
    wheel(0, 0, 0);
}

// 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
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

// 뒤로 간다음 중간 맞춤
void back(int time)
{
    Serial.println("back...");
    wheel(0, 50, 0);
    delay(time);
    wheel(0, 0, 0);
    center();
    //  if (currentFlag == 0)
    //  {
    //    currentLineFlag = 1;
    //  }
    //  else
    //  {
    //    currentLineFlag = 0;
    //  }
}

// 가운데 맞추는 코드
void center()
{
    int errorRange = 80;
    int onLine = 250;      // 아날로그 센서가 줄 위에 있을 때의 센서 최소값
    int checkTime = 500;   // 이 숫자를 올리면 줄을 찾는 반경이 넓어짐
    bool isCenter = false; // 중앙(D2) 확인
    bool isMiddle = false; // 중간(a1, a2)확인

    collectSensor();

    //== 1. 중앙 맞추기 ==//
    while (!isCenter)
    {
        if (D2 == HIGH)
        {
            isCenter = true;
        }
        else if (D2 == LOW)
        {
            // 오른쪽 탐색
            int start = millis();
            int end = millis();
            while (1)
            {
                end = millis();
                if (end - start >= checkTime || isCenter)
                {
                    wheel(0, 0, 0);
                    break;
                }
                wheel(30, 0, 0);
                collectSensor();
                if (D2 == HIGH)
                {
                    isCenter = true;
                }
            }

            // 왼쪽 탐색
            start = millis();
            end = millis();
            while (1)
            {
                end = millis();
                if (end - start >= checkTime * 2 || isCenter)
                {
                    wheel(0, 0, 0);
                    break;
                }
                wheel(-30, 0, 0);
                collectSensor();
                if (D2 == HIGH)
                {
                    isCenter = true;
                }
            }
        }
    }

    //== 2. 중간 맞추기 ==//

    while (!isMiddle)
    {
        collectSensor();

        // 두 센서중에 하나라도 줄을 감지한다면
        if (a1 >= onLine || a2 >= onLine)
        {

            if (a1 >= a2 - errorRange && a1 - errorRange <= a2) // 두 센서 사이에 줄이 있음 => 성공
            {
                isMiddle = true;
                Serial.println("완성");
                wheel(0, 0, 0);
                break;
            }

            else if (a1 < a2 - errorRange) // 오른쪽 센서 값이 더 클 때
            {
                Serial.println("D2 감지 | A2 감지 됨");
                wheel(0, 0, 15);
            }

            else if (a1 - errorRange > a2) // 왼쪽 센서 값이 더 클 때
            {
                Serial.println("D2 감지 | A1 감지 됨");
                wheel(0, 0, -15);
            }
        }

        else // 줄에서 벗어나 있는 경우
        {
            // 오른쪽 탐색
            bool isFindLine = false;
            int start = millis();
            int end = millis();
            while (!isFindLine)
            {
                while (1)
                {
                    if (a2 > onLine)
                    {
                        isFindLine = true;
                    }
                    end = millis();

                    if (end - start >= checkTime || isFindLine)
                    {
                        break;
                    }

                    wheel(0, 0, 25);
                    collectSensor();
                }

                // 왼쪽 탐색
                start = millis();
                end = millis();
                while (1)
                {
                    if (a1 > onLine)
                    {
                        isFindLine = true;
                    }
                    end = millis();
                    if (end - start >= checkTime * 2 || isFindLine)
                    {
                        break;
                    }
                    wheel(0, 0, -25);
                    collectSensor();
                }
            }
        }
    }
}

// 센서 값 읽는 함수, 아날로그는 a2 값 기준
void collectSensor()
{
    a1 = analogRead(A1) + diff;
    a2 = analogRead(A2);
    D2 = prizm.readLineSensor(2);
    D3 = prizm.readLineSensor(3);
    D4 = prizm.readLineSensor(4);
    //  Serial.print("a1: ");
    //  Serial.print(a1);
    //  Serial.print(" / a2: ");
    //  Serial.print(a2);
    //  Serial.print(" / D2: ");
    //  Serial.print(D2);
    //  Serial.print(" / D3: ");
    //  Serial.print(D3);
    //  Serial.print(" / D4: ");
    //  Serial.println(D4);
}

// 노란색기둥말고 가장 앞쪽에서 가까운 오브젝트 라인 찾는 함수
void shfkstorakfrhwjrwo()
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (columnBlock[i][j] == 1 || columnBlock[i][j] == 2 || columnBlock[i][j] == 3)
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
            break;
    }
    Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

void objectLiftup()
{
    while (1)
    {
        //    lineTracing();
        if (prizm.readSonicSensorCM(A1) > 8.5) // 초음파 센서 거리가 --정도일 때
        {
            wheel(0, 0, 0);
            break;
        }
    }
    objectGrab();
    lift_up(1000); // lift 아주 살짝 올려주기
                   //  back();    // 시간 넣어주기
                   // 오브젝트 리프트업을 위한 전진과 리프트업 하고 뒤에까지 오기
}

void objectLiftdown()
{
    while (1)
    {
        //    lineTracing();
        if (prizm.readSonicSensorCM(A1) > 8.5) // 초음파 센서 거리가 --정도일 때
        {
            wheel(0, 0, 0);
            break;
        }
    }
    lift_down(n); // lift 올린만큼 내려주기
    objectDrop();
    //  back(); // 시간 넣어주기
    // 오브젝트 리프트다운을 위한 전진과 리프트다운 하고 뒤에까지 오기
}

// 바로앞에 있는 오브젝트기둥의 색깔 판별
void vkseks()
{
    objectColumnFlag[objectFlagCount] = columnBlock[currentLine][currentLineFlag];
    objectFlagCount++;
}

void findYellowColumn()
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (columnBlock[i][j] == 4)
            {
                targetLine = i;
                targetLineFlag = j;
                objectBlock[i][j] = objectFlag;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
            break;
    }
    Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

void findTargetColumn(int targetObject)
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (columnBlock[i][j] == targetObject)
            {
                targetLine = i;
                targetLineFlag = j;
                objectBlock[i][j] = targetObject;
                forflag = 1;
                break;
            }
        }
        if (forflag == 1)
            break;
    }
    Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

//
void flagColorLine(int colorNum)
{
    int forflag = 0;
    for (int i = 0; i <= 4; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            if (objectBlock[i][j] == colorNum)
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
            break;
    }
    Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
}

void objectGrab()
{
    // 모터 회전
}

void objectDrop()
{
    // 모터 회전
}

void finish()
{
    Direction_find(currentLine, 0, currentLineFlag, 0);
    // 끝라인에서 피니쉬 들어가기
}

void colorvksquf()
{
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
                }
                if (result.yCenter <= 80)
                {
                    objectBlock[currentLine][currentLineFlag] = result.ID;
                }
            }
            printResult(result);
            delay(1000);
        }
    }
}

void printResult(HUSKYLENSResult result)
{
    if (result.command == COMMAND_RETURN_BLOCK)
    {
        Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID + F(", 오브젝트 배열상태 = ") + objectBlock[0][0] + F(", 기둥 배열상태 = ") + columnBlock[0][0]);
    }
    else if (result.command == COMMAND_RETURN_ARROW)
    {
        Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
    }
    else
    {
        Serial.println("Object unknown!");
    }
}

void checkBlock()
{

    for (int j = 0; j <= 4; j++)
    {
        for (int k = 0; k <= 1; k++)
        {
            Serial.println("오브젝트배열 : ");
            Serial.println(objectBlock[j][k]);
            delay(1000);
        }
    }
}

void qodufcodnrl()
{
    for (int i = 0; i < 4; i++)
    {
        Direction_move(1, 1);
        colorvksquf();
    }
    turn();
    for (int j = 4; j > 0; j--)
    {
        Direction_move(RIGHT, 1);
        colorvksquf();
    }
}
