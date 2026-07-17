/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"

#define USE_ICM20948  0
#define USE_UART  1
void cliBoot(cli_args_t *args);

//cmd_t cmd;

void apInit(void)
{
	uartOpen(_DEF_UART1, 115200);
	uartOpen(_DEF_UART2, 115200);
}

#define EchoTEST 	0
#define ButtonTEST	0
void apMain(void)
{
	uint8_t rx_buf;
	while(1)
	{
		if(uartAvailable(_DEF_UART1)>0)
		{
			uartPrintf(_DEF_UART1, "uart1_rx_data: %x", uartRead(_DEF_UART1));
		}

#if EchoTEST
		if(uartAvailable(_DEF_UART1)>0)
		{
			uartPrintf(_DEF_UART2, "uart1_rx_data: %x", uartRead(_DEF_UART1));
		}
		if(uartAvailable(_DEF_UART2)>0)
		{
			uartPrintf(_DEF_UART1, "uart2_rx_data: %x", uartRead(_DEF_UART2));
		}
#endif

#if ButtonTEST
		if(buttonGetPressed(0))
		{
			uartPrintf(_DEF_UART1, "buttonPressed");
			delay(100);
		}
#endif

	}
}
