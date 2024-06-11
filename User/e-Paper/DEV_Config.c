/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V2.0
* | Date        :   2018-10-30
* | Info        :
# ******************************************************************************
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
#include "DEV_Config.h"
#include "main.h"
// #include "stm32l4xx_hal_spi.h"
// extern SPI_HandleTypeDef hspi1;

void DEV_SPI_WriteByte(uint8_t value)
{

	// Check if the SPI is enabled
	if (!((SPI1)->CR1 & SPI_CR1_SPE))
	{
		SPI1->CR1 |= SPI_CR1_SPE;
	}

	while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET)
		;

	CS_L();
	// Send bytes over the SPI
	LL_SPI_TransmitData8(SPI1, value);

	// Wait until the transmission is complete
	while (LL_SPI_IsActiveFlag_RXNE(SPI1) == RESET)
		;
	/* Read data register */
	(void)LL_SPI_ReceiveData8(SPI1);
	CS_H();
}

int DEV_Module_Init(void)
{
	DC_L();
	CS_H();
	RST_H(); // The Reset is active low.
	return 0;
}

void DEV_Module_Exit(void)
{
	DC_L();
	CS_L();
	// close 5V
	RST_L();
}
