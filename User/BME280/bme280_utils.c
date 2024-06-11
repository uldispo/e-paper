

#include "bme280.h"
#include "bme280_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

inline static uint8_t SPI1_SendByte(uint8_t data);
inline static uint8_t SPI1_Readbyte(uint8_t reg_addr);
inline static void SPI1_Writebyte(uint8_t reg_addr, uint8_t val);


void user_delay_us(uint32_t period,void *intf_ptr)
{
	HAL_Delay(period);
}


inline static uint8_t SPI1_SendByte(uint8_t data)
{
	while(LL_SPI_IsActiveFlag_TXE(SPI1)==RESET);
	LL_SPI_TransmitData8(SPI1, data);
	
	while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);
	return LL_SPI_ReceiveData8(SPI1);
}


inline static uint8_t SPI1_Readbyte(uint8_t reg_addr)
{
	uint8_t val;

	CSB_L();
	SPI1_SendByte(reg_addr); 	
	val = SPI1_SendByte(0x00); 	//Send DUMMY to read data
	CSB_H();
	
	return val;
}



uint8_t user_spi_read(uint8_t reg_addr, uint8_t* data, uint8_t len, void *intf_ptr)
{
	unsigned int i = 0;
	if (!((SPI1)->CR1 & SPI_CR1_SPE)) {SPI1->CR1 |= SPI_CR1_SPE;}
	CSB_L();
	SPI1_SendByte(reg_addr); 	
	while(i < len)
	{
		data[i++] = SPI1_SendByte(0x00); 	//Send DUMMY to read data
	}
	CSB_H();
	return 0;
}


inline static void SPI1_Writebyte(uint8_t reg_addr, uint8_t val)
{
	CSB_L();
	SPI1_SendByte(reg_addr); 	
	SPI1_SendByte(val); 		//Send Data to write
	CSB_H();
}



uint8_t user_spi_write(uint8_t reg_addr, uint8_t* data, uint8_t len, void *intf_ptr)
{
	uint8_t i = 0;
	if (!((SPI1)->CR1 & SPI_CR1_SPE)) {SPI1->CR1 |= SPI_CR1_SPE;}
	CSB_L();
	SPI1_SendByte(reg_addr); 		
	while(i < len)
	{
		SPI1_SendByte(data[i++]); 	//Send Data to write
	}
	CSB_H();
	return 0;
}






