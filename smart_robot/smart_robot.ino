#include <PRIZM.h>
#include <stdio.h>
PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

void wheel(int x, int y, int z);
void LineTracing();
void centering();
void start();
void turn();

// void qwe();

void setup() {
  prizm.PrizmBegin();
//  prizm.resetEncoder(1);
  Serial.begin(9600);
}

void loop() {
//   start();
 wheel(0, 0, 35);  // 오른쪽으로 돌기
  delay(1000);
  wheel(60, -60, 0);// 왼쪽 앞 대각선으로 이동
  delay(900);
  while(1){
    wheel(60, 0, 0);  // 왼쪽으로 이동
    if(prizm.readLineSensor(3) == HIGH)
      break;
  }
  LineTracing(50);
    wheel(0, 80,0);
    delay(600);
    turn();
    LineTracing(50);
      prizm.PrizmEnd();
}


//void start(){
//  wheel(0, 0, 30);  // 오른쪽으로 돌기
//  delay(1000);
//  wheel(60, -60, 0);// 왼쪽 앞 대각선으로 이동
//  delay(2000);
//  wheel(60, 0, 0);  // 왼쪽으로 이동
//  positing();
//   delay(80);
//   wheel(0,0,0);           //정지 라인 도착
//  //qwe();
//}

void line(int l){ 
    while(prizm.readLineSensor(2)==LOW){ }
}

void turn(){
    wheel(0, 0 , 30);
    delay(1000);
    while(1){
//      int s1 = analogRead(A1);
//      int s2 = analogRead(A2);
//
//      if(s1 > 350 && s1 > 350){
//        wheel(0, 0, 0);
//        break;

//      }
    if(prizm.readLineSensor(4)== HIGH) {
      break;
    }
    }
    
}

void positing(){
    line(3);
    while(1){
      int s1 = analogRead(A1);
      int s2 = analogRead(A2);

      if(s1 > 200 && s1 > 200){
        wheel(0, 0, 0);
        break;
      }
    }
    
}

//void centering(){
//    int s1 = analogRead(A1);
//    int s2 = analogRead(A2);
//    Serial.println(prizm.readLineSensor(2));
//    if(prizm.readLineSensor(2)==LOW){
//        wheel(-100, 0, 0);//우방
//        for(int i=0; i <100; i++){
//            if(prizm.readLineSensor(4)==HIGH){
//                goto check1;
//            }
//        }
//        wheel(100, 0, 0);//좌방
//        for(int i=0; i <100; i++){
//            if(prizm.readLineSensor(3)==HIGH){
//                goto check1;
//            }
//        }
//        check1:
//        wheel(0,0,0);
//   }
//   if(s1 < 100 && s2 < 100){
//        wheel(0, 0, -30);//회전
//        for(int i=0; i <5000; i++){
//            s1 = analogRead(A1);
//            s2 = analogRead(A2);
//            if(s1 > 100 && s2 > 100){
//                goto check2;
//            }
//        }
//        wheel(0, 0, 30);//회전
//        for(int i=0; i <5000; i++){
//            s1 = analogRead(A1);
//            s2 = analogRead(A2);
//            if(s1 > 100 && s2 > 100){
//                goto check2;
//            }
//        }
//        check2:
//        wheel(0,0,0);
//        delay(80);
//   }
//}
void LineTracing(int s){//라인 트레이싱, 아날로그 센서
    while(1){
        int s1 = analogRead(A1);
        int s2 = analogRead(A2);
        if(s1 < 300 && s2 > 300) {
            wheel(-s*0.17, -s, s);  // 오른쪽 앞으로 돌기
        }else if(s1 >300 && s2 < 300) {
            wheel(s*17, -s, -s);  // 왼쪽 앞으로 돌기 
        }else if( prizm.readLineSensor(3)== HIGH ){
          wheel(-s*0.17, -s, s);  // 오른쪽 앞으로 돌기
        }else if(prizm.readLineSensor(4)== HIGH){
          wheel(s*17, -s, -s);  // 왼쪽 앞으로 돌기 
        }
        else{
          wheel(0, -s, 0);
        }
        if(prizm.readLineSensor(3)== HIGH && prizm.readLineSensor(4)== HIGH) break;
    }
    wheel(0,0,0);
    delay(1000);
}

void wheel(int x, int y, int z){
  int A = 0, B = 0, C = 0, D = 0;

  A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
  B = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
  C = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);
  D = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);

  exc1.setMotorPowers(1, A, B);
  exc2.setMotorPowers(2, C, D);
}
