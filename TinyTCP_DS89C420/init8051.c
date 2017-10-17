/*
 * Copyright (C) 2002 by TechiZ. All rights reserved.
 *
 * This program was written in Korean(Comment and some more). 
 *
 * This program was developed by TechiZ(The Company name).
 * TechiZ want to share this program with you who loves the 8051 & the TCP/IP.
 * 
 * You DO NOT USE THIS CODE FOR COMMERCIAL PURPOSE.
 * This code is ONLY FREE FOR THE STUDY. 
 * If you want more, send me E-mail.
 *
 * E-mail: techiz@techiz.com
 * ( Subject is : [DS89C420 & TinyTCP] bla~ bla bla.... )
 *
 * Homepage: http://www.techiz.com
 * 
 * You DO NOT DELETE THIS COPYRIGHT MESSAGE IN THE USING OF THIS CODE.
 *
 * In the using of this code, TechiZ does NOT GUARANTEE ABOUT WORKING WITHOUT ERROR.
 */
 
/*******************************************************************/
/*                     S Y S T E M     I n i t i a l i z e                                                       */
/*******************************************************************/
#include <aduc812.h>
#include "depend.h"


void system_init(void);

extern union t {				/* time out variable for interrupt */
		word hl;
		byte reg[2];
		word watchdog_counter;
}time[2];

	
void system_init(void)   /*  hardware setting, it is very importand !!!		*/
{
		EA = 0;          /* ALL INTERRUPT DISABLE							*/
        
        time[0].reg[0] = TIMER0_INTERVAL_HIGH_BYTE;
		time[0].reg[1] = TIMER0_INTERVAL_LOW_BYTE;        
        TH0 = time[0].reg[0];
		TL0 = time[0].reg[1];
		time[0].watchdog_counter = 0;
		
        TMOD = 0x21;     /* T1 is AUTO RELOAD MODE and T0 is 16bit counter	*/
        				 /* TIMER1: GATE C/T M1 M0  TIMER0: GATE C/T M1 M0	*/
        TCON = 0x60;     /* TIMER 1 RUN FOR COMMUNICATION					*/
        				 /* TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0					*/
        SCON = 0x52;     /* SERIAL MODE=1,REN=1,TI=1						*/
        PCON = 0x80;     /* PCON <--#80H(PCOM.7=SMOD) DOUBLE BAUDRATE		*/
        TH1  = 0xff;     /* ff:57600bps (22.1184 Mhz)						*/
                         /* FD:9600bps (11.0592 Mhz)						*/
        IE   = 0x83;     /* EA=1 enable interrupt flag						*/
        				 /* EA ? ? PS PT1 PX1 PT0 PX0 						*/
        IT0  = 1;		 /* External INT0 edge trigger when setting			*/
		//IT0  = 0;		 /* External INT0 level trigger when setting		*/
        IP   = 0x01;     /* MAKE EX-INT0  HIGHest							*/
        /*        P1   = 0x01;     ROM EMULATE DISABLE(74245 G)				*/
		ET0  = 1;
        TR0	 = 1;
        T0   = 1;
        T1   = 0;
        EA   = 1; 		/* ALL INT enable */
}