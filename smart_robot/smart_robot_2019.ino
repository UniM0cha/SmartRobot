#include <PRIZM.h>
#include <Adafruit_TCS34725.h>
#include <stdio.h>

#define RED 1;
#define GREEN 2;
#define BLUE 3;
#define TRUE 1;
#define FALSE 0;

PRIZM prizm;
EXPANSION exc1;
EXPANSION exc2;

//0.154초마다 색감지 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

int objSt[3][2]={{1,1},{1,1},{1,1}};
//옮겨야할 오브젝트 배열 1,2,3라인의 1층과 2층의 유무를 정하고
//0 : 없음(FALSE), 1 : 있음(TRUE) 으로 정의

int objEd[3][2]={{0,0},{0,0},{0,0}};
//옮겨진 오브젝트 배열의 색상 1,2,3라인의 1층과 2층의 유무를 정하고
//0 : 없음(FALSE), 1 : 있음(TRUE) 으로 정의

int lineCurrent = 1; //현재라인을 표시하는 변수
                     //1 : 레드, 2 : 그린, 3 : 블루

int sameFlag = 0;    //0 : 모름, 1 : 같음, 2 : 다름
int fir_Color = 0;   //0 : 모름, 1 : 레드, 2 : 그린, 3 : 블루
int sec_Color = 0;   //0 : 모름, 1 : 레드, 2 : 그린, 3 : 블루
int n = 0;

//함수 선언 구역=======================================================================
void test();//테스트를 위한 임시칸, 테스트 함수만 실행후 종료, 평소에는 주석 상태

void Direction_find(int now_line, int compare, int next);
void Direction_move(String direc, int cnt);
void objLoad();     
void objFind();
void same_objPutdown();    
void diff_objPutdown(int now_line, int compare, int two, int one);

void turn_r();
void turn_l();
void fork(int s);
int cSameCheck(int f2,int f1);

void f_move(int s);
void b_move(int s);
void o_r_move(int s);
void o_l_move(int s);
void t_r_move(int s);
void t_l_move(int s);

//불변 함수
void start(); 
void wheel(int x, int y, int z);
void line(int l);
void positing();
void centering();
void LineTracing(int s);  //수직라인까지 전진 s는 올리는 시간 300이 적정속도
int colorcheck();
void lift_up(int s);
void lift_down(int s);
//함수 선언 구역=======================================================================

void setup() {
  Serial.begin(9600);
  prizm.PrizmBegin();
  if(tcs.begin()) { //센서 연결 o
    Serial.println("Found sensor");
  }else {           //센서 연결 x
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
}
void loop() {
  test(); //테스트를 위한 함수 실전시에는 주석으로 표시

  //prizm.PrizmEnd(); // END
}
void test(){//테스트 함수
  while(1){
     
    objFind();
  
  }
  //여기까지 메모 lift_up 높이 속도 500기준 바닥0초 들기 3000초 2층 700초

  prizm.PrizmEnd();
}

void Direction_find(int now_line, int compare, int next){//목적지 라인 찾기
    //Serial.print("로봇의 현재 라인 : ");    Serial.println(now_line);
    //Serial.print("2층 : ");    Serial.print(two);  //2층 오브젝트 컬러
    //Serial.print(", 1층 : ");  Serial.println(one);//1층 오브젝트 컬러
    int destination = now_line - next;//음수면 왼쪽, 양수면 오른쪽
    int cnt = abs(destination);//라인 이동 횟수
    String direc = "";

    if(destination < 0){
        direc = "LEFT";
        Serial.println(direc);    
    }else if(destination > 0){
        direc = "RIGHT";
        Serial.println(direc);
    }else{
        direc = "STOP";
        Serial.println(direc);
    }
    Direction_move(direc, cnt);//목적지 라인으로 이동
}

void Direction_move(String direc, int cnt){//목적지 라인으로 이동하는 함수
    int s1 = analogRead(A1);
    int s2 = analogRead(A2);
    for(int i=0; i<cnt; i++){//이동 횟수에 따른 반복문
        if(direc.equals("LEFT")){
            wheel(-200,0,0);
            delay(200);
            wheel(-200,0,0);
            line(5);
            wheel(0,0,0);
            lineCurrent += 1;
        }else if(direc.equals("RIGHT")){
            wheel(200,0,0);
            delay(200);
            wheel(200,0,0);
            line(5);
            wheel(0,0,0);
            lineCurrent -= 1;
        }else{
            wheel(0,0,0);
        }
        delay(200);
    }
    while(prizm.readLineSensor(5)==LOW){
        exc2.setMotorSpeeds(2, -50, -50);
        if(prizm.readLineSensor(5)==HIGH){break;}
    }
    wheel(0,0,0);
    //Serial.print("이동 후 현재 라인 : ");
    //Serial.println(lineCurrent);
}
void objFind(){      //오브젝트 찾기
  //현재 라인부터 오브젝트 유무 판별
  //현재 라인에 오브젝트가 있을 경우 => Load함수
  //현재 라인에 오브젝트가 없을 경우 =>     
    for(int i=0; i < 3; i++){
        if(sec_Color == 1){
            wheel(200,0,0);
            delay(200);
            wheel(200,0,0);
            line(5);
            wheel(0,0,0);
            lineCurrent += 1;
            if(objSt[lineCurrent-1][1] == 1){
                objLoad();
            }else{
                wheel(200,0,0);
                delay(200);
                wheel(200,0,0);
                line(5);
                wheel(0,0,0);
                lineCurrent += 1;
                objLoad();
            }
        }else if(sec_Color == 2){
            if(objSt[lineCurrent-1][1] == 1){
                objLoad();
            }else{
                wheel(200,0,0);
                delay(200);
                wheel(200,0,0);
                line(5);
                wheel(0,0,0);
                lineCurrent += 1;
                objLoad();
            }
        }else if(sec_Color == 3){
            if(objSt[lineCurrent-1][1] == 1){
                objLoad();
            }else{
                wheel(-200,0,0);
                delay(200);
                wheel(-200,0,0);
                line(5);
                wheel(0,0,0);
                lineCurrent -= 1;
                objLoad();
            }
        }else{
            objLoad();
        }
    }
}
void objLoad(){      //오브젝트 적재
  if(objSt[lineCurrent-1][0] == 1){   //현재라인의 2층에 오브젝트가 있는가
     if(objSt[lineCurrent-1][1] == 1){ //현재라인의 1층에 오브젝트가 있는가
        lift_up(1050);//2층 오브젝트를 위한 리프트 높이

        LineTracing(100);//가로라인을 만나면 정지
    
        wheel(0,50,0);//오브젝트 적재를 위한 전진
        delay(100);
        wheel(0,0,0);
        delay(300);

        sec_Color = colorcheck();//2층 오브젝트 컬러

        wheel(0,-100,0);//오브젝트에서 포크 길이만큼 빼기
        delay(2700);
        wheel(0,0,0);

        lift_down(1050);//1층 오브젝트를 위한 리프트 높이

        LineTracing(100);//가로라인을 만나면 정지
    
        wheel(0,50,0);//오브젝트 적재를 위한 전진
        delay(100);
        wheel(0,0,0);
        delay(300);

        fir_Color = colorcheck();//1층 오브젝트 컬러

        sameFlag= cSameCheck(sec_Color, fir_Color);//2층컬러와 1층컬러 비교

        lift_up(1000);//오브젝트 띄우기

        wheel(0,-100,0);//후진
        delay(3000);
        wheel(0,0,0);

        turn_r();//오른쪽 회전

        for(int i = 0; i < 2; i++){ //적재한 오브젝트 배열 0(없음)으로 설정
            objSt[lineCurrent-1][i] = 0;
        }

        Direction_find(lineCurrent, sameFlag, fir_Color);

        lift_up(4000);

        LineTracing(100);//가로라인을 만나면 정지
        
        if(sameFlag == 0){
            Serial.println("color 비교 모름");
        }else if(sameFlag == 1){
            Serial.println("color 비교 같음");
            same_objPutdown();
        }else if(sameFlag == 2){
            Serial.println("color 비교 다름");
            diff_objPutdown(lineCurrent, sameFlag, sec_Color, fir_Color);//오브젝트 내려놓기
        }else{
            Serial.println("color 비교 오류");
        }
        
     }
  }else if(objSt[lineCurrent-1][1] == 1 && objSt[lineCurrent-1][0] == 0){//현재라인의 1층에 오브젝트가 있는가
      //2개 동시에 옮기는 동작일 경우 이런 상황 없음
      //만약 하나씩 옮기는 동작이면 1층 옮기는 동작 실시
  }else{
      Serial.println("Load 오류");
      //배열 안에 이상한 값 추후 오류로 인한 행동제어 추가 필수!!!!!!!!!!
  }
}
void same_objPutdown(){//같은 색상 오브젝트 내려놓기
    lift_down(1000);//목표지점 적재를 위한 리프트 높이

    wheel(0,-100,0);//포크 빼기
    delay(3000);
    wheel(0,0,0);

    turn_l();

    lift_down(n);
}
void diff_objPutdown(int now_line, int compare, int two, int one){//색상이 다른 오브젝트 내려놓기
    //lift_up(4000);//목표지점을 위한 리프트 높이
    if(objEd[one-1][1] == 0){//목적지 1층의 값이 0이면
        Serial.println("one : 1층에 내려놓기");
        lift_down(1000);//목적지 1층에 내려놓기

        wheel(0,-100,0);//오브젝트에서 포크 길이만큼 빼기
        delay(2700);
        wheel(0,0,0);

        objEd[one-1][1] = 1;
        Serial.print("목적지 : " );
        Serial.print(objEd[one-1][1]);
        Serial.println(objEd[two-1][1]);

        lift_up(1000);//2층 오브젝트 가져가기

        wheel(0,100,0);//오브젝트에서 포크 길이만큼 넣기
        delay(2700);
        wheel(0,0,0);
        delay(300);

        lift_up(500);
        
    }else{//목적지 1층의 값이 1이면
        lift_up(1200);
        Serial.println("one : 2층에 내려놓기");
        lift_down(500);//목적지 2층에 내려놓기

        wheel(0,-100,0);//오브젝트에서 포크 길이만큼 빼기
        delay(2700);
        wheel(0,0,0);
        
        objEd[one-1][1] = 1;
        Serial.print(objEd[one-1][1]);
        Serial.println(objEd[two-1][1]);

        lift_up(1000);//3층 오브젝트 가져가기

        wheel(0,100,0);//오브젝트에서 포크 길이만큼 넣기
        delay(2700);
        wheel(0,0,0);
        delay(300);

        lift_up(500);
    }
    Direction_find(now_line, compare, two);
    if(objEd[two-1][1] == 0){//목적지 1층의 값이 0이면
        Serial.println("two : 1층에 내려놓기");
        lift_down(1500);//목적지 1층에 내려놓기

        wheel(0,-100,0);
        delay(3000);
        wheel(0,0,0);

        objEd[two-1][1] = 1;
        
    }else{
        Serial.println("two : 2층에 내려놓기");
        lift_down(500);//목적지 2층에 내려놓기

        wheel(0,-100,0);
        delay(3000);
        wheel(0,0,0);
        
        objEd[two-1][1] = 1;
    }
    
    wheel(0,-100,0);//후진
    delay(2000);
    wheel(0,0,0);

    turn_l();

    lift_down(n);    
    
}
void turn_r(){        //오른쪽 회전 함수
    wheel(0 ,0, 250);
    delay(1500);
    wheel(0 ,0, 200);
    delay(1500);
    wheel(0,0,100);
    delay(300);
    wheel(0,0,30);
    while(1){
        int s1 = analogRead(A1);
        int s2 = analogRead(A2);
        
        if(s1 > 70 && s2 > 100){
            wheel(0, 0, 0);
            break;
        }
    }
    while(prizm.readLineSensor(5)==LOW){
        exc2.setMotorSpeeds(2, -50, -50);
        if(prizm.readLineSensor(5)==HIGH){break;}
    }
    wheel(0,0,0);
   
}
void turn_l(){        //왼쪽 회전 함수
    wheel(0 ,0, -250);
    delay(1500);
    wheel(0 ,0, -200);
    delay(1500);
    wheel(0,0,-100);
    delay(300);
    wheel(0,0,-30);
    while(1){
        int s1 = analogRead(A1);
        int s2 = analogRead(A2);
        
        if(s1 > 100 && s2 > 70){
            wheel(0, 0, 0);
            break;
        }
    }
    while(prizm.readLineSensor(5)==LOW){
        exc2.setMotorSpeeds(2, -50, -50);
        if(prizm.readLineSensor(5)==HIGH){break;}
    }
    wheel(0,0,0);
}

//void fork(int s){       //fork 함수
//  prizm.setMotorTargets(1, 0, 0, 180, 1440);
//}

int cSameCheck(int f2,int f1){
    int sflag = 0;
    if(f2 == f1 && f2 != 0 && f1 != 0){
        sflag = 1;
    }else if(f2 !=f1 && f2 != 0 && f1 != 0){
        sflag = 2;
    }else{
        Serial.println("SameCheck 오류");
    }
    return sflag;
}

void f_move(int s){//전진
    wheel(0,300,0);
    delay(s);
    wheel(0,0,0);
}
void b_move(int s){//후진
    wheel(0,-300,0);     
    delay(s);
    wheel(0,0,0);
}
void o_r_move(int s){//오브젝트를 바라본 오른쪽
    wheel(200,0,0);     
    delay(s);
    wheel(0,0,0);
    lineCurrent += 1;
}
void o_l_move(int s){//오브젝트를 바라본 왼쪽
    wheel(-200,0,0); 
    delay(2000);    
    line(5);
    wheel(0,0,0);
    lineCurrent -= 1;
}
void t_r_move(int s){//테이블을 바라본 오른쪽
    wheel(200,0,0);     
    delay(s);
    wheel(0,0,0);
    lineCurrent -= 1;
}
void t_l_move(int s){//테이블을 바라본 왼쪽
    wheel(200,0,0);     
    delay(s);
    wheel(0,0,0);
    lineCurrent += 1;
}

//=================================================================
//불변 함수
void start(){             //시작 무브먼트 라인으로 이동
    wheel(600,400,-100);    //전우측 방향(대각선) & 회전
    delay(1800);
    wheel(0, 0, -150);      //회전
    delay(1100);
    wheel(300,0,0);         //우측
    positing();             //라인 찾기 및 위치 세부 조정
    delay(80);
    wheel(0,0,0);           //정지 라인 도착
}
void wheel(int x, int y, int z) {//옴니휠 이동공식
    int A = 0, B = 0, C = 0, D = 0;

    A = (x * 0.5) + (y * 0.5) + (z * 0.841471);
    B = (x * 0.5) + (y * 0.5 * -1) + (z * 0.841471);
    C = (x * 0.5 * -1) + (y * 0.5) + (z * 0.841471);
    D = (x * 0.5 * -1) + (y * 0.5 * -1) + (z * 0.841471);

    exc1.setMotorSpeeds(1, A, B);
    exc2.setMotorSpeeds(2, C, D);
}
void line(int l){     //라인 검출 함수
    while(prizm.readLineSensor(l)==LOW){ }
}
void positing(){//로봇 중앙 라인센서가 감지되면 정지하는 함수
    line(5);
    wheel(0, 0, 0);
}
void centering(){//로봇이 라인 중앙에 맞춰 이동할 수 있게 하는 함수
    int s1 = analogRead(A1);
    int s2 = analogRead(ghgA2);
    if(prizm.readLineSensor(5)==LOW){
        wheel(-100, 0, 0);//우방
        for(int i=0; i <100; i++){
            if(prizm.readLineSensor(5)==HIGH){
                goto check1;
            }
        }
        wheel(100, 0, 0);//좌방
        for(int i=0; i <100; i++){
            if(prizm.readLineSensor(5)==HIGH){
                goto check1;
            }
        }
        check1:
        wheel(0,0,0);
   }
   if(s1 < 100 && s2 < 100){
        wheel(0, 0, -30);//회전
        for(int i=0; i <5000; i++){
            s1 = analogRead(A1);
            s2 = analogRead(A2);
            if(s1 > 100 && s2 > 100){
                goto check2;
            }
        }
        wheel(0, 0, 30);//회전
        for(int i=0; i <5000; i++){
            s1 = analogRead(A1);
            s2 = analogRead(A2);
            if(s1 > 100 && s2 > 100){
                goto check2;
            }
        }
        check2:
        wheel(0,0,0);
        delay(80);
   }
}
void LineTracing(int s){//라인 트레이싱, 아날로그 센서
    while(1){
        int s1 = analogRead(A1);
        int s2 = analogRead(A2);
        if(s1 < s2) {
            //wheel(-(s*0.17), s, 0);
            wheel(-(s*0.15), (s*0.8), -5);
        }else if(s1 > s2) {
            //wheel((s*0.17), s, 0);
            wheel((s*0.15), (s*0.8), 5);
        }else {
            wheel(0, s, 0);
        }
        if(prizm.readLineSensor(3)== HIGH && prizm.readLineSensor(4)== HIGH) break;
    }
    wheel(0,0,0);
}
int colorcheck(){//색상 판별 함수
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    int r_cr;
//  Serial.print("r : ");   Serial.print(r);
//  Serial.print(", g : "); Serial.print(g);
//  Serial.print(", b : "); Serial.print(b);
//  Serial.println();
    if (c > 3000) {
        Serial.println("color 인식 error");
        tcs.setInterrupt(true);
        r_cr = 0;
    }
    else {
        tcs.setInterrupt(false);
        if(r>=320 && r<=470 && g>=240 && g<=350 && b>=220 && b<=330){
            Serial.println("RED");
            r_cr = 1;
        }else if(r>=180 && r<=300 && g>=315 && g<=500 && b>=230 && b<=360){
            Serial.println("GREEN");
            r_cr = 2;
        }else if(r>=160 && r<=250&& g>=250 && g<=400 && b>=290 && b<=450){
            Serial.println("BLUE");
            r_cr = 3;
        }
    }
    return r_cr;
}

void lift_up(int s){    //리프트 up 함수
    n += s;
    prizm.setMotorSpeed(1, -300);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}

void lift_down(int s){ //리프트 down 함수
    n -= s;
    prizm.setMotorSpeed(1, 300);
    delay(s);
    prizm.setMotorSpeed(1, 0);
}