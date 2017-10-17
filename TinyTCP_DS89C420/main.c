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
 
#include <aduc812.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg_8019.h"
#include "depend.h"
#include "global.h"
#include "proto.h"

/*****************************************************************************/
/*			Debug Mode                                                       */
/*****************************************************************************/
//#define STRING_TEST  /* sizeof(data type) */     
//#define RECEIVE_STATUS
//#define RTL8019AS_ID_CHECK
#define RECEIVE_TEST

/*****************************************************************************/
/*			Global variable                                                  */
/*****************************************************************************/

byte EthRxBuf[NBUF][SBUFSIZ];
byte EthTxBuf[BUFSIZ];

union t {				/* time out variable for interrupt */
		word hl;
		byte reg[2];
		word watchdog_counter;
}time[2];


/*****************************************************************************/
/*			Main routine start                                               */
/*****************************************************************************/

void main(void)
{
    
    system_init();
    //ethernet_init();
    print_logo();
    print("\n\rInitialize OK ..");
    while(1){
    	//EN_CMD = EN_PAGE0 + EN_NODMA + EN_START;
    	print("\n\r'Y2K.11.20:\\>");
        switch(get_serial_echo()){
			case '0': output_port0();break;
            case '1': output_port1();break;
            case '2': output_port2();break;
            case '3': output_port3();break;
            case '4': ethernet_register_test();break;
            case 'c':
			case 'C': clear_external_ram();break;
			case 'd': dump_auto = 1;
					  dump_memory();break;
			case 'D': dump_auto = 0;
					  dump_memory();break;
            case 'H':
			case 'h':
			case '?': help_message();break;
			case 'i':
			case 'I': input_port1();break;
			case ':': loading_ready = 1;
            case 'L':
			case 'l': loading_program();break;
			case 'q':
			case 'Q': quit_program();break;
			case 'r': 
			case 'R': reset();break;
			case 's': 
			case 'S': ethernet_address();break;
			case 't': 
           		case 'T': tiny_tcp_main();break;
			case '$': ethernet_test();break;
			case 'Z':
			case 'z': DoS();break;
			default : break;
		}
	}
}



/*****************************************************************************/
/*		              INTERRUPT SERVICE ROUTINE                              */
/*****************************************************************************/

void RTL8019AS_INT0() interrupt 0   /* External Interrupt 0 */
{
    /* you have to use data(Internal RAM) to access faster */
    /* ENISR_ALL value must be modified when you detect error or success and so on.. */
    byte e_isr;
    
    disable();	/*	EA = 0 global interrupt flag disable   */
    
    e_isr = EN0_ISR;
    P1_1 = !P1_1;	/* When it go into interrupt routine, this bit changes 1 and 0 */
    time[0].watchdog_counter = 0;	/*  clear to zero to inhibit watchdog */ 
    
    EN_CMD = EN_PAGE0 + EN_NODMA;    
    
    if ( e_isr & ( ENISR_OVER ) ) {	/* Receiver overwrote the ring */
		ei_rx_overrun();
	}
	else if ( e_isr & (ENISR_RX) ) {		/* Receive without error */
#ifdef RECEIVE_TEST
	ei_receive();		
#elif	
	receive_test();
#endif		
		EN0_ISR = ENISR_RX;
	}	
	if ( e_isr & ENISR_RX_ERR ) {		
		/* Receiver with error(CRC, frame aligement,FIFO overflow, receive packet failure  */
		//print_int("\n\rReceive error..");
		EN0_ISR = ENISR_RX_ERR;
	}
	if ( e_isr & ( ENISR_TX ) ) {		/* Transmit without error */
		//print_int("\n\rTx OK!");
		EN0_ISR = ENISR_TX;
	}
	if ( e_isr & ( ENISR_TX_ERR ) ) {	/* Transmit with error(collision, FIFO underrun) */
		//print_int("\n\rTx error..");
		EN0_ISR = ENISR_TX_ERR;     /* ? */
	}
	if ( e_isr & ( ENISR_COUNTERS ) ) {
		//print_int("\n\rCounter error..");
		rx_frame_errors  += EN0_COUNTER0;
		rx_crc_errors    += EN0_COUNTER1;
		rx_missed_errors += EN0_COUNTER2;
		EN0_ISR = ENISR_COUNTERS;	/*  Counters need emptying               */
	}
	if ( e_isr & ( ENISR_RDC ) ) {		/* Remote DMA complete */
		EN0_ISR = ENISR_RDC;
	}
	
		
	EN_CMD = EN_PAGE0 + EN_NODMA + EN_START;
	EN0_ISR = 0xff;
	
	//EN0_ISR = ENISR_ALL;
	enable();	/*	EA = 1 global interrupt flag enable    */
}

void TIMEOUT_T0_INT() interrupt 1
{
	T0 = !T0;		/* T0 pin	*/
	clock_MS++;			/* I don't know yet ??*/
	TH0 = time[0].reg[0];	/* <-- TIMER0_INTERVAL_HIGH_BYTE  */
	TL0 = time[0].reg[1];	/* <-- TIMER0_INTERVAL_LOW_BYTE   */
	
	if (time[0].watchdog_counter > WATCHDOG){
		time[0].watchdog_counter = 0;
		//print_int("S");
		//ethernet_init();
		//print_int("\n\rWatchDog bowwow~!!");
	}
	else{
		time[0].watchdog_counter += 1;
	}
}