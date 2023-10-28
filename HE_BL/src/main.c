/*
 * main.c
 *
 *  Created on: Oct 13, 2020
 *      Author: Ahmed Salem
 */



#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "RCC_interface.h"
#include "DIO_interface.h"
#include "STK_interface.h"
#include "USART_interface.h"
#include "FPEC_interface.h"
#include "NVIC_INTERFACE.h"



#define STK_TIME    9000000

void Parser_voidParseRecord(u8* Copy_u8BufData);

volatile u8  u8RecBuffer[100]   ;
volatile u8  u8RecCounter    = 0;
volatile u8  u8TimeOutFlag   = 0;
volatile u16 u16TimerCounter = 0;
volatile u8  u8BLWriteReq    = 1;

/******************************************************************/
/* System Flags  Initialize values are {0} and to make any conditions for the true you should write {1} on them */

u8 FLAG = 0;
u8 BL_STAY_IN_BL_FLAG  = 0; // Flag used to stay in bootloader
u8 BL_ACC_BL_REC       = 0; // Flag used to Accept records from uart
u8 BL_JUMP_TO_APP      = 0; // Flag used to Jump to certain application
/******************************************************************/
/******************************************************************/
/* PINS Checkers  **/

#define BL_PIN_Befor_UPLOAD  PIN1
#define BL_PIN_FLASHING_UPLOAD  PIN2

/******************************************************************/
typedef void (*Function_t)(void);
Function_t addr_to_call = 0;

void Jump_To_CertainAPP(void)
{

	#define SCB_VTOR   *((volatile u32*)0xE000ED08)
	SCB_VTOR = 0x08001000;
	addr_to_call = *(Function_t*)(0x08001004);
	addr_to_call();

	//MGPIO_VidSetPinValue(GPIOA,PIN2,1);

}


void main(void)
{
	u8 Local_u8RecStatus;

	RCC_voidInitSysClock();
	RCC_voidEnableClock(RCC_APB2,14); /* USART1 */
	RCC_voidEnableClock(RCC_APB2,2);  /* PortA  */
	RCC_voidEnableClock(RCC_AHB,4);   /* FPEC   */


	MGPIO_VidSetPinDirection(GPIOA,0,0b1010);   /* TX AFPP */
	MGPIO_VidSetPinDirection(GPIOA,9,0b1010);   /* TX AFPP */
	MGPIO_VidSetPinDirection(GPIOA,10,0b0100);  /* RC Input Floating */

	MGPIO_VidSetPinDirection(GPIOA,PIN1,OUTPUT_SPEED_10MHZ_PP);
	MGPIO_VidSetPinDirection(GPIOA,PIN3,OUTPUT_SPEED_10MHZ_PP);
	MGPIO_VidSetPinDirection(GPIOA,PIN2,OUTPUT_SPEED_10MHZ_PP);

	MGPIO_VidSetPinValue(GPIOA,PIN1,1);


	MUSART1_voidInit();
	MSTK_voidInit();


	MSTK_voidSetIntervalSingle(STK_TIME,Jump_To_CertainAPP);

	while(u8TimeOutFlag == 0)
	{
		Local_u8RecStatus = MUSART1_u8Receive( &(u8RecBuffer[u8RecCounter]) );

		if (Local_u8RecStatus == 1)
		{
			MSTK_voidStopInterval();

			if(u8RecBuffer[u8RecCounter] == '\n')
					{
				MGPIO_VidSetPinValue(GPIOA,PIN3,1);

				if (u8BLWriteReq == 1)
						{
							/* The flash should be erased only once to write on the erased address */
							FPEC_voidEraseAppArea();
							u8BLWriteReq = 0; // To make sure that flash is erased only once
							MGPIO_VidSetPinValue(GPIOA,PIN1,0);

						}

						/* Start parsing the new records */
						Parser_voidParseRecord(u8RecBuffer);

						// As the tool that sends the Hex records rewuires "ok" massege to send the reccords
						MUSART1_voidTransmit("ok");

						/* To start recieving from first of the data buffer */
						u8RecCounter = 0;
					}

					else
					{
						u8RecCounter ++ ;
					}


			if(u8RecBuffer[u8RecCounter] == '/') // Indication of the end of hex file 
			{
				Jump_To_CertainAPP();
			}
				}

				else
				{

				}
			}
		}
