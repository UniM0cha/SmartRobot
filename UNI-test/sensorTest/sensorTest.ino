/**
 *  허스키렌즈 학습 순서 : 빨간색(1), 초록색(2), 파란색(3), 노란색(4)
 *  스캔올 실패했을 경우 다시 스캔올
 *  조기종료하고 기둥탐색 알고리즘 수정
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

void setup()
{
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
    prizm.PrizmBegin();
    Serial.begin(115200);
}

void loop()
{
    bool D1 = prizm.readLineSensor(1);
    bool D2 = prizm.readLineSensor(2);
    bool D3 = prizm.readLineSensor(3);
    bool D4 = prizm.readLineSensor(4);
    int a1 = analogRead(A1);
    int a2 = analogRead(A2);
    int a3 = prizm.readSonicSensorCM(A3);
    Serial.print("D1 = ");
    Serial.println(D1);
    Serial.print("D2 = ");
    Serial.println(D2);
    Serial.print("D3 = ");
    Serial.println(D3);
    Serial.print("D4 = ");
    Serial.println(D4);
    Serial.print("a1 = ");
    Serial.println(a1);
    Serial.print("a2 = ");
    Serial.println(a2);
    Serial.print("a3 = ");
    Serial.println(a3);
    Serial.println("============================");
}