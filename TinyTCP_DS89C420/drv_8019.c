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
/*		              RTL8019AS DRIVER FUNCTIONS                             */
/*****************************************************************************/
#include <stdio.h>
#include <aduc812.h>
#include "reg_8019.h"
#include "depend.h"

#define WARNING	

/* H/W and IP Address */
extern ETHADDR sed_lclEthAddr;
//extern ETHADDR sed_ethBcastAddr;

void ethernet_register_test(void);
//void ethernet_get_8390_hdr(word StartAddr, word Count);
void ethernet_get_8390_hdr(word StartAddr, word Count) reentrant;
void ei_rx_overrun(void);
void ethernet_init(void);
void ethernet_test(void);
void DMA_write(byte *buffer, word StartAddr, word Count);
void ei_receive(void) reentrant;
//void ei_input(byte *buf, word StartAddr, word Count);
void ei_input(byte *buf, word StartAddr, word Count) reentrant;
void ei_output( BYTE *buf, WORD StartAddr, WORD Count );
BYTE sed_Receive( BYTE *buf );
BYTE *sed_IsPacket(void);
BYTE *sed_FormatPacket( BYTE *destEAddr, WORD ethType );
BYTE sed_CheckPacket( BYTE *BufLocation, WORD expectedType );
BYTE sed_Send( WORD pkLengthInOctets );

extern void delay(word d);
extern void delay_1ms(int times);
extern void putb_ser(byte byte_data);
extern void print(byte *ch);
extern void print_int(const byte *ch);
extern word address_ascii_to_hex_echo(void);
extern byte two_ascii_to_hex(void);
extern void Move( BYTE *src, BYTE *dest, WORD numbytes );

extern byte current_page;
extern byte next_pkt;
extern byte rx_frame_errors;  
extern byte rx_crc_errors;    
extern byte rx_missed_errors;
extern byte EthRxBufRdPtr;
extern byte EthRxBufWrPtr;
extern byte ethernet_8390_hdr[4];
extern byte EthRxBuf[NBUF][SBUFSIZ];
extern byte EthTxBuf[BUFSIZ];
extern longword	pkt_cnt;

extern struct e8390_pkt_hdr {
	byte status; 	/* status of receiver */
	byte next;   	/* pointer to next packet. */
	byte countl;	/* header + packet length in bytes */
	byte counth;
};

void DoS(void);

void DoS(void)
{
	byte temp,count = 0x01,temp1=count;
	word test_packet1_size;
	code byte test_packet1[] = {0x00, 0x10, 0x4b, 0x18, 0xd8, 0x30, 0x00, 0x10,\
						0x4b, 0x18, 0xd8, 0x31, 0x08, 0x00, 0x45, 0x00,\
						0x00, 0x30, 0x05, 0x11, 0x40, 0x00, 0x80, 0x06,\
						0x00, 0x00, 0xca, 0x1e, 0x14, 0xac, 0xca, 0x1e,\
						0x14, 0xaa, 0x00, 0x50, 0x04, 0x47, 0xb2, 0x0e,\
						0x02, 0x66, 0x4c, 0xb8, 0xec, 0x0a, 0x70, 0x12,\
						0x44, 0x70, 0x8f, 0x3d, 0x00, 0x00, 0x02, 0x04,\
						0x05, 0xb4, 0x01, 0x01, 0x04, 0x02};
	EN0_IMR = 0x00;		/* 송신중에는 인터럽트가 안걸리게 막아 놓는다. */ 
	test_packet1_size = sizeof(test_packet1);						
	
	while(count--){
		while ((temp=EN_CMD) & EN_TRANS);
		DMA_write(test_packet1, 0x4000, test_packet1_size);
		EN0_TPSR = NE_START_PG;      /* Transmit starting page  */
		EN0_TCNTLO = test_packet1_size;   /*  Low  byte of tx byte count  */
    	EN0_TCNTHI = 0;     /*  High byte of tx byte count  Transmit byte count register */
		EN_CMD = EN_PAGE0 + EN_NODMA + EN_TRANS + EN_START;   /* Transmit a frame  */
	}
	EN0_IMR = ENISR_ALL; /* INTerrupt mask reg */
	print("\n\rTotal Transmitted Packet is ");putb_ser(temp1);
}

BYTE sed_Receive( BYTE *buf )
{
	return 0;
}

BYTE *sed_IsPacket(void)
{
	BYTE *buf;
	//BYTE *buf;
	
	EthRxBufRdPtr++;
	if ( EthRxBufRdPtr == NBUF )
		EthRxBufRdPtr = 0;
	
	if( EthRxBufRdPtr == EthRxBufWrPtr ) {
		if ( EthRxBufRdPtr ) EthRxBufRdPtr--;
		else EthRxBufRdPtr = NBUF-1;
		return (BYTE *)NULL;
	}

	buf = &EthRxBuf[EthRxBufRdPtr][14]; /* ? size of array ? start of IP header */

	if( EthRxBufRdPtr == NBUF )
		EthRxBufRdPtr = 0;
	return buf;
}

/*  Make a MAC header */
BYTE *sed_FormatPacket( BYTE *destEAddr, WORD ethType )
{
	Move( destEAddr, EthTxBuf, 6 );     /*	Make a destination address */
	Move( sed_lclEthAddr, EthTxBuf+6, 6 );	/* Make a source address */ 

	*(WORD *)(EthTxBuf+12) = ethType;	/*	Make a Ethertype	*/
	return ( EthTxBuf+14 );
}

BYTE sed_CheckPacket( BYTE *BufLocation, WORD expectedType )
{
	if ( *(WORD *)(BufLocation-2) != expectedType ) {
		return ( 0 );
	}
	return (1);
}

BYTE sed_Send( WORD pkLengthInOctets )
{
	//WORD i;
	
	if (EthTxBuf == NULL)
		return 0;

	if (pkLengthInOctets <= 0)
		return 0;
	
	pkLengthInOctets += 14;					/* Ethernet Header */
	if ( pkLengthInOctets < MIN_PACKET_SIZE )
		pkLengthInOctets = MIN_PACKET_SIZE+4;		/* CRC is 4Byte */

	/* Mask interrupts from the ethercard. */
	EN0_IMR = 0x00; /* ? */

	/* Wait for other transmit */
	while ( EN0_ISR & E8390_TRANS );
	/* We should already be in page 0, but to be safe... */
	EN_CMD = E8390_PAGE0 + E8390_START + E8390_NODMA;

#ifndef NE8390_RW_BUGFIX
	EN0_RCNTLO = 0x42;
	EN0_RCNTHI = 0x00;
	EN0_RSARLO = 0x42;
	EN0_RSARHI = 0x00;
	EN_CMD = E8390_RREAD + E8390_START;
	delay_1ms(10);
	
#endif
	
	/* copy RAM data to RTL8019AS iRAM with (from MAC to end of data ) */
	ei_output( EthTxBuf, TX_START_PG << 8, pkLengthInOctets );

	/* Just send it, and does not check */
	EN0_TCNTLO = pkLengthInOctets& 0xff;
	EN0_TCNTHI = pkLengthInOctets>> 8;
	EN0_TPSR = TX_START_PG;
	EN_CMD = E8390_NODMA + E8390_TRANS + E8390_START;
	
	/* Turn 8390 interrupts back on. */
	EN0_IMR = ENISR_ALL;
	return 1;
}

/* copy RAM data to RTL8019AS Internal RAM */ 
void ei_output( BYTE *buf, WORD StartAddr, WORD Count )
{
	word loop;
	
	EN0_ISR = ENISR_RDC;
	/* Now the normal output. */
	EN0_RCNTLO = Count & 0xff;
	EN0_RCNTHI = Count >> 8;
	EN0_RSARLO = StartAddr & 0xFF;
	EN0_RSARHI = StartAddr >> 8;
	EN_CMD = E8390_RWRITE + E8390_START + E8390_PAGE0;
	for(loop=0;loop < Count;loop++){
        EN_DATA = *buf++;
	}
	EN0_ISR = ENISR_RDC;	/* Ack intr. to Remote DMA */
}

void ei_receive(void) reentrant /* maybe reentrant... */
{
	word pkt_len, current_offset;

	byte rx_pkt=0;
	byte rxing_page, this_frame, next_frame;
	
	while ( ++rx_pkt < 10) { 
		/* Get the Receive Page, CURR */
		EN_CMD = EN_NODMA + EN_PAGE1 + EN_START;  
		rxing_page = EN1_CURPAG;
		EN_CMD = EN_NODMA + EN_PAGE0 + EN_START;
		
		/* Remove one frame from the ring.  Boundary is always a page behind. */
		this_frame = EN0_BOUNDARY + 1;
		
		if (this_frame >= RX_STOP_PG)
			this_frame = RX_START_PG;
		
		if (this_frame == rxing_page)	/* Read all the frames? */
			break;				/* Done for now */
		
		current_offset = (word)(this_frame << 8);
		
		/* Get the header of this packet */
		ethernet_get_8390_hdr( current_offset, 4);
		pkt_len = (word)(ethernet_8390_hdr[3]<<8) + ethernet_8390_hdr[2] - 4;

		next_frame = this_frame + 1 + ((pkt_len+4)>>8);
		//print(" | this : ") ; putb_ser( this_frame ) ;
		//print(" | next : ") ; putb_ser( next_frame ) ;
		//print(" | 8390_hdr[1] : ") ; putb_ser( ethernet_8390_hdr[1] ) ;
		//print("\r\n");
	
		if ( ethernet_8390_hdr[1] != next_frame 
			&& ethernet_8390_hdr[1] != next_frame + 1
			&& ethernet_8390_hdr[1] != next_frame - (RX_STOP_PG-RX_START_PG) 
			&& ethernet_8390_hdr[1] != next_frame + 1 - (RX_STOP_PG-RX_START_PG) ) {
			current_page = rxing_page;
			EN0_BOUNDARY = current_page-1;
			continue;
		}

		if ( pkt_len > MAX_PACKET_SIZE || pkt_len < MIN_PACKET_SIZE ) {
			//print("\n\rBogus packet size..");
		}
		else if ((ethernet_8390_hdr[0] & 0x0f) == ENRSR_RXOK) {
			//print("\r\nhdr len =");
			//putb_ser(ethernet_8390_hdr[3]);
			//putb_ser(ethernet_8390_hdr[2]);
			
			/* If RxBuffer is full, then break */
			if ( EthRxBufWrPtr == EthRxBufRdPtr ) 
				break;
			ei_input( EthRxBuf[EthRxBufWrPtr], current_offset + 4, pkt_len );
			EthRxBufWrPtr++;
			if ( EthRxBufWrPtr == NBUF ) EthRxBufWrPtr = 0;
		}
		
		next_frame = ethernet_8390_hdr[1];
		
		/* This _should_ never happen: it's here for avoiding bad clones. */
		if (next_frame >= RX_STOP_PG) { /* next frame inconsistency */
			next_frame = RX_START_PG;
		}
		current_page = next_frame;
		EN0_BOUNDARY = next_frame-1;
	}

	/* We used to also ack ENISR_OVER here, but that would sometimes mask
    a real overrun, leaving the 8390 in a stopped state with rec'vr off. */
    EN0_ISR = ENISR_RX+ENISR_RX_ERR;
}

//void ei_input(byte *buf, word StartAddr, word Count)
void ei_input(byte *buf, word StartAddr, word Count) reentrant
{
    word loop;
        
    EN_CMD = EN_PAGE0 + EN_RREAD + EN_START;
    /* Set Remote byte count */
    EN0_RCNTLO = (byte)(Count & 0xff);	 /*  Low  byte of tx byte count  */
    EN0_RCNTHI = (byte)(Count >> 8);	/*  High byte of tx byte count  Transmit byte count register */

    /* Set Remote Start Address */
    EN0_RSARLO = (byte)(StartAddr & 0xff);     /*LSB Remote start address reg  */
    EN0_RSARHI = (byte)(StartAddr >> 8);     /*  MSB Remote start address reg  */
		
    EN_CMD = EN_PAGE0 + EN_RREAD + EN_START; /* Remote Read, Start the chip, clear reset */
    for(loop=0;loop < Count;loop++){
        *buf++ = EN_DATA; 
    }
    EN0_ISR = ENISR_RDC;	/* Ack intr. to Remote DMA */
}

//void ethernet_get_8390_hdr( word StartAddr, word Count)
void ethernet_get_8390_hdr( word StartAddr, word Count) reentrant
{
    word loop;
        
    EN_CMD = EN_PAGE0 + EN_RREAD + EN_START;
    /* Set Remote byte count */
    EN0_RCNTLO = (byte)(Count & 0xff);	 /*  Low  byte of tx byte count  */
    EN0_RCNTHI = (byte)(Count >> 8);	/*  High byte of tx byte count  Transmit byte count register */

    /* Set Remote Start Address */
    EN0_RSARLO = (byte)(StartAddr & 0xff);     /*LSB Remote start address reg  */
    EN0_RSARHI = (byte)(StartAddr >> 8);     /*  MSB Remote start address reg  */
		
    EN_CMD = EN_PAGE0 + EN_RREAD + EN_START; /* Remote Read, Start the chip, clear reset */
    for(loop=0;loop < Count;loop++){
        ethernet_8390_hdr[loop] = EN_DATA;
    }
    EN0_ISR = ENISR_RDC;	/* Ack intr. to Remote DMA */
}

void ei_rx_overrun( void )
{
	byte was_txing, must_resend = 0;

	//print_int("OVER");
	/* Record whether a Tx was in progress 
	   and then issue the stop command.	*/
	
	was_txing = EN_CMD & EN_TRANS;
	EN_CMD = EN_NODMA + EN_PAGE0 + EN_STOP;
	
	delay_1ms(2);   /* I don't know exactly time yet.. at least 2ms */

	/** Reset RBCR[01] back to zero as per magic incantation. */
	EN0_RCNTLO = 0x00;
	EN0_RCNTHI = 0x00;

	/* See if any Tx was interrupted or not. According to NS, this
	   step is vital, and skipping it will cause no end of havoc. */
	if (was_txing) { 
		if (!(EN0_ISR & (ENISR_TX + ENISR_TX_ERR))) must_resend = 1;
   	}

	/* Have to enter loopback mode and then restart the NIC before
	  you are allowed to slurp packets up off the ring.	 */
	EN0_TXCR = E8390_TXOFF;
	EN_CMD = EN_NODMA + EN_PAGE0 + EN_START;

	/* Clear the Rx ring of all the debris(파편), and ack the interrupt. 	*/
	ei_receive();
	EN0_ISR = ENISR_OVER;
	
	/* Leave loopback mode, and resend any packet that got stopped.	 */
	EN0_TXCR = E8390_TXCONFIG;
	if (must_resend) EN_CMD = EN_NODMA + EN_PAGE0 + EN_START + EN_TRANS;
	
}
    
void ethernet_register_test(void)   /* All Register test */
{
    byte i,read,write;
    
    print("\n\r------------------------------------------------------------------------");
    print("\n\rAddr       Page0         Page1         Page2         Page3");
    print("\n\r         Write/Read   Write/Read     Write/Read    Write/Read");
    print("\n\r------------------------------------------------------------------------");                    
    for (i=0; i < 16; i++) {
         /* Page0 select  */
         EN_CMD = EN_PAGE0 + EN_NODMA + EN_START;
         print("\n\r"); putb_ser(i);print("       ");         
         write = EN_PAGE0 + EN_NODMA + EN_START; 
      /*   outportb(i,write);  */
         putb_ser(write);  
         print("    "); read = inportb(i);
         putb_ser(read); print("     ");

         /* Page1 select */
         EN_CMD = EN_PAGE1 + EN_NODMA + EN_START;
         write = EN_PAGE1 + EN_NODMA + EN_START;
         /* outportb(i,write);  */
         putb_ser(write); print("    ");
         read = inportb(i);
         putb_ser(read); print("       ");

         /* Page2 select */
         EN_CMD = EN_PAGE2 + EN_NODMA + EN_START;
         print("x     ");
         read = inportb(i);
         putb_ser(read); print("      ");

         /* Page3 select */
         EN_CMD = EN_PAGE3 + EN_NODMA + EN_START;
         write = EN_PAGE3 + EN_NODMA + EN_START;
    /*     outportb(i,write);  */
         putb_ser(write);  
         print("    ");
         read = inportb(i);
         putb_ser(read); print("      ");
    }
    print("\n\r------------------------------------------------------------------------\n\r");
}

//*****************************************************************************/
//*		D M A   Write                                                                                                                */
//*****************************************************************************/
void DMA_write(byte *buffer, word StartAddr, word Count)
{
    word loop;
    
    //print("\n\rDMA write : "); 
    EN_CMD = EN_PAGE0 + EN_NODMA + EN_START; /* Remote DMA, Start the chip, clear reset */
    
    EN0_RCNTLO = (byte)(Count & 0xff);  /*  LSB Remote byte count reg  */
    EN0_RCNTHI = (byte)(Count >> 8);  /*  MSB Remote byte count reg  */

    EN0_RSARLO = (byte)(StartAddr & 0xff);   /*  LSB Remote start address reg  */
    EN0_RSARHI = (byte)(StartAddr >> 8);     /*  MSB Remote start address reg  */
    
    EN_CMD = EN_PAGE0 + EN_RWRITE + EN_START; /* Remote Write, Start the chip, clear reset */
    for(loop=0;loop < Count;loop++){
        EN_DATA = *buffer++;
        //putb_ser(*buffer++);       
    }
      
}


/*****************************************************************************/
/*		D M A   Read                                                         */
/*****************************************************************************/
void ethernet_test(void)
{
    byte    temp1,temp2;
    
    print("\n\n\r/*****************************************************************/\n\r");
	print("/*   RTL8019AS TEST ROUTINE     0.5.2                            */\n\r");
	print("/*   If you see 0x50 and 0x70 or 'P' and 'p', it operate well    */\n\r");
    print("/*****************************************************************/\n\r");
       
    temp1 = EN0_8019ID0;  /* EN0_8019ID0 */
    temp2 = EN0_8019ID1;  /* EN0_8019ID1 */
    putb_ser(temp1);
    putb_ser(temp2);

    if ((temp1 != 0x50)|(temp2 != 0x70)) {
        print("       Circuit have H/W problem.\n\r");
        
    }
    print("      Circuit OK!\n\r");

    
#ifdef STRING_TEST
    test = 0x1234;
    printf("\n\n\r%x         %d  ",test,test);
    putb_ser(test);
    test1 = (byte)test;
    printf("\n\n\r%x         %d  ",test1,test1);
#endif

}

/*****************************************************************************/
/*		              RTL8019AS  Initialize                                  */
/*****************************************************************************/
void ethernet_init(void)
{
    word i;

#ifdef RTL8019AS_ID_CHECK
    ethernet_test();
#endif
    
    EN_RESET = 0x00;        /* very important !!!!!    */
    for (i=0;i<DELAY;i++);

    EN_CMD = EN_PAGE0 + EN_NODMA + EN_STOP; /* 00001010B: PS1 PS0 RD2 RD1 RD0 TXP STA STP */
    EN0_DCFG = ENDCFG_FT10 + ENDCFG_BMS + ENDCFG_BOS;   /*  ?FIFO treshold 8byte !!,normal operation, byte transfer mode selection */

    /* Clear the remote	byte count registers. */
    EN0_RCNTHI = 0x00; /* MSB remote byte count reg */
    EN0_RCNTLO = 0x00; /* LSB remote byte count reg */

    EN0_RXCR = E8390_RXOFF; /* RX configuration reg    Monitor mode (no packet receive) */
    EN0_TXCR = E8390_TXOFF; /* TX configuration reg   set internal loopback mode  */
    
    EN0_TPSR = NE_START_PG;
    EN0_STARTPG = RX_START_PG ; /* 패킷 수신시에 로컬 DMA가 수신하는 페이지를 지정한다. 46h */ 
                                /* Starting page of ring buffer. First page of Rx ring buffer 46h*/
    EN0_BOUNDARY = RX_START_PG;	/* Boundary page of ring buffer */
    EN0_STOPPG = NE_STOP_PG;    /* Ending page +1 of ring buffer */
    
    EN_CMD = EN_PAGE1 + EN_NODMA + EN_STOP;
    EN1_CURR = RX_START_PG + 1; /* RX_CURR_PG; Current memory page = RX_CURR_PG  ? */
	current_page = RX_START_PG + 1;	/* assert boundary+1 */
    next_pkt = RX_START_PG + 1; 
    EN_CMD = EN_PAGE0 + EN_NODMA + EN_STOP;
    EN0_ISR = 0xff; /* INTerrupt stauts reg */
      /* Host는 인터럽트를 일으키는 것을 찾아내기위해 읽는다. 
      각 비트는 관련된 bit에 1을 써 넣음으로서 클리어 된다. 
      그것은 파워업 후에 반드시 클리어 되어야한다.(must)     */
    EN0_IMR = 0; /* INTerrupt mask reg = Disable all Interrupt */
    
    EN_CMD = EN_NODMA + EN_PAGE1 +  EN_STOP; /* Select page 1, remote DMA */
       
    EN1_PAR0 = 0x12;
    EN1_PAR1 = 0x34;
    EN1_PAR2 = 0x56;
    EN1_PAR3 = 0x78;
    EN1_PAR4 = 0x9A;
    EN1_PAR5 = 0xBC;
    /* Initialize the multicast list to accept-all.  If we enable multicast
	   the higher levels can do the filtering. multicast filter mask array (8 bytes) */
    EN1_MAR0 = 0xff;  
    EN1_MAR1 = 0xff;
    EN1_MAR2 = 0xff;
    EN1_MAR3 = 0xff;
    EN1_MAR4 = 0xff;
    EN1_MAR5 = 0xff;
    EN1_MAR6 = 0xff;
    EN1_MAR7 = 0xff;
    
    EN_CMD = EN_PAGE0 + EN_NODMA + EN_STOP;  /* 00001010B: PS1 PS0 RD2 RD1 RD0 TXP STA STP */
 
    EN0_ISR = 0xff; 	/* Individual bits are cleared by writing a "1" into it. */
    					/* It must be cleared after power up. */
    EN0_IMR = ENISR_ALL; /* INTerrupt mask reg */

    EN_CMD = EN_PAGE0 + EN_NODMA + EN_START;
    EN0_TXCR = ENTXCR_TXCONFIG;      /* xmit on. */
    /* 3c503 TechMan says rxconfig only after the NIC is started. */
    EN0_RXCR = ENRXCR_RXCONFIG;      /* rx on(broadcasts, no multicast,errors */
	
	CONFIG1 = 0x90;   /* IRQ enable + INT0:IRQ2/9(pin 4) + I/O BASE(300H) */
	CONFIG2 = 0x00;
	CONFIG3 = 0xf0;
	CONFIG4 = 0x00;
	
	EthRxBufRdPtr = 0;
	EthRxBufWrPtr = EthRxBufRdPtr + 1;
	
	EN_CMD = EN_PAGE0 + EN_NODMA + EN_START;
}