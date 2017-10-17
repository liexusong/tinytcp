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

/* a few useful Ethernet definitions. */
#define     BUFSIZ          1600
#define     NBUF            10
#define     MIN_PACKET_SIZE 60      /* smallest legal size packet, no fcs    */
#define     MAX_PACKET_SIZE 1514	/* largest legal size packet, no fcs     */
#define     ETHER_ADDR_LEN	6   /* Etherne;a few useful Ethernet definitions.*/

#define     TX_PAGES	    6       
#define     NE_START_PG	    0x40     	/* First page of TX buffer           */
#define	    TX_START_PG		NE_START_PG
#define     NE_STOP_PG	    0x80		/* Last page + 1 of RX Ring          */ 
#define     RX_STOP_PG      NE_STOP_PG
#define     RX_START_PG	    NE_START_PG + TX_PAGES
#define     RX_CURR_PG      RX_START_PG + 1

#define     EN_RBUF_STAT	0	    /*  Received frame status                */
#define     EN_RBUF_NXT_PG	1	    /*  Page after this frame                */
#define     EN_RBUF_SIZE_LO	2	    /*  Length of this frame                 */
#define     EN_RBUF_SIZE_HI	3	    /*  Length of this frame                 */
#define     SIZE_OF_8019_HDR 4	    /*  size of 8019 specific packet header  */

#define     SBUFSIZ		1600
#define     BUFSIZ		1600
#define     PKT_DEST		0
#define     PKT_SRC		6
#define     PKT_TYPE		12
#define     SIZE_OF_ETH_PKT_HDR	14

/*****************************************************************************/
/* The EN registers - the Realtek RTL8019AS chip registers                   */
/* There are four pages of registers in the chip. You select                 */
/* which page you want, then address them at offsets 00-0F from base.        */
/* The chip command register (EN_CMD) appears in all pages.                  */

#define     EN_CMD          XBYTE [0x300]   /*	The command register (for all pages) */
#define     EN_DATA	    XBYTE [0x310]	/*  Remote DMA Port10~17h (for all pages)*/
#define     EN_RESET	    XBYTE [0x318]	/*  Reset Port 18~1fh(for all pages)     */

/* Register accessed at EN_CMD, the RTL8019 base addr                        */
#define     EN_STOP	    0x01	/*  Stop and reset the chip              */
#define     EN_START	    0x02	/*  Start the chip, clear reset          */
#define     EN_TRANS	    0x04	/*  Transmit a frame                     */
#define     EN_RREAD	    0x08	/*  Remote read                          */
#define     EN_RWRITE	    0x10	/*  Remote write                         */
#define     EN_NODMA	    0x20	/*  Remote DMA                           */
#define     EN_PAGE0	    0x00	/*  Select page chip registers           */
#define     EN_PAGE1	    0x40	/*  using the two high-order bits        */
#define     EN_PAGE2	    0x80 
#define     EN_PAGE3	    0xc0 
#define	    E8390_STOP	    0x01	/* Stop and reset the chip */
#define     E8390_START	    0x02	/* Start the chip, clear reset */
#define     E8390_TRANS	    0x04	/* Transmit a frame */
#define     E8390_RREAD	    0x08	/* Remote read */
#define     E8390_RWRITE    0x10	/* Remote write  */
#define     E8390_SPACKET   0x18	/* Send Packet */
#define     E8390_NODMA	    0x00	/* Remote DMA */
#define     E8390_PAGE0		   0x00	/* Select page chip registers */
#define     E8390_PAGE1		   0x40	/* using the two high-order bits */
#define     E8390_PAGE2		   0x80	/* Page 3 is invalid. */

/* Page 0 register offsets                                                   */
#define     EN0_CLDALO		XBYTE [0x301]   /*  RD Low byte of current local dma addr*/
#define     EN0_STARTPG     XBYTE [0x301]	/*  WR Starting page of ring buffer      */
#define     EN0_CLDAHI		XBYTE [0x302]   /*  RD High byte of current local dma add*/
#define     EN0_STOPPG  	XBYTE [0x302]	/*  WR Ending page +1 of ring buffer     */
#define     EN0_BOUNDARY	XBYTE [0x303]	/*  RD/WR Boundary page of ring buffer   */
#define     EN0_TSR			XBYTE [0x304]	/*  RD Transmit status reg               */
#define     EN0_TPSR		XBYTE [0x304]	/*  WR Transmit starting page            */
#define     EN0_NCR			XBYTE [0x305]	/*  RD Number of collision reg           */
#define     EN0_TCNTLO  	XBYTE [0x305]	/*  WR Low  byte of tx byte count        */
#define     EN0_FIFO		XBYTE [0x306]	/*  RD FIFO                              */
#define     EN0_TCNTHI		XBYTE [0x306]	/*  WR High byte of tx byte count        */
#define     EN0_ISR			XBYTE [0x307]	/*  RD/WR Interrupt status reg           */
#define     EN0_CRDALO  	XBYTE [0x308]	/*  RD low byte of current remote dma add*/
#define     EN0_RSARLO		XBYTE [0x308]	/*  WR Remote start address reg 0        */
#define     EN0_CRDAHI		XBYTE [0x309]	/*  RD high byte, current remote dma add.*/
#define     EN0_RSARHI		XBYTE [0x309]	/*  WR Remote start address reg 1        */
#define     EN0_RCNTLO	    XBYTE [0x30a]	/*  WR Remote byte count reg 0           */
#define     EN0_8019ID0	    XBYTE [0x30a]	/*  Only 8019AS                          */
#define     EN0_RCNTHI		XBYTE [0x30b]	/*  WR Remote byte count reg 1           */
#define     EN0_8019ID1 	XBYTE [0x30b]	/*  Only 8019AS                          */
#define     EN0_RSR			XBYTE [0x30c]	/*  RD RX status reg                     */
#define     EN0_RXCR		XBYTE [0x30c]	/*  WR RX configuration reg              */
#define     EN0_TXCR		XBYTE [0x30d]	/*  WR TX configuration reg              */
#define     EN0_COUNTER0	XBYTE [0x30d]	/*  RD Rcv alignment error counter       */
#define     EN0_DCFG		XBYTE [0x30e]	/*  WR Data configuration reg            */
#define     EN0_COUNTER1	XBYTE [0x30e]	/*  RD Rcv CRC error counter             */
#define     EN0_IMR			XBYTE [0x30f]	/*  WR Interrupt mask reg                */
#define     EN0_COUNTER2	XBYTE [0x30f]	/*  RD Rcv missed frame error counter    */
                                                
/* Bits in EN0_TSR - Transmitted packet status                               */
#define     ENTSR_PTX	    0x01	/*  Packet transmitted without error     */
#define     ENTSR_ND	    0x02	/*  The transmit wasn't deferred.  ?     */
#define     ENTSR_COL	    0x04	/*  The transmit collided at least once. */
											/*  Record of collision remain NCR reg.  */
#define     ENTSR_ABT	    0x08	/*  The transmit collided 16 times, and was deferred. */
#define     ENTSR_CRS	    0x10	/*  The carrier sense was lost.          */
#define     ENTSR_FU	    0x20	/*  A "FIFO underrun" occurred during transmit. */
#define     ENTSR_CDH	    0x40	/*  The collision detect "heartbeat" signal was lost. */
#define     ENTSR_OWC	    0x80	/*  There was an out-of-window collision.*/
/*  After slot time, there was collision. This case was excuted retransmission.  ? */
                                    
              
/* Bits in EN0_ISR - Interrupt status register        (RD WR)                */
#define     ENISR_RX   		0x01	/*  Receiver, no error                   */
#define     ENISR_TX	    0x02	/*  Transceiver, no error                */
#define     ENISR_RX_ERR	0x04	/*  Receiver, with error                 */
#define     ENISR_TX_ERR	0x08	/*  Transmitter, with error              */
#define     ENISR_OVER	    0x10	/*  Receiver overwrote the ring          */
                       /*  Gap area of receiver ring buffer was disappeared  */ 
#define     ENISR_COUNTERS	0x20	/*  Counters need emptying               */
                                    /* MSB of network tally counter became 1 */
#define     ENISR_RDC	    0x40	/*  remote dma complete                  */
#define     ENISR_RESET     0x80	/*  Reset completed                      */
#define     ENISR_ALL	    0x35	/*3f  Interrupts we will enable 에러수정함	 */
                                	/*  RST RDC CNT OVW TXE RXE PTX PRX		 */
/* Bits in EN0_RSR and received packet status byte                           */
#define     ENRSR_RXOK  	0x01	/*  Received a good packet               */
#define     ENRSR_CRC	    0x02	/*  CRC error(CNTR1++)                   */
#define     ENRSR_FAE	    0x04	/*  Frame alignment error(CNTR0++)       */
                                    /*Packet isn't finished in byte boundary.*/
#define     ENRSR_FO	    0x08	/*  FIFO overrun when receiving          */
#define     ENRSR_MPA	    0x10	/*  Missed packet.. Gap area isn't exist */
             /* or because of monitor mode, packet is not buffered (CNTR2++) */
#define     ENRSR_PHY	    0x20	/* Physical/multicase address            */
 /*  수신한 패킷의 수신지 어드레스(destination address)의 종류 
 피지컬 어드레스(PAR0~6)였던 경우에는 1, 멀티 캐스트 어드레스였던 경우는 0   */
#define     ENRSR_DIS	    0x40	/*  Receiver disable.set in monitor mode */
#define     ENRSR_DEF	    0x80	/*  Deferring because of collision       */
                                
/*****************************************************************************/
/* Some generic ethernet register configurations.                            */
#define E8390_TX_IRQ_MASK   0x0A  /* For register EN0_ISR */
#define E8390_RX_IRQ_MASK   0x05                                           */
#define E8390_RXCONFIG      0x04 /* EN0_RXCR: broadcasts, no multicast & errors */
#define E8390_RXOFF         0x20    /* EN0_RXCR: Accept no packets */
#define E8390_TXCONFIG      0x00 /* EN0_TXCR: Normal transmit mode */
#define E8390_TXOFF         0x02    /* EN0_TXCR: Transmitter off */
/*****************************************************************************/

/* Bits in EN0_RXCR - RX configuration reg                                   */
#define     ENRXCR_RXCONFIG 0x04 	/* EN0_RXCR: broadcasts,no multicast,errors */
//#define     ENRXCR_RXCONFIG 0x00 	/* EN0_RXCR: only unicast */
#define     ENRXCR_CRC	    0x01	/*  Save error packets(admit)            */
#define     ENRXCR_RUNT	    0x02	/*  Accept runt pckt(below 64bytes)      */
#define     ENRXCR_BCST	    0x04	/*  Accept broadcasts when 1             */
#define     ENRXCR_MULTI	0x08	/*  Multicast (if pass filter) when 0    */
#define     ENRXCR_PROMP	0x10	/*  Promiscuous physical addresses when 1*/
											/* when 0,accept assigned PAR0~5 address */
#define     ENRXCR_MON	    0x20	/*  Monitor mode (no packets rcvd)       */

/* Bits in EN0_TXCR - TX configuration reg                                   */
#define     ENTXCR_TXCONFIG 0x00    /* Normal transmit mode                  */
#define     ENTXCR_CRC	    0x01	/*  inhibit CRC,do not append crc when 1 */
#define     ENTXCR_LOOP	    0x02	/*  set internal loopback mode     ?     */
#define     ENTXCR_LB01	    0x06	/*  encoded loopback control       ?     */
#define     ENTXCR_ATD	    0x08	/*  auto tx disable                      */
/* when 1, if specified multicast packet was received, disable transmit      */ 
#define     ENTXCR_OFST	    0x10	/*  collision offset enable              */
/* selection of collision algorithm. When 0, gererally back-off algorithm select */
/* 1 이라면 우선 순위가 내려갈 수 있는 백오프 알고리즘을 선택한다.           */         

/* Bank0/Reg0E                                                               */
/* Bits in EN0_DCFG - Data config register                                   */
#define     ENDCFG_WTS	    0x01	/*  byte transfer mode selection         */        
/*#endif    */
#define     ENDCFG_BOS	    0x02	/*  byte order selection                 */
                                    /* 0: MD15-8, LSB on MD7-0;little endian */
                                    /* 1: MSB on MD7-0, LSB on MD15-8     ?  */
#define     ENDCFG_LAS	    0x04	/* long addr selection (must be set to zero) */
                                    /* when 1 is 32bit address mode          */
/* 현재 자동 초기화 모드는 지정되어 있지 않다.. 왜 그럴까? */
#define     ENDCFG_BMS	    0x08	/* loopback select       set is normal  mode      */
/* 0: Loopback mode select. Bits 1 and 2 of the
  EN0_TXCR must also be programmed for Loopback
  1: Normal operation                                                        */
#define     ENDCFG_ARM	    0x10	/*  autoinitialize remote when 1         */
#define     ENDCFG_FT00	    0x00	/*  fifo treshold   2 byte               */
#define     ENDCFG_FT01	    0x20	/*                  4 byte               */  
#define     ENDCFG_FT10	    0x40	/*                  8 byte               */
#define     ENDCFG_FT11	    0x60	/*                 12 byte               */

/* Page 1 register offsets                                                   */
#define     EN1_PAR0	    XBYTE [0x301]	/* RD/WR This board's physical ethernet addr */
#define     EN1_PAR1	    XBYTE [0x302]
#define     EN1_PAR2	    XBYTE [0x303]
#define     EN1_PAR3	    XBYTE [0x304]
#define     EN1_PAR4	    XBYTE [0x305]
#define     EN1_PAR5	    XBYTE [0x306]
#define     EN1_CURR	    XBYTE [0x307]   /* RD/WR current page reg */
#define		EN1_CURPAG		EN1_CURR
#define     EN1_MAR0        XBYTE [0x308]   /* RD/WR Multicast filter mask array (8 bytes) */
#define     EN1_MAR1		XBYTE [0x309]
#define     EN1_MAR2        XBYTE [0x30a]
#define     EN1_MAR3        XBYTE [0x30b]
#define     EN1_MAR4        XBYTE [0x30c]
#define     EN1_MAR5        XBYTE [0x30d]
#define     EN1_MAR6        XBYTE [0x30e]
#define     EN1_MAR7        XBYTE [0x30f]

/* Page 2 register offsets                                                   */
#define     EN2_STARTPG     XBYTE [0x301]   /* RD only Reg.           */
#define     EN2_STOPPG      XBYTE [0x302]
#define     EN2_TPSR		XBYTE [0x304]
#define     EN2_RXCR		XBYTE [0x30c]
#define     EN2_TXCR		XBYTE [0x30d]
#define     EN2_DCFG		XBYTE [0x30e]
#define     EN2_IMR			XBYTE [0x30f]

/* Page 3 register offsets    There is no Reg. in the DP8390.                */
#define     CR9346			XBYTE [0x301]   /* RD/WR There is no Reg. in the DP8390 */
#define     BPAGE			XBYTE [0x302]   /* RD/WR */
#define     CONFIG0			XBYTE [0x303]  /* RD    */
#define     CONFIG1			XBYTE [0x304]   /* RD/WR */
#define     CONFIG2			XBYTE [0x305]   /* RD/WR */
#define     CONFIG3			XBYTE [0x306]   /* RD/WR */
#define     CONFIG4			XBYTE [0x30d]  /* RD       */
#define     CSNSAV          XBYTE [0x308]   /* RD    */
#define     HLTCLK			XBYTE [0x309]   /*    WR */
#define     INTR			XBYTE [0x30B]   /* RD    */