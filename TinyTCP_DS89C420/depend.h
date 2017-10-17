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
 
/*****************************************************************************/
/*			Essential define (Dependency)                                    */
/*****************************************************************************/
#define     HIGH            1
#define     LOW             0
#define     XBYTE           ((unsigned char *) 0x20000L)	/* use @DPTR */
// use for 89c51rd2
#define     PBYTE           ((unsigned char *) 0x30000L)	/* use @R0   */
#define     DBYTE           ((unsigned char *) 0x40000L)	/* data      */
#define     CBYTE           ((unsigned char *) 0x50000L)	/* code      */

#ifndef byte
typedef 	unsigned long longword;     /* 32 bits          */
typedef		unsigned long dword;		/* 32 bits			*/
typedef 	unsigned short word;			/* 16 bits          */
typedef 	unsigned char byte;         /*  8 bits          */
typedef 	byte octet;                 /*  8 bits, for TCP */


#define		BYTE	byte
#define		WORD	word
#define		DWORD	dword
#endif
#define 	write_ram(address,value)		XBYTE [address] = value
#define		read_ram(address)		   		XBYTE [address] 
#define 	inportb(address)		   		XBYTE [address]
#define 	outportb(address,value)			XBYTE [address] = value
#define		outp(address,value)				XBYTE [address] = value
#define 	disable()						EA = 0
#define 	enable()						EA = 1
//#define 	HW_RESET        P1_7            /*  Hardware RESET use 8051 P1.7 */
#define 	DELAY           0x8000
#define		WATCHDOG		0x0800	/* This value is needed for watchdog timer */
#define		TIMER0_INTERVAL_HIGH_BYTE		0x00
#define		TIMER0_INTERVAL_LOW_BYTE        0x01


// adding for tinytcp
//#define REG(x)		(BYTE *)( x)	/* I don't know yet ??*/

typedef BYTE ETHADDR[6];
typedef DWORD IPADDR;

