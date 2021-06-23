 
#include <Arduino_FreeRTOS.h>
#include <TM1637Display.h> // Tm1637 module
 
// TM1637 Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);
int speakerpin = 12; //스피커가 연결된 디지털핀 설정
int sw=4;
 
int states=0;
int counts=0;
char turnon='0';
char check='0';
 
 
unsigned long cnt=0;//버튼눌리는 시간간의 거리
 
 
const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };
 
 
// time calc
#define numberOfSeconds(_time_) ((_time_ / 1000) % 60)
#define numberOfMinutes(_time_) ((_time_ / 1000) / 60)
// hold time selected in ms.
//unsigned long timeLimit = 3000000;// 1000*60*50 = 50분
//unsigned long timeLimit = 1200000;// 1000*60*20 = 20분
unsigned long timeLimit = 600000;// 1000*60*10 = 10분
//unsigned long timeLimit = 1800000;// 1000*60*30 = 30분
//unsigned long timeLimit = 1800; // 3초
int timer=0;
 
 
 
void setup() {
 Serial.begin(9600);
 
  //display setup
  display.setBrightness(0);    // LED 밝기조정   0~6
  //display.setBrightness(0x0c);
  display.showNumberDecEx(0, 0x40, true);    //64
  pinMode(sw, INPUT);
  /* Create one of the two tasks. */
  xTaskCreate(  vTask1,   /* Pointer to the function that implements the task. */
    "Task 1", /* Text name for the task.  This is to facilitate debugging only. */
    200,    /* Stack depth - most small microcontrollers will use much less stack than this. */
    NULL,   /* We are not using the task parameter. */
    1,      /* This task will run at priority 1. */
    NULL );   /* We are not using the task handle. */
 
  /* Create the other task in exactly the same way. */
  xTaskCreate( vTask2, "Task 2", 200, NULL, 1, NULL );
 
 
  vTaskStartScheduler();
 
  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/
 
void vTask1( void *pvParameters )
{
  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    for (long i = 0; i < timeLimit; i=i+1000)   // data type을 long 으로
    {
    // To display the countdown in mm:ss format, separate the parts
      if(check=='2'){
        i=0;
        int seconds = numberOfSeconds(i);
        int minutes = numberOfMinutes(i);
        display.showNumberDecEx(seconds, 0, true,2,2) ;
        display.showNumberDecEx(minutes, (0x80 >> 1), false, 2, 0) ;  // turn on colon    원본은  0x80 >> 3 인데 콜론이 on 되지 않아 0x80 >> 1 로 변경
      }
    while(turnon=='0'){//일시정지
     delay(100);
    }
    
    int seconds = numberOfSeconds(i);//시간표시
    int minutes = numberOfMinutes(i); 
    display.showNumberDecEx(seconds, 0, true,2,2) ;
    // Display the minutes in the first two places, with colon
    display.showNumberDecEx(minutes, (0x80 >> 1), false, 2, 0) ;  // turn on colon    원본은  0x80 >> 3 인데 콜론이 on 되지 않아 0x80 >> 1 로 변경
    //showNumberDecEx(표시할번호,도트,선행제로,설정할 자릿수,위치 최하위 숫자(0-맨왼쪽 ,3맨오른쪽)); 
    //https://sminghub.github.io/Sming/api/classTM1637Display.html#a37c8b71de1418f1b545782b722d7277a
    /*도트
    0.000 (0b10000000)
    00.00 (0b01000000)
    000.0 (0b00100000)
    0.0.0.0 (0b11100000) 콜론 만있는 디스플레이의 경우 :
    00:00 (0b01000000) 점과 콜론 콜론이있는 디스플레이의 경우 :
    0.0 : 0.0 (0b11100000)
     * 
     */
     delay(950);//동작시간이 50정도 소요되서 1000-50=950
      
  }
      
   display.setSegments(SEG_DONE);//시간다됬을시 done표시
   tone(speakerpin, 500,3000);//3초동안 부저울림(핀,소리음,소리지속시간)
   delay(2000);
   turnon='0';
 
  }
}
/*-----------------------------------------------------------*/
 
void vTask2( void *pvParameters )
{
 
  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    if (digitalRead(sw) && counts==0 && cnt==0)//디지털핀에서 신호를 받았을때 
  {
    cnt=millis();//현재시간
    counts++;//누른횟수
  }
  else if (millis() > cnt+500 && cnt>0)//0.5초안에 버튼을 누르지 않고 1번이상 눌럿을때
  {
    states=counts;//누른횟수 저장
    counts=0;//카운트 초기화
    cnt=0;
  }
  else if (digitalRead(sw) && millis() > cnt+200)//0.2초안에 한번더 눌리면 이건 조절이 필요할수도 있다.
  {
    cnt=millis();//시간초기화
    counts++;//카운트증가
 
    if (counts >= 3)
    {
      counts=3;//3번이상이면 카운트3
    }
  }
 
   switch (states)
  {
    case 0:
    break;
    case 1:
     if (turnon=='0'){
      turnon='1';
     }else{
      turnon='0';
     }
     check='1';
     states=0;
    break;
    case 2:
     check='2';
     
     turnon='0';//초기화
     states=0;
    break;
  }
 
  }
}
  /*------------------------------------------------------------*/
 
void loop() {}
