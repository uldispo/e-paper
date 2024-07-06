/******************************************************************************
 * | File      	:   paint_sensor.c
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <math.h>
#include <stdbool.h>
#include "printf.h"

#include "GUI_Paint.h"
#include "DEV_Config.h"
#include "EPD_1in54_V2.h"
#include "Debug.h"
#include "paint_sensor.h"
#include "Debug.h"

extern uint16_t H_old;
extern uint16_t T_old;
extern uint16_t vbat_old;

/* Buffers used for displaying Time and Date */
static char aEndTime[20] = {0};
static char aEndDate[20] = {0};

UBYTE *BlackImage;
// UWORD Imagesize;
static char str_array[7] = {0};

struct X0_Y0
{
	uint8_t x;
	uint8_t y;
};

static const struct X0_Y0 big_1 = {10, 30};	   //	Big digit 2
static const struct X0_Y0 big_0 = {67, 30};	   //	Big digit 1
static const struct X0_Y0 small_0 = {142, 46}; //	Small digit 0

// *************************************************************************************

void battery_out(uint16_t bat)
{ // Battery voltage out  X.Y

	printf("**  Vbat out\n");
	uint8_t x = 3;
	uint8_t y = 160;

	sprintf(str_array, "%2d", bat);
	Paint_ClearWindows(x, y, x + 40, y + 29, WHITE);

	Paint_DrawChar(x, y, str_array[0], &calibri_24pts, BLACK, WHITE); // 	12, 29, 8
	Paint_DrawChar(x + 12, y, str_array[1], &calibri_24pts, BLACK, WHITE);
	Paint_DrawChar(x + 12 + 12, y, str_array[2], &calibri_24pts, BLACK, WHITE);

	//		EPD_1IN54_V2_DisplayPart(BlackImage);
}

void temperature_out(uint16_t tempr)
{
	printf("**  T out\n");
	sprintf(str_array, "%3d", tempr);
	Paint_ClearWindows(big_1.x, big_1.y, 142 + 41, big_1.y + 101, WHITE); // 40 ms

	Paint_DrawChar(big_1.x, big_1.y, str_array[0], &calibri_104pts, BLACK, WHITE); // 15 ms
	Paint_DrawChar(big_0.x, big_0.y, str_array[1], &calibri_104pts, BLACK, WHITE);
	Paint_DrawPoint(131, 100, BLACK, DOT_PIXEL_5X5, DOT_FILL_AROUND); // Draw decimal point
	Paint_DrawChar(small_0.x, small_0.y, str_array[2], &calibri_80pts, BLACK, WHITE);

	//	EPD_1IN54_V2_DisplayPart(BlackImage);	// 0.84 sec ms
}

void humidity_out(uint16_t hum)
{
	printf("**  H out\n");
	uint8_t x = 125;
	uint8_t y = 150;
	//  char pcent = '%';
	//  sprintf(str_array, "%2d%%", hum);
	//  append % to string
	//  strncat(str_array, &pcent, 1);
	snprintf(str_array, sizeof(str_array), "%2d", hum);					// Format the number
	strncat(str_array, "%", sizeof(str_array) - strlen(str_array) - 1); // Append %

	Paint_ClearWindows(x, y, 200, 197, WHITE);									// 10^1
	Paint_DrawString_EN(x, y, (char *)str_array, &calibri_36pts, WHITE, BLACK); // 20, 34
}

//  **************************************************************************************

int ESP_Init(void)
{
	printf("ESP_Init\n");
	DEV_Module_Init();	 // DC_L();SPI_CS_H();RST_H(); - set initial values
	EPD_1IN54_V2_Init(); // Reset pin set low, set LUT etc.

	// Create a new image cache
	/* !!!  you have to edit the startup_stm32fxxx.s file and set a heap size = 1500 !!!  */
	UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
	if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
	{
		printf("Failed to apply for black memory...\r\n");
		return -1;
	}

	Paint_NewImage(BlackImage, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 0, WHITE);

	//  	Paint_Clear(WHITE);
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);
	Paint_DrawRectangle(2, 3, 198, 140, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

	EPD_1IN54_V2_Display(BlackImage); // Write data to display's RAM

	EPD_1IN54_V2_DisplayPartBaseImage(BlackImage);
	EPD_1IN54_V2_Init_Partial();

	return 0;
}

void final_message(uint16_t bat_voltage)
{ // sleep

	ESP_Init();
	uint16_t Xstart = 5;
	uint16_t Ystart = 2;

	EPD_1IN54_V2_Init();
	Paint_SelectImage(BlackImage); // Paint.Image = image;
	Paint_Clear(WHITE);

	sprintf(str_array, "%d", bat_voltage);
	Paint_DrawString_EN(Xstart, Ystart, "Vbat:", &calibri_20pts, WHITE, BLACK); // font size 17,24
	Paint_DrawString_EN(Xstart + 7 * 10, Ystart, str_array, &calibri_20pts, WHITE, BLACK);

	Show_RTC_Calendar();
	Paint_DrawString_EN(Xstart, Ystart + 75, "EndTime:", &calibri_20pts, WHITE, BLACK);
	Paint_DrawString_EN(Xstart, Ystart + 100, aEndTime, &calibri_20pts, WHITE, BLACK);
	Paint_DrawString_EN(Xstart + 6 * 10 + 20, Ystart + 100, aEndDate, &calibri_20pts, WHITE, BLACK);

	EPD_1IN54_V2_Display(BlackImage);
	EPD_1IN54_V2_Sleep();
}

void Show_RTC_Calendar(void)
{
	/* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
	/* Display time Format : hh:mm:ss */
	sprintf((char *)aEndTime, "%.2d:%.2d:%.2d", __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)),
			__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)),
			__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC)));

	/* Display date Format : mm-dd-yy */
	sprintf((char *)aEndDate, "%.2d-%.2d-%.2d", __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC)),
			__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC)),
			2000 + __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC)));
	printf("%s\t %s\n", aEndTime, aEndDate);
}

//	****************_ ESP_Init_after_sleep, screen hold previous image_**************
int ESP_Init_after_sleep(void)
{
	printf("ESP_Init_after_sleep\n");
	// Create a new image cache
	/* !!!  you have to edit the startup_stm32fxxx.s file and set a heap size = 1400 !!!  */
	UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
	if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
	{
		printf("Failed to apply for black memory...\r\n");
		return -1;
	}
	Paint_NewImage(BlackImage, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 0, WHITE);

	// Drawing on the image ======================================================
	//    INFO("Drawing\r\n");

	Paint_SelectImage(BlackImage); // Paint.Image = image;
	Paint_Clear(WHITE);
	Paint_DrawRectangle(2, 3, 198, 140, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

	// T
	sprintf(str_array, "%3d", T_old);
	Paint_DrawChar(big_1.x, big_1.y, str_array[0], &calibri_104pts, BLACK, WHITE); // 15 ms
	Paint_DrawChar(big_0.x, big_0.y, str_array[1], &calibri_104pts, BLACK, WHITE);
	Paint_DrawPoint(131, 100, BLACK, DOT_PIXEL_5X5, DOT_FILL_AROUND); // Draw decimal point
	Paint_DrawChar(small_0.x, small_0.y, str_array[2], &calibri_80pts, BLACK, WHITE);

	// hum
	uint8_t x = 125;
	uint8_t y = 150;
	// char pcent = '%';
	//  sprintf(str_array, "%2d%%", H_old);
	snprintf(str_array, sizeof(str_array), "%2d", H_old); // Format the number
	// append % to string
	// strncat(str_array, &pcent, 1);
	strncat(str_array, "%", sizeof(str_array) - strlen(str_array) - 1);			// Append %
	Paint_DrawString_EN(x, y, (char *)str_array, &calibri_36pts, WHITE, BLACK); // 20, 34

	// battery voltage
	x = 3;
	y = 160;
	sprintf(str_array, "%3d", vbat_old);

	Paint_ClearWindows(x, y, x + 40, y + 29, WHITE);
	Paint_DrawChar(x, y, str_array[0], &calibri_24pts, BLACK, WHITE); // 	12, 29, 8
	Paint_DrawChar(x + 12, y, str_array[1], &calibri_24pts, BLACK, WHITE);
	Paint_DrawChar(x + 12 + 12, y, str_array[2], &calibri_24pts, BLACK, WHITE);

	//		EPD_1IN54_V2_Display(BlackImage);
	//		EPD_1IN54_V2_DisplayPartBaseImage(BlackImage);
	//		EPD_1IN54_V2_Init_Partial();

	return 0;
}
