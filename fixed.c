// fixed.c
// Created by: Sijin Woo
// UTEID: sw34773
// Section: 15665
// Contact: sijin.woo@utexas.edu
// Date Created: 1/18/2018
// Date of Last Revision: 1/21/2018
// Description: source file for the outputs onto the ST7735 LCD - Fixed point decimal, fixed point binary,
//		xy graph initialization, scatter plot

#include "fixed.h"
#include "ST7735.h"

#define OUTPUT_CMD_LENGTH	6		// Number of characters that will be on the LCD screen to make everything look nice

// Private function prototypes:
int32_t absValue(int32_t n);	// absolute value of n
void plotXYpoint(int32_t x, int32_t y);		// Plots the xy point onto the LCD
void drawPixels(uint32_t x, uint32_t y, uint32_t size, uint16_t color);		// Draws pixel onto LCD according to size

// Private gobal variables:
static int32_t XMinLim = 0;		// MIN value of x point that can be graphed
static int32_t XMaxLim = 0;		// MAX value of x point that can be graphed
static int32_t YMinLim = 0;		// MIN value of y point that can be graphed
static int32_t YMaxLim = 0;		// MAX value of y point that can be graphed
static uint32_t XScale = 0;		// scaling of x axis (distance between each pixel)
static uint32_t YScale = 0;		// scaling of y axis
static uint32_t XOrigin = 0;	// x point of origin of graph on LCD (0 <= x < 128)
static uint32_t YOrigin = 32;	// y point of origin of graph on LCD (32 <= y < 160)

//********************************************************************************
// Name: ST7735_sDecOut3
// If the signed 32-bit number is less than -9999 or greater than 9999, then *.*** will be outputted.
// If number within acceptable range, then the decimal is shifted to the left three places.
void ST7735_sDecOut3(int32_t n){
	if(n > 9999 || n < -9999){
		char nErrorStr[] = " *.***";		// If error, *.*** is outputted no matter sign
		ST7735_OutString(nErrorStr);		// Output to LCD
	}else{
		char nFixedPointStr[] = "  .   ";		// Template for what is going to be outputted
		
		// If n is negative, place negative sign
		if(n < 0){
			nFixedPointStr[0] = '-';
			n = -n;
		}
		
		for(int i = OUTPUT_CMD_LENGTH - 1; i > 0; i--){
			// Since at index 2 there must be a decimal, skip if this occurs
			if(i != 2){
				nFixedPointStr[i] = n % 10 + '0';
				n /= 10;
			}
		}
		ST7735_OutString(nFixedPointStr);		// Output to LCD
	}
}

//********************************************************************************
// Name: ST7735_uBinOut8
// The following function prints a fixed point unsigned binary representation of the unsigned 32-bit input number
// Input: unsigned 32-bit number
// Output: None
void ST7735_uBinOut8(uint32_t n){
	if(n >= 256000){
		char nErrorStr[] = "***.**";		// Error string
		ST7735_OutString(nErrorStr);		// Output to LCD
	}else{
		uint32_t uBinFixPtNum = (100 * n >> 8) ;	// resolution = .01
		
		//n = (double)(n / 2.56) + .5;		// 2^8 = 256 resolution; the .5 is to round a value up because the program truncates the decimal
		char uBinFixPtStr[] = "   .  ";
		for(int i = OUTPUT_CMD_LENGTH - 1; i >= 0; i--){
			// At index less than 2, 0's should not be outputted if n is already 0
			if(i < 2 && uBinFixPtNum == 0){
				break;
			}
			
			// At index 3 of nBinFixedStr, there is a decimal
			if(i != 3){
				uBinFixPtStr[i] = uBinFixPtNum % 10 + '0';
				uBinFixPtNum /= 10;
			}
		}
		ST7735_OutString(uBinFixPtStr);	// Output to LCD
	}
}

//********************************************************************************
// Name: ST7735_XYplotInit
// The following function creates initializes the LCD ST7735 screen to output a graph
// Input: char pointer to title, signed 32-bit min and max of x and y
// Output: None
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);				// Reset screen to black
	ST7735_FillRect(0, 32, ST7735_TFTWIDTH, ST7735_TFTWIDTH, ST7735_Color565(228,228,228));	// Space for the graph (light gray)
	ST7735_SetCursor(0, 0);			// Reset cursor
	ST7735_OutString(title);		// Output title onto LCD
	
	// Set x limits
	XMinLim = minX;
	XMaxLim = maxX;
	
	// Set y limits
	YMinLim = minY;
	YMaxLim = maxY;
	
	// Set distance between each pixel
	int32_t xTotalDist = XMaxLim - XMinLim;
	int32_t yTotalDist = YMaxLim - YMinLim;
	XScale = (double)xTotalDist / ST7735_TFTWIDTH + .5;		// .5 is for rounding the number up if decimal is greater than .5
	YScale = (double)yTotalDist / ST7735_TFTWIDTH + .5;
	
	// Save pixel addresses for origin of graph
	// The x part of the origin is the number pixels from the left depending on minX
	XOrigin = ((double)absValue(minX) / xTotalDist) * (ST7735_TFTWIDTH - 1);
	// The y part of the origin is the number pixels from the top depending on minY 
	YOrigin = ST7735_TFTHEIGHT - ((double)absValue(minY) / yTotalDist) * (ST7735_TFTWIDTH - 1);
}

//********************************************************************************
// Name: ST7735_XYplot
// The following function graphs the buffers of the x and y coordinates
// Input: number of coordinates, signed 32 -bit x and y buffers
// Output: None
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	for(int i = 0; i < num; i++){
		plotXYpoint(bufX[i], bufY[i]);
	}
}

//********************************************************************************
// Name: plotXYpoint
// The following private function plots a (x,y) point on the graph of the LCD
// Input: signed 32-bit x and y coordinates
// Output: None
// Note: Four pixels (2 by 2) are drawn for better visualization on graph. The actual point will on the upper left corner
void plotXYpoint(int32_t x, int32_t y){
	// If a point is beyond the scope of the graph limits, then nothing is graphed
	if(!(x > XMaxLim || x < XMinLim || y > YMaxLim || y < YMinLim)){
		int32_t xOffset = (double)x / XScale + .5;		// The offset from the origin to draw the pixel
		int32_t yOffset = (double)y / YScale + .5;		// The .5 is meant to round the number of if above __.5
		// NOTE: positive y direction is up on LCD but the address decreases as y increases on graph so thats why YOrigin - yOffset
		drawPixels(XOrigin + xOffset, YOrigin - yOffset, GRAPH_POINTS_PIXEL_SIZE, 0);
		// ST7735_DrawPixel(XOrigin + xOffset, YOrigin - yOffset, 0);	// Use this line if only 1 pixel to be drawn on LCD
	}
}

//********************************************************************************
// Name: drawPixels
// The following pixel fills in n by n pixel with the reference at the upper left corner
// Input: unsigned 32-bit x and y addresses of ST7735, size of the point (size by size), color of coordinate
// Output: None
void drawPixels(uint32_t x, uint32_t y, uint32_t size, uint16_t color){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			ST7735_DrawPixel(x + i, y + j, color);
		}
	}
}

//********************************************************************************
// Name: absValue
// The following private function returns the absolute value of a number
// Input: signed 32-bit number
// Output: positive value of signed 32-bit input
int32_t absValue(int32_t n){
	if(n < 0){
		n = -n;
	}
	return n;
}
void drawline (int X1, int Y1, int X2, int Y2, uint32_t color){

	double slopeRounding;
	double slope;
	int Xlength, Ylength, X, Y, minX, maxX, minY, maxY,absX,absY;
	Xlength = X2 - X1;
	Ylength = Y2 - Y1;

	X = X1;
	Y = Y1;
	slopeRounding = 0.0;
	minX = X1;
	maxX = X2;
	minY = Y1;
	maxY = Y2;
	
	if (Ylength < 0){
		absY = Ylength *-1;
	}
	else{
		absY = Ylength;
	}
	if (Xlength < 0){
		absX = Xlength *-1;
	}
	else{
		absX = Xlength;
	}
	if (X1 > X2){
		minX = X2;
		maxX = X1;
		minY = Y2;
		maxY = Y1;
		Y = Y2;
		Xlength = X1 - X2;
		Ylength = Y1 - Y2;
		
	}
	
	int slopenum; 
	int slopeden;
	int currentx = X1;
	int currenty = Y1;
	int counter = 0;
	slopenum = Y2 - Y1;
	slopeden = X2 - X1;
	if ( absValue(slopeden) > absValue(slopenum) ){
		int curCount = 1;
		if (slopeden > 0){
			counter = 1;
		} else {
			counter = -1;
		}
		while (currentx != X2){
			ST7735_DrawPixel(currentx, currenty, color);
			currentx += counter;
			currenty = (Y1 * slopeden + counter * curCount * slopenum) / (slopeden); 
			++curCount;
		}
	} else {
		int curCount = 1;
		if (slopenum > 0){
			counter = 1;
		} else {
			counter = -1;
		}
		while (currenty != Y2){
			ST7735_DrawPixel(currentx, currenty, color);
			currenty += counter;
			currentx = (X1 * slopenum + counter * curCount * slopeden) / (slopenum);
			++curCount;
		}
	}
		/*
	if(minX < maxX){
			//slope = (double) ((double)maxY - (double)minY)/ (double) ((double)maxX - (double)minX);
			slope = (maxY - minY)/(maxX - minX);
			/*for(X = minX; X<maxX ; X++){
				ST7735_DrawPixel(X, Y,ST7735_BLUE);
				Y = Y + slope;
				slopeRounding = slopeRounding + slope;
				/*while (slopeRounding >= .5){
						Y =  Y + slope;
					slopeRounding = slopeRounding - 1;
				}
				
				if (slope > 0){ //negative slope
					Y = Y + slope;
					if (slopeRounding > -.5){
						Y = Y + slope;
					}
					else{
						while(slopeRounding <= -.5){
							
						Y = Y + slope + 1;
						slopeRounding = slopeRounding + 1;
						}
					}
				}
				else{ //0 or positive
					Y = Y + slope;
					if (slopeRounding < .5){
						Y = Y + slope;					
					}
					else{
						while(slopeRounding >= .5){
							Y = Y + slope;
							slopeRounding = slopeRounding -1;
						}
					
					}
				}
				
			}
		
		}
		else if(maxX == minX){// Vertical Line
			if (Y1 > Y2){
				Y = Y2;
				maxY = Y1;
			}
			else{
				Y = Y1;
				maxY = Y2;
			}
			for (; Y < maxY; Y++){
					ST7735_DrawPixel(X, Y, ST7735_MAGENTA);
			}
			
		}*/
}
void makeCircle(uint16_t x0, uint16_t y0) {
	{
  int x = 50;
  int y = 0;
  int decisionOver2 = 1 - x;

  while( y <= x )
  {
    ST7735_DrawPixel( x + x0,  y + y0, ST7735_YELLOW);
    ST7735_DrawPixel( y + x0,  x + y0, ST7735_YELLOW);
    ST7735_DrawPixel(-x + x0,  y + y0, ST7735_YELLOW);
    ST7735_DrawPixel(-y + x0,  x + y0, ST7735_YELLOW);
    ST7735_DrawPixel(-x + x0, -y + y0, ST7735_YELLOW);
    ST7735_DrawPixel(-y + x0, -x + y0, ST7735_YELLOW);
    ST7735_DrawPixel( x + x0, -y + y0, ST7735_YELLOW);
    ST7735_DrawPixel( y + x0, -x + y0, ST7735_YELLOW);
    y++;
    if (decisionOver2<=0)
    {
      decisionOver2 += 2 * y + 1;
    }
    else
    {
      x--;
      decisionOver2 += 2 * (y - x) + 1;
    }
  }
}
}
