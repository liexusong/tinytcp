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
/*			Part of Monitor Program to use RS-232                            */
/*****************************************************************************/
data word buf_addr;
data word buf_addr2;
data byte loading_ready = 0;
data byte dump_auto = 0;
data word dump_address = 0x8000;


/*****************************************************************************/
/*			Part of Ethernet Driver                 	                     */
/*****************************************************************************/

byte current_page;
byte next_pkt;
byte rx_frame_errors;  
byte rx_crc_errors;    
byte rx_missed_errors;
byte EthRxBufRdPtr;
byte EthRxBufWrPtr;
byte ethernet_8390_hdr[4];
dword	pkt_cnt = 0;


/*****************************************************************************/
/*  Description of header of each packet in receive area of memory           */
/*  The 8019 specific per-packet-header format.                              */
/*****************************************************************************/
struct e8390_pkt_hdr {
	byte status; 	/* status of receiver */
	byte next;   	/* pointer to next packet. */
	byte countl;	/* header + packet length in bytes */
	byte counth;
};

/*****************************************************************************/
/*  from tinytcp main                                                        */
/*****************************************************************************/
#include "tinytcp.h"
#define LOCAL_IP_ADDR	0xca1e1eed   /* 202.30.30.237 */
eth_HwAddress sed_lclEthAddr = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
eth_HwAddress sed_ethBcastAddr = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
in_HwAddress  sin_lclINAddr = LOCAL_IP_ADDR;

dword clock_MS = 0;			/* I don't know yet ??*/
