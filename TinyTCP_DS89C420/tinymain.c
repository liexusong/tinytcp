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
 
#include <stdio.h>
#include <aduc812.h>
#include <string.h>
#include "depend.h"
#include "tinytcp.h"
#include "include.h"
#include "reg_8019.h"
	
#define	kbhit()	while(!(RI != 0))

WORD test_application( void );
WORD test_dataHandler( tcp_Socket *s, BYTE *dp, WORD len );
void system_init( void ); 

//WORD i_test;		//	from test_application
//BYTE tbuf_test[80];

procref	dataHandler[1];    /* called with incoming data */

extern in_Header *ip_tcp;
extern DWORD timeout_tcp, start_tcp;
extern DWORD x_tcp;

extern void print(BYTE *ch);
extern void tcp_Init(void);
extern void tcp_Listen(tcp_Socket *s, WORD port, DWORD timeout);
extern void tcp(void) reentrant;
extern void putb_ser(BYTE byte_data);
extern void tcp_Write( tcp_Socket *s, BYTE *dp, WORD len );
extern void tcp_Close(tcp_Socket *s);



typedef struct _test_buf {
	BYTE buf[80];
	WORD len;
} TBUF;

TBUF test_buf;
tcp_Socket t_socket;

int tiny_tcp_main( void )
{
    //kbhit();
	print(" ** TINYTCP Test Program **\r\n");
	tcp_Init();
	test_buf.len = 0;

	while ( 1 ) {
		memset( &t_socket, 0, sizeof( tcp_Socket ) );
		memset( &test_buf, 0, sizeof( TBUF ) );
		print(" # ");
		//tcp_Listen( &t_socket, 9999, test_dataHandler, 0 );
		tcp_Listen( &t_socket, 9999, 0 );


		printf("Our Ip address is %lx", sin_lclINAddr ) ;

		print(" tcp_Listen..");
		//tcp( test_application );
		test_application();
		tcp();
		print("test_application..");
	}
}
/*
WORD test_application( void )
{
	//WORD i;
	//BYTE tbuf[80];

	if ( test_buf.len > 0 ) {
		strcpy( tbuf_test, "You said," );
		strncat( tbuf_test, test_buf.buf, test_buf.len );
		for (i_test=0;i_test < test_buf.len;i_test++)
			putb_ser( test_buf.buf[i_test] );
		tcp_Write( &t_socket, tbuf_test, strlen(tbuf_test) );
		tcp_Close( &t_socket );
		test_buf.len = 0;
	}
	return 0;
}
*/
WORD test_application( void )
{
	WORD i;
	BYTE tbuf[80];

	if ( test_buf.len > 0 ) {
		EN0_RXCR = 0;      /* rx on(no broadcasts, no multicast,errors) */
		strcpy( tbuf, "You said," );
		strncat( tbuf, test_buf.buf, test_buf.len );
		//for (i=0;i < test_buf.len;i++)
		//	putb_ser( test_buf.buf[i] );
		tcp_Write( &t_socket, tbuf, strlen(tbuf) );
		//tcp_Close( &t_socket );
		test_buf.len = 0;
	}
	return 0;
}

WORD test_dataHandler( tcp_Socket *s, BYTE *dp, WORD len )
{
	//BYTE buf[80];
	WORD i;
	
	if ( dp == (BYTE *)0 ) {
		return (0);
	}

	if ( len > 0 ) {
		for ( i=0; i<len; i++ ) {
			test_buf.buf[i] = dp[i];
		}
		test_buf.len = len;
	}
	return (1);
}

	
void ethernet_address(void)
{
	printf("\n\rsed_lclEthAddr = %x\n\r",sed_lclEthAddr);
	printf("sed_ethBcastAddr= %x\n\r",sed_ethBcastAddr);
	printf("sin_lclINAddr = %x\n\r",sin_lclINAddr);
	printf("LOCAL_IP_ADDR = %x\n\r",LOCAL_IP_ADDR);
}



	/*
	
	
	#define	khbit()	( RI != 0 )

void (code *vreset)() = (void (*)())0x0 ;
extern byte get_serial(void) ;

//extern void reset (void) ;
WORD test_application( void )
{
	
	WORD len;
	WORD tread;
	
	if ( khbit() ) {
		if ( get_serial() == 'q' ) {
			print("warm boot\r\n") ;
			vreset() ;
		}
		
	*/