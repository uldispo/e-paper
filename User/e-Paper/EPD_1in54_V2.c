/*****************************************************************************
* | File      	:   EPD_1in54_V2.c
* | Author      :   Waveshare team
* | Function    :   1.54inch e-paper V2
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_1in54_V2.h"
#include "Debug.h"
#include "rtc.h"
#include "main.h"
#include "stm32u0xx_ll_usart.h"
#include "usart.h"
#include "lpm.h"

// static uint32_t timeout_value = 30000;

// waveform full refresh
static unsigned char WF_Full_1IN54[159] =
    {
        0x80, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x40, 0x48, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x80, 0x48, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x40, 0x48, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x8, 0x1, 0x0, 0x8, 0x1, 0x0, 0x2,
        0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
        0x22, 0x17, 0x41, 0x0, 0x32, 0x20};

// waveform partial refresh(fast)
static unsigned char WF_PARTIAL_1IN54_0[159] =
    {
        0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x40, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
        0x02, 0x17, 0x41, 0xB0, 0x32, 0x28};

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_1IN54_V2_Reset(void)
{
    RST_H(); // DEV_Digital_Write(EPD_RST_PIN, 1);
    RST_L(); // DEV_Digital_Write(EPD_RST_PIN, 0);
    HAL_Delay(2);
    RST_H(); // DEV_Digital_Write(EPD_RST_PIN, 1);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_1IN54_V2_SendCommand(UBYTE Reg)
{
    DC_L();
    CS_L();
    DEV_SPI_WriteByte(Reg);
    CS_H();
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_1IN54_V2_SendData(UBYTE Data)
{
    DC_H();
    CS_L();
    DEV_SPI_WriteByte(Data);
    CS_H();
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
static void EPD_1IN54_V2_ReadBusy(void)
{

    uint32_t time1 = HAL_GetTick();
    //	bool result = GPIOA->regs->IDR & 0x0004; //returns true if A2 is HIGH
    while (GPIOA->IDR & 0b0000001000000000)
    { // A9, LOW = idle, HIGH = busy; DEV_Digital_Read(EPD_BUSY_PIN) == 1

        // Timeout check
        if ((HAL_GetTick() - time1) > timeout_value)
        {
            timeout_reset(__func__, __LINE__);
        }
    }

    //DE_BUG("busy: %d\r\n", (HAL_GetTick() - time1));
}

/******************************************************************************
function :	Turn On Display full
parameter:
******************************************************************************/
static void EPD_1IN54_V2_TurnOnDisplay(void)
{
    EPD_1IN54_V2_SendCommand(0x22);
    EPD_1IN54_V2_SendData(0xc7);
    EPD_1IN54_V2_SendCommand(0x20);

#if (USE_TIME_PROFILING == 1)
    uint32_t startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    uint32_t tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_TurnOnDisplay: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */
}

/******************************************************************************
function :	Turn On Display part
parameter:
******************************************************************************/
void EPD_1IN54_V2_TurnOnDisplayPart(void)
{
    EPD_1IN54_V2_SendCommand(0x22);
    EPD_1IN54_V2_SendData(0xcF);
    EPD_1IN54_V2_SendCommand(0x20);

    //  *********************   S L E E P  571 ms !  *********************
    // Wakeup Time Base = 8 /(32768)  = 0.244 ms
    // Wakeup Time = 0.244 ms  * WakeUpCounter
    // WakeUpCounter = Wakeup Time / (0.244 ms => 570ms /0.244ms=2336=0x920) /// 500/0.244ms=2049
    //	#define SHORT_WUT_WUTR               ((uint32_t)2336)     /* 570 ms -> WUTR = 1163 */

//    enter_stop2(2330, LL_RTC_WAKEUPCLOCK_DIV_8);

    //  *********************   End S L E E P  571 ms !  *********************

    EPD_1IN54_V2_ReadBusy();
}

static void EPD_1IN54_V2_Lut(UBYTE *lut)
{
    EPD_1IN54_V2_SendCommand(0x32);
    for (UBYTE i = 0; i < 153; i++)
        EPD_1IN54_V2_SendData(lut[i]);

#if (USE_TIME_PROFILING == 1)
    uint32_t startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    uint32_t tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_Lut: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */
}

static void EPD_1IN54_V2_SetLut(UBYTE *lut)
{
    EPD_1IN54_V2_Lut(lut);

    EPD_1IN54_V2_SendCommand(0x3f);
    EPD_1IN54_V2_SendData(lut[153]);

    EPD_1IN54_V2_SendCommand(0x03);
    EPD_1IN54_V2_SendData(lut[154]);

    EPD_1IN54_V2_SendCommand(0x04);
    EPD_1IN54_V2_SendData(lut[155]);
    EPD_1IN54_V2_SendData(lut[156]);
    EPD_1IN54_V2_SendData(lut[157]);

    EPD_1IN54_V2_SendCommand(0x2c);
    EPD_1IN54_V2_SendData(lut[158]);
}

static void EPD_1IN54_V2_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_1IN54_V2_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_1IN54_V2_SendData((Xstart >> 3) & 0xFF);
    EPD_1IN54_V2_SendData((Xend >> 3) & 0xFF);

    EPD_1IN54_V2_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_1IN54_V2_SendData(Ystart & 0xFF);
    EPD_1IN54_V2_SendData((Ystart >> 8) & 0xFF);
    EPD_1IN54_V2_SendData(Yend & 0xFF);
    EPD_1IN54_V2_SendData((Yend >> 8) & 0xFF);
}

static void EPD_1IN54_V2_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_1IN54_V2_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_1IN54_V2_SendData(Xstart & 0xFF);

    EPD_1IN54_V2_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_1IN54_V2_SendData(Ystart & 0xFF);
    EPD_1IN54_V2_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_1IN54_V2_Init(void)
{
    EPD_1IN54_V2_Reset();

#if (USE_TIME_PROFILING == 1)
    uint32_t startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    uint32_t tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_Init-1: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */

    EPD_1IN54_V2_SendCommand(0x12); // SWRESET

#if (USE_TIME_PROFILING == 1)
    startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_Init-2: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */

    EPD_1IN54_V2_SendCommand(0x01); // Driver output control
    EPD_1IN54_V2_SendData(0xC7);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SendCommand(0x11); // data entry mode
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SetWindows(0, EPD_1IN54_V2_HEIGHT - 1, EPD_1IN54_V2_WIDTH - 1, 0);

    EPD_1IN54_V2_SendCommand(0x3C); // BorderWavefrom
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SendCommand(0x18);
    EPD_1IN54_V2_SendData(0x80);

    EPD_1IN54_V2_SendCommand(0x22); // Load Temperature and waveform setting.
    EPD_1IN54_V2_SendData(0XB1);
    EPD_1IN54_V2_SendCommand(0x20);

    EPD_1IN54_V2_SetCursor(0, EPD_1IN54_V2_HEIGHT - 1);

#if (USE_TIME_PROFILING == 1)
    startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_Init-3: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */

    EPD_1IN54_V2_SetLut(WF_Full_1IN54);
}

/******************************************************************************
function :	Initialize the e-Paper register (Partial display)
parameter:
******************************************************************************/
void EPD_1IN54_V2_Init_Partial(void)
{
    EPD_1IN54_V2_Reset();

#if (USE_TIME_PROFILING == 1)
    uint32_t startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    uint32_t tmp = DWT->CYCCNT - startTick;
    printf("####  EPD_1IN54_V2_Init_Partial-1: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */

    EPD_1IN54_V2_SetLut(WF_PARTIAL_1IN54_0);
    EPD_1IN54_V2_SendCommand(0x37);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x40);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);

    EPD_1IN54_V2_SendCommand(0x3C); // BorderWavefrom
    EPD_1IN54_V2_SendData(0x80);

    EPD_1IN54_V2_SendCommand(0x22);
    EPD_1IN54_V2_SendData(0xc0);
    EPD_1IN54_V2_SendCommand(0x20);

#if (USE_TIME_PROFILING == 1)
    startTick = DWT->CYCCNT;
#endif /* USE_TIME_PROFILING */
    EPD_1IN54_V2_ReadBusy();
#if (USE_TIME_PROFILING == 1)
    tmp = DWT->CYCCNT - startTick;
    printf("###  EPD_1IN54_V2_Init_Partial-2: %d\n", (int)(tmp * 0.0000625));
#endif /* USE_TIME_PROFILING */
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_1IN54_V2_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_1IN54_V2_SendData(0XFF);
        }
    }
    EPD_1IN54_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_1IN54_V2_SendData(0XFF);
        }
    }
    EPD_1IN54_V2_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_1IN54_V2_Display(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_TurnOnDisplay();
}

/******************************************************************************
function :	 The image of the previous frame must be uploaded, otherwise the
                 first few seconds will display an exception.
parameter:
******************************************************************************/
void EPD_1IN54_V2_DisplayPartBaseImage(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_TurnOnDisplayPart();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_1IN54_V2_DisplayPart(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }

    EPD_1IN54_V2_TurnOnDisplayPart();
}
/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_1IN54_V2_Sleep(void)
{
    EPD_1IN54_V2_SendCommand(0x10); // enter deep sleep
    EPD_1IN54_V2_SendData(0x01);
    //    DEV_Delay_ms(100);
}
