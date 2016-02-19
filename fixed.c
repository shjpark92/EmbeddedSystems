// File Name: fixed.c
// Authors: Sung Park and Wallace Tran
// Initial Creation Date: August 28, 2015
// Description: Displays signed decimal fixed point, unsigned binary fixed point,
//  initializes plot for display on LCD, and displays an array of points.
// Lab Number: #16520
// TA: Dylan Zika
// Date of last revision: September 6, 2015
// Hardware Configuration: connected to ST7735R

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "inc/tm4c123gh6pm.h"

#define ST7735_Y_OFFSET 32

static int32_t RangeX, RangeY;
static float RatioX, RatioY;

/****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 
void ST7735_sDecOut3(int32_t inputValue) 
{
  char output[6];
  if(inputValue > 9999 || inputValue < -9999) 
  {
    sprintf(output, " *.***");
  } 
  else 
  {
    float outputValue = inputValue / 1000.0;
    if(inputValue >= 0) 
    {
      sprintf(output, " %.3f", outputValue);
    } 
    else 
    {
      sprintf(output,"%.3f", outputValue);
    }
  }
  ST7735_OutString((char*)output);
}


/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0    "  0.00"
     2    "  0.01"
    64    "  0.25"
   100    "  0.39"
   500    "  1.95"
   512    "  2.00"
  5000    " 19.53"
 30000    "117.19"
255997    "999.99"
256000    "***.**"
*/
void ST7735_uBinOut8(uint32_t inputValue) 
{
  char output[6];
  if(inputValue >= 256000)
  {
    sprintf(output, "***.**");
  } 
  else 
  {
    float outputValue = (float)inputValue / 256.0;
		outputValue = floorf(outputValue * 100) / 100;
    sprintf(output, "%6.2f", outputValue);
  }
  ST7735_OutString((char*)output);
}

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) 
{
  ST7735_FillScreen(0);
  ST7735_SetCursor(0, 0);
  ST7735_OutString(title);
  ST7735_FillRect(0, 32, 128, 128, ST7735_Color565(228, 228, 228));

  RangeX = (maxX - minX) - maxX;
  RangeY = (maxY - minY) + minY;
  RatioX = 128 / (float)(maxX - minX);
  RatioY = 128 / (float)(maxY - minY);
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]) 
{
  while (num != 0) 
  {
    int32_t x = (RangeX + bufX[num]) * RatioX;
    int32_t y = (RangeY - bufY[num]) * RatioY;
    ST7735_DrawPixel(x, y + ST7735_Y_OFFSET, ST7735_BLUE);
    num--;
  }
}
