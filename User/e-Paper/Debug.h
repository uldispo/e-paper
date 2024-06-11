/*****************************************************************************
* | File      	:	Debug.h
* | Author      :   Waveshare team
* | Function    :	debug with printf
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>
#include <inttypes.h>
#define DEBUG 1
#undef DEBUG
#if DEBUG
	#define DE_BUG(__info,...) printf("Debug: " __info,##__VA_ARGS__)
	#define SENSOR_DEBUG(...)
	
#else
	#define DE_BUG(__info,...)  
#endif

#endif
