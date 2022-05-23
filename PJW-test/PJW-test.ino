#include <PRIZM.h>
#include <stdio.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
HUSKYLENS huskylens;
//HUSKYLENS green line >> SDA; blue line >> SCL
void printResult(HUSKYLENSResult result);

PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

#define RED 1;
#define GREEN 2;
#define BLUE 3;

#define RIGHT 1;
#define LEFT -1;
#define STOP 0;

#define TRUE 1;
#define FALSE 0;

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

/////////////////////////////////////
//초록 기둥 파란 오브 //////////////////
/////////////////////////////////////
///////////////////빨간 기둥 초록 오브//
/////////////////////////////////////
//     노란 기둥    //////////////////
/////////////////////////////////////
///////////////////파란 기둥 빨간 오브//
/////////////////////////////////////
/////////////////////////////////////
///////finish///////////start////////

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
  while (!huskylens.begin(Wire))
 {
 Serial.println(F("Begin failed!"));
 Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
 Serial.println(F("2.Please recheck the connection."));
 delay(100);
 }
  // collectSensor();
}

void loop()
{
//  shart();              // 구현해야함
//  ();             // 카메라 모듈값 받아야함 // 카메라 관련 새로운 함수 만들기
//  shfkstorakfrhwjrwo(); // 노란색 기둥을 제외한 가장 끝 쪽 기둥 탐색 후 이동 //
//  objectLiftup();       // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
//  findYellowColumn();   // 노란색 기둥을 향해 이동 //
//  objectLiftdown();     // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
//  for (int i = 0; i < 3; i++)
//  {
//    flagColorLine(objectColumnFlag[i])     // 판단함수에서 받은 기둥 색과 맞는 오브젝트를 찾아서 이동 //
//        vkseks();                          // 현재 서 있는 라인과 방향의 기둥색 가져오기 
//    objectLiftup();                        // 초음파센서 이용해서 거리 조절하고 리프트업 하고 백함수
//    findTargetColumn(objectColumnFlag[i]); // 잡고있는 오브젝트와 같은 색의 기둥을 찾아서 이동 //
//    objectLiftdown();                      // 초음파센서 이용해서 거리 조절하고 리프트다운 하고 백함수
//  }
//  finish(); // 구현해야함

//  wheel(0, -60, 0);
//  if(prizm.readSonicSensorCM(A1) <= 8.5) {
//    wheel(0, 0, 0);
//    prizm.PrizmEnd();
//  }
  colorvksquf();


  
}

// 처음 시작할 때 대각선으로 이동
void start()
{
  // 직접 해보기
  // 움직이다가 전진후 D3 & D4 만나면 스탑
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

//void findRightLine()
//{
//  Serial.println("Find Right Line...");
//  collectSensor();
//  wheel(-45, 0, 0);
//  delay(200);
//  while (1)
//  {
//    collectSensor();
//    wheel(-45, 0, 0);
//    if (D2 == HIGH)
//    {
//      Serial.println("Line Found");
//      wheel(45, 0, 0);
//      delay(230);
//      wheel(0, 0, 0);
//      delay(200);
//      currentLine++;
//      break;
//    }
//  }
//}
//
//void findLeftLine()
//{
//  collectSensor();
//  Serial.println("Find Left Line...");
//  wheel(45, 0, 0);
//  delay(200);
//  while (1)
//  {
//    collectSensor();
//    wheel(45, 0, 0);
//    if (D2 == HIGH)
//    {
//      Serial.println("Line Found");
//      wheel(-45, 0, 0);
//      delay(230);
//      wheel(0, 0, 0);
//      delay(200);
//      currentLine--;
//      break;
//    }
//  }
//}
//
//void lift_up(int s)
//{ //리프트 up 함수
//  n += s;
//  prizm.setMotorSpeed(1, -600);
//  delay(s);
//  prizm.setMotorSpeed(1, 0);
//}
//
//void lift_down(int s)
//{ //리프트 down 함수
//  n -= s;
//  prizm.setMotorSpeed(1, 600);
//  delay(s);
//  prizm.setMotorSpeed(1, 0);
//}
//
//// 뒤로 돌아서 중간 맞추는 함수
//void turn()
//{
//  int errorRange = 600;
//  // 처음엔 빠른 속도로 돌다가
//  wheel(0, 0, 40);
//  delay(1200);
//  // 느린속도로 센서 감지
//  wheel(0, 0, 25);
//  while (1)
//  {
//    collectSensor();
//    // 아날로그 센서 사용
//    if (a2 - errorRange >= a1)
//    {
//      wheel(0, 0, 0);
//      break;
//    }
//  }
//  center();
//}
//
//void Direction_find(int now_line, int next, int currentFlag, int targetFlag)
//{                                    //목적지 라인 찾기
//  int destination = now_line - next; //음수면 오른쪽, 양수면 왼쪽 // 음수면 왼쪽, 양수면 오른쪽
//  int cnt = abs(destination);        //라인 이동 횟수
//  int direc = 0;
//  if (targetFlag - currentFlag != 0)
//  {
//    turn(); // 시간 넣어주기
//    currentLineFlag = targetFlag;
//  }
//  if (currentFlag == 0)
//  {
//    if (destination < 0)
//    {
//      direc = RIGHT;
//    }
//    else if (destination > 0)
//    {
//      direc = LEFT;
//    }
//    else
//    {
//      direc = STOP;
//    }
//  }
//  else if (currentFlag == 1)
//  {
//    if (destination < 0)
//    {
//      direc = LEFT;
//    }
//    else if (destination > 0)
//    {
//      direc = RIGHT;
//    }
//    else
//    {
//      direc = STOP;
//    }
//  }
//
//  Serial.println(direc);
//  Serial.println(cnt);
//  Direction_move(direc, cnt); //목적지 라인으로 이동
//}
//
//void Direction_move(int direc, int cnt)
//{ //목적지 라인으로 이동하는 함수
//  for (int i = 0; i < cnt; i++)
//  { //이동 횟수에 따른 반복문
//    if (direc == 1)
//    {
//      findRightLine();
//    }
//    else if (direc == -1)
//    {
//      findLeftLine();
//    }
//    else
//    {
//      wheel(0, 0, 0);
//    }
//    delay(300);
//  }
//  wheel(0, 0, 0);
//}
//
//// 줄 위에 서있는 상태에서 T자 구간에 도착할 때까지 라인트레이싱을 하면서 전진 반복
//void LineTracing()
//{
//  int frontSpeed = 45;
//  int errorRange = 0;
//
//  while (true)
//  {
//    Serial.println("Line Tracing...");
//    collectSensor();
//
//    // 도착하면 정지
//    if (D3 == HIGH && D4 == HIGH)
//    {
//      wheel(0, 0, 0);
//      break;
//    }
//    // 중앙에 선이 있을 때
//    if (D2 == HIGH)
//    {
//      // 디지털 감지
//      if (D3 == HIGH && D4 == LOW)
//      {
//        // 왼쪽 회전
//        wheel(0, -frontSpeed, -5);
//      }
//      else if (D3 == LOW && D4 == HIGH)
//      {
//        // 오른쪽 회전
//        wheel(0, -frontSpeed, 5);
//      }
//      // 아날로그 감지
//      else if (D3 == LOW && D4 == LOW)
//      {
//        if (a1 > a2 + errorRange)
//        {
//          // 왼쪽 회전
//          wheel(0, -frontSpeed, -3);
//        }
//
//        else if (a1 + errorRange < a2)
//        {
//          // 오른쪽 회전
//          wheel(0, -frontSpeed, 3);
//        }
//      }
//    }
//
//    // 중앙에 선이 없을 때
//    else if (D2 == LOW)
//    {
//      // 디지털 감지
//      if (D3 == HIGH && D4 == LOW)
//      {
//        // 왼쪽 횡이동 & 왼쪽 회전
//        wheel(5, -frontSpeed, -5);
//      }
//      else if (D3 == LOW && D4 == HIGH)
//      {
//        // 오른쪽 횡이동 & 오른쪽 회전
//        wheel(-5, -frontSpeed, 5);
//      }
//      // 아날로그 감지
//      else if (D3 == LOW && D4 == LOW)
//      {
//        if (a1 > a2 + errorRange)
//        {
//          // 왼쪽 횡이동 & 왼족 회전
//          wheel(5, -frontSpeed, -3);
//        }
//
//        else if (a1 + errorRange < a2)
//        {
//          // 오른쪽 횡이동 & 오른쪽 회전
//          wheel(-5, -frontSpeed, 3);
//        }
//      }
//    }
//  }
//}
//
//// 뒤로 간다음 중간 맞춤
//void back(int time)
//{
//  Serial.println("back...");
//  wheel(0, 50, 0);
//  delay(time);
//  wheel(0, 0, 0);
//  center();
//  if (currentFlag == 0)
//    currentLineFlag = 1;
//  else
//    currentLineFlag = 0;
//}
//
//// 가운데 맞추는 코드
//void center()
//{
//  int errorRange = 80;
//  int onLine = 250;      // 아날로그 센서가 줄 위에 있을 때의 센서 최소값
//  int checkTime = 500;   // 이 숫자를 올리면 줄을 찾는 반경이 넓어짐
//  bool isCenter = false; // 중앙(D2) 확인
//  bool isMiddle = false; // 중간(a1, a2)확인
//
//  collectSensor();
//
//  //== 1. 중앙 맞추기 ==//
//  while (!isCenter)
//  {
//    if (D2 == HIGH)
//    {
//      isCenter = true;
//    }
//    else if (D2 == LOW)
//    {
//      // 오른쪽 탐색
//      int start = millis();
//      int end = millis();
//      while (1)
//      {
//        end = millis();
//        if (end - start >= checkTime || isCenter)
//        {
//          wheel(0, 0, 0);
//          break;
//        }
//        wheel(30, 0, 0);
//        collectSensor();
//        if (D2 == HIGH)
//        {
//          isCenter = true;
//        }
//      }
//
//      // 왼쪽 탐색
//      start = millis();
//      end = millis();
//      while (1)
//      {
//        end = millis();
//        if (end - start >= checkTime * 2 || isCenter)
//        {
//          wheel(0, 0, 0);
//          break;
//        }
//        wheel(-30, 0, 0);
//        collectSensor();
//        if (D2 == HIGH)
//        {
//          isCenter = true;
//        }
//      }
//    }
//  }
//
//  //== 2. 중간 맞추기 ==//
//
//  while (!isMiddle)
//  {
//    collectSensor();
//
//    // 두 센서중에 하나라도 줄을 감지한다면
//    if (a1 >= onLine || a2 >= onLine)
//    {
//
//      if (a1 >= a2 - errorRange && a1 - errorRange <= a2) // 두 센서 사이에 줄이 있음 => 성공
//      {
//        isMiddle = true;
//        Serial.println("완성");
//        wheel(0, 0, 0);
//        break;
//      }
//
//      else if (a1 < a2 - errorRange) // 오른쪽 센서 값이 더 클 때
//      {
//        Serial.println("D2 감지 | A2 감지 됨");
//        wheel(0, 0, 15);
//      }
//
//      else if (a1 - errorRange > a2) // 왼쪽 센서 값이 더 클 때
//      {
//        Serial.println("D2 감지 | A1 감지 됨");
//        wheel(0, 0, -15);
//      }
//    }
//
//    else // 줄에서 벗어나 있는 경우
//    {
//      // 오른쪽 탐색
//      bool isFindLine = false;
//      int start = millis();
//      int end = millis();
//      while (!isFindLine)
//      {
//        while (1)
//        {
//          if (a2 > onLine)
//          {
//            isFindLine = true;
//          }
//          end = millis();
//
//          if (end - start >= checkTime || isFindLine)
//          {
//            break;
//          }
//
//          wheel(0, 0, 25);
//          collectSensor();
//        }
//
//        // 왼쪽 탐색
//        start = millis();
//        end = millis();
//        while (1)
//        {
//          if (a1 > onLine)
//          {
//            isFindLine = true;
//          }
//          end = millis();
//          if (end - start >= checkTime * 2 || isFindLine)
//          {
//            break;
//          }
//          wheel(0, 0, -25);
//          collectSensor();
//        }
//      }
//    }
//  }
//}
//
//// 센서 값 읽는 함수, 아날로그는 a2 값 기준
//void collectSensor()
//{
//  a1 = analogRead(A1) + diff;
//  a2 = analogRead(A2);
//  D2 = prizm.readLineSensor(2);
//  D3 = prizm.readLineSensor(3);
//  D4 = prizm.readLineSensor(4);
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
//}
//
//// 노란색기둥말고 가장 뒤쪽에서 가까운 오브젝트 라인 찾는 함수
//void shfkstorakfrhwjrwo()
//{
//  int forflag = 0;
//  for (int i = 4; i >= 0 i--)
//  {
//    for (int j = 1; j >= 0 j--)
//    {
//      if (columnBlock[i][j] == 1 || columnBlock[i][j] == 2 || columnBlock[i][j] == 3)
//      {
//        targetLine = i;
//        targetLineFlag = j;
//        objectFlag = objectBlock[i][j];
//        objectBlock[i][j] = 0;
//        forflag = 1;
//        break;
//      }
//    }
//    if (forflag == 1)
//      break;
//  }
//  Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
//}
//
//void objectLiftup()
//{
//  while (1)
//  {
//    linetracing();
//    if (prizm.readSonicSensorCM(A1) > 8.5) // 초음파 센서 거리가 --정도일 때
//    {
//      wheel(0, 0, 0);
//      break;
//    }
//  }
//  objectGrap();
//  lift_up(1000); // lift 아주 살짝 올려주기
//  back();    // 시간 넣어주기
//  // 오브젝트 리프트업을 위한 전진과 리프트업 하고 뒤에까지 오기
//}
//
//void objectLiftdown()
//{
//  while (1)
//  {
//    linetracing();
//    if (prizm.readSonicSensorCM(A1) > 8.5) // 초음파 센서 거리가 --정도일 때
//    {
//      wheel(0, 0, 0);
//      break;
//    }
//  }
//  lift_down(n); // lift 올린만큼 내려주기
//  objectDrop();
//  back(); // 시간 넣어주기
//  // 오브젝트 리프트다운을 위한 전진과 리프트다운 하고 뒤에까지 오기
//}
//
//// 바로앞에 있는 오브젝트기둥의 색깔 판별
//void vkseks()
//{
//  objectColumnFlag[objectFlagCount] = columnBlock[currentLine][currentLineFlag];
//  objectFlagCount++;
//}
//
//void findYellowColumn()
//{
//  int forflag = 0;
//  for (int i = 4; i >= 0; i--)
//  {
//    for (int j = 1; j >= 0; j--)
//    {
//      if (columnBlock[i][j] == 4)
//      {
//        targetLine = i;
//        targetLineFlag = j;
//        objectBlock[i][j] = objectFlag;
//        forflag = 1;
//        break;
//      }
//    }
//    if (forflag == 1)
//      break;
//  }
//  Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
//}
//
//void findTargetColumn(int targetColumn)
//{
//  int forflag = 0;
//  for (int i = 4; i >= 0; i--)
//  {
//    for (int j = 1; j >= 0; j--)
//    {
//      if (columnBlock[i][j] == targetColumn)
//      {
//        targetLine = i;
//        targetLineFlag = j;
//        objectBlock[i][j] = objectFlag;
//        forflag = 1;
//        break;
//      }
//    }
//    if (forflag == 1)
//      break;
//  }
//  Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
//}
//
////
//void flagColorLine(int colorNum)
//{
//  int forflag = 0;
//  for (int i = 4; i >= 0; i--)
//  {
//    for (int j = 1; j >= 0; j--)
//    {
//      if (objectBlock[i][j] == colorNum)
//      {
//        targetLine = i;
//        targetLineFlag = j;
//        objectFlag = objectBlock[i][j];
//        objectBlock[i][j] = 0;
//        forflag = 1;
//        break;
//      }
//    }
//    if (forflag == 1)
//      break;
//  }
//  Direction_find(currentLine, targetLine, currentLineFlag, targetLineFlag);
//}
//
//void objectGrab()
//{
//  // 모터 회전
//}
//
//void objectDrop()
//{
//  // 모터 회전
//}
//
//void finish()
//{
//  Direction_find(currentLine, 0, currentLineFlag, 0);
//  // 끝라인에서 피니쉬 들어가기
//}

void colorvksquf() 
{
 if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
 else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
 else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
 else
 {
  Serial.println(F("###########"));
  while (huskylens.available())
  {
    HUSKYLENSResult result = huskylens.read();
    if (result.command == COMMAND_RETURN_BLOCK)
    {
      if(result.yCenter >= 120) 
      {
        columnBlock[currentLine][currentLineFlag] = result.ID;
      }
      if(result.yCenter <= 110)
      {
        objectBlock[currentLine][currentLineFlag] = result.ID;
      }
    }
    printResult(result);
    delay(1000);
  }
 }
}

 void printResult(HUSKYLENSResult result){
 if (result.command == COMMAND_RETURN_BLOCK){
 Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID + F(", 배열상태 = ") + objectBlock[0][0]);
 }
 else if (result.command == COMMAND_RETURN_ARROW){
 Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
 }
 else{
 Serial.println("Object unknown!");
 }
}