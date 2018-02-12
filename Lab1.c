
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "tm4c123gh6pm.h"
void DelayWait10ms(uint32_t n);
void Clock();
int middleX = 64;
int middleY  =84;
static int testX[60] = {
64,69,74,79,84,
89,93,97,101,104,
107,110,112,113,114,
114,114,113,112,110,
107,104,101,97,93,
89,84,79,74,69,
64,59,54,49,44,
39,35,31,27,24,
21,18,16,15,14,
14,14,15,16,18,
21,24,27,31,35,
39,44,49,54,59,};
static int testY[60] = {
34,34,35,36,38,
41,44,47,51,55,
59,64,69,74,79,
84,89,94,99,104,
109,113,117,121,124,
127,130,132,133,134,
134,134,133,132,130,
127,124,121,117,113,
109,104,99,94,89,
84,79,74,69,64,
59,55,51,47,44,
41,38,36,35,34,};
static int xHour[60] = {
64,
67,
69,
72,
74,
77,
79,
81,
83,
84,
86,
87,
88,
88,
89,
89,
89,
88,
88,
87,
86,
84,
83,
81,
79,
77,
74,
72,
69,
67,
64,
61,
59,
56,
54,
52,
49,
47,
45,
44,
42,
41,
40,
40,
39,
39,
39,
40,
40,
41,
42,
44,
45,
47,
49,
52,
54,
56,
59,
61
};
	
static int yHour[60] = {59,
59,
60,
60,
61,
62,
64,
65,
67,
69,
72,
74,
76,
79,
81,
84,
87,
89,
92,
94,
97,
99,
101,
103,
104,
106,
107,
108,
108,
109,
109,
109,
108,
108,
107,
106,
104,
103,
101,
99,
97,
94,
92,
89,
87,
84,
81,
79,
76,
74,
72,
69,
67,
65,
64,
62,
61,
60,
60,
59
};


void SystemInit(){
}

int main(void){uint32_t i;
  PLL_Init(Bus80MHz); 
  ST7735_InitR(INITR_REDTAB);
	
	ST7735_FillScreen(ST7735_BLACK);
					ST7735_OutString("Current Time : \n");
					ST7735_OutString("Alarm Time : ");
	int testA, testB;
	testA = 10;
	testB = 10;
	ST7735_SetCursor(15,3);
	ST7735_OutString("1");
	ST7735_SetCursor(18,5);
	ST7735_OutString("2");
	ST7735_SetCursor(19,8);
	ST7735_OutString("3");
	ST7735_SetCursor(14,10);
	ST7735_OutString("4");
	ST7735_SetCursor(12,12);
	ST7735_OutString("5");
	ST7735_SetCursor(10,14);
	ST7735_OutString("6");
	ST7735_SetCursor(8,12);
	ST7735_OutString("7");
	ST7735_SetCursor(4,10);
	ST7735_OutString("8");
	ST7735_SetCursor(0,8);
	ST7735_OutString("9");
	ST7735_SetCursor(4,6);
	ST7735_OutString("10");
	ST7735_SetCursor(8,4);
	ST7735_OutString("11");
	ST7735_SetCursor(10,2);
	ST7735_OutString("12");
	while(1){
		int H = -1;
uint32_t AlarmTime;
uint32_t RealTime;
		char nRealTimeStr[] = "AB:CD";
		char nAlarmTimeStr[] = "AB:CD";
			
			
			for(int M = 0; M < 720; M ++){
				int minute = M % 60;
				if(minute%12 == 0){
					H += 1;
				}
				int hour = M/60;
				int h10;
				int h1;
				int m10;
				int m1;
				
				h10 = hour/10+ '0';
				h1 = hour%10+ '0';
				m10 = minute/10+ '0';
				m1 = minute%10+ '0';
				
				nRealTimeStr[0] = h10;
				nRealTimeStr[1] = h1;
				nRealTimeStr[3] = m10;
				nRealTimeStr[4] = m1;

				
				ST7735_SetCursor(15,0);
				ST7735_OutString(nRealTimeStr);
				//printf("%d:%d", hour, minute);
				//ST7735_OutString("\n");
				
				//ST7735_OutString("Current Time : ");
				ST7735_SetCursor(15,1);
				ST7735_OutString(nAlarmTimeStr);
				
				//ST7735_OutString("Alarm Time : ");
				//printf("%d:%d", H+5,M+5);
				ST7735_SetCursor(0,0);
				Clock(middleX, middleY,testA,testB);
				//Clock(middleX, middleY, H, minute);
				DelayWait10ms(100);
			}
		}
}

	
	


void Clock(int midX, int midY, int hour, int minute){
	makeCircle(midX, midY);
	if(minute == 0){
		drawline(midX, midY, testX[59], testY[59], ST7735_BLACK);
	}
	else{
		drawline(midX, midY, testX[minute-1], testY[minute-1], ST7735_BLACK);
	}
		if(hour == 0){
		drawline(midX, midY, xHour[59], yHour[59], ST7735_BLACK);
	}
	else{
		drawline(midX, midY, xHour[hour-1], yHour[hour-1], ST7735_BLACK);
	}
	
	drawline(midX, midY, testX[minute], testY[minute], ST7735_CYAN);
	drawline(midX, midY, xHour[hour], yHour[hour], ST7735_MAGENTA);

}

void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
