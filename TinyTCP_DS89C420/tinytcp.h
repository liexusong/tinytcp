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
 
/*
 * tinytcp.h - header file for tinytcp.c
 *
 * Copyright (C) 1986, IMAGEN Corporation
 *  "This code may be duplicated in whole or in part provided that [1] there
 *   is no commercial gain involved in the duplication, and [2] that this
 *   copyright notice is preserved on all copies.  Any other duplication
 *   requires written notice of the author (Geoffrey H. Cooper)."
 *
 * Note: the structures herein must guarantee that the
 *       code only performs WORD fetches, since the
 *       imagenether card doesn't accept BYTE accesses.
 */
extern void ethernet_init(void);

#ifndef _global_h
#include "global_T.h"
#endif

#ifndef _tinytcp_h
#define _tinytcp_h

#define TRUE        1
#define true        1
#define FALSE       0
#define false       0
#define NIL         0               /* The distinguished empty pointer */

/* Useful type definitions */
typedef WORD (*procref)(); 

typedef BYTE BOOL;                  /* boolean type */

/* Canonically-sized data */
#ifdef DDT
extern DWORD MsecClock();
#define clock_ValueRough() MsecClock()
#else
extern DWORD clock_ValueRough();
#endif

/* protocol address definitions */
typedef DWORD in_HwAddress;
typedef BYTE eth_HwAddress[6];

/* The Ethernet header */
typedef struct {
	eth_HwAddress   destination;
	eth_HwAddress   source;
	WORD            type;
} eth_Header;

/* The Internet Header: */
typedef struct {
	WORD		vht;    /* version, hdrlen, tos */
	WORD		length;
	WORD		identification;
	WORD		frag;
	WORD		ttlProtocol;
	WORD		checksum;
	in_HwAddress    source;
	in_HwAddress    destination;
} in_Header;

#define in_GetVersion(ip) (((ip)->vht >> 12) & 0xf)
#define in_GetHdrlen(ip)  (((ip)->vht >> 8) & 0xf)
#define in_GetHdrlenBytes(ip)  (((ip)->vht >> 6) & 0x3c)
#define in_GetTos(ip)      ((ip)->vht & 0xff)

#define in_GetTTL(ip)      ((ip)->ttlProtocol >> 8)
#define in_GetProtocol(ip) ((ip)->ttlProtocol & 0xff)

typedef struct {
	WORD		srcPort;
	WORD		dstPort;
	DWORD		seqnum;
	DWORD		acknum;
	WORD		flags;
	WORD		window;
	WORD		checksum;
	WORD		urgentPointer;
} tcp_Header;

#define tcp_FlagFIN	0x0001
#define tcp_FlagSYN	0x0002
#define tcp_FlagRST	0x0004
#define tcp_FlagPUSH	0x0008
#define tcp_FlagACK	0x0010
#define tcp_FlagURG	0x0020
#define tcp_FlagDO	0xF000
#define tcp_GetDataOffset(tp) ((tp)->flags >> 12)

/* The TCP/UDP Pseudo Header */
typedef struct {
	in_HwAddress	src;
	in_HwAddress	dst;
	BYTE		mbz;
	BYTE		protocol;
	WORD		length;
	WORD		checksum;
} tcp_PseudoHeader;

/*
 * TCP states, from tcp manual.
 * Note: close-wait state is bypassed by automatically closing a connection
 *       when a FIN is received.  This is easy to undo.
 */
#define tcp_StateLISTEN  0      /* listening for connection */
#define tcp_StateSYNSENT 1      /* syn sent, active open */
#define tcp_StateSYNREC  2      /* syn received, synack+syn sent. */
#define tcp_StateESTAB   3      /* established */
#define tcp_StateFINWT1  4      /* sent FIN */
#define tcp_StateFINWT2  5      /* sent FIN, received FINACK */
/*#define tcp_StateCLOSEWT 6    /* received FIN waiting for close */
#define tcp_StateCLOSING 6      /* sent FIN, received FIN (waiting for FINACK) */
#define tcp_StateLASTACK 7      /* fin received, finack+fin sent */
#define tcp_StateTIMEWT  8      /* dally after sending final FINACK */
#define tcp_StateCLOSED  9      /* finack received */

/*
 * TCP Socket definition
 */
#define tcp_MaxData 32              /* maximum BYTEs to buffer on output */

typedef struct _tcp_socket {
	struct _tcp_socket	*next;
	BYTE		state;          /* connection state */
	procref		dataHandler;    /* called with incoming data */
	
	eth_HwAddress	hisethaddr;     /* ethernet address of peer */
	in_HwAddress	hisaddr;        /* internet address of peer */
	WORD		myport, hisport;/* tcp ports for this connection */
	DWORD		acknum, seqnum; /* data ack'd and sequence num */
	DWORD		timeout;        /* timeout, in milliseconds */
	BOOL		unhappy;        /* flag, indicates retransmitting segt's */
	WORD		flags;          /* tcp flags WORD for last packet sent */
	WORD		dataSize;       /* number of BYTEs of data to send */
	BYTE		datas[tcp_MaxData]; /* data to send */
} tcp_Socket;

#define	DATAHANDLER(a,b,c)	test_dataHandler(a,b,c)

extern eth_HwAddress sed_lclEthAddr;
extern eth_HwAddress sed_ethBcastAddr;
extern in_HwAddress  sin_lclINAddr;

/*
 * ARP definitions
 */
#define arp_TypeEther  1        /* ARP type of Ethernet address *

/* harp op codes */
#define ARP_REQUEST 1
#define ARP_REPLY   2

/*
 * Arp header
 */
typedef struct {
	WORD		hwType;
	WORD		protType;
	WORD		hwProtAddrLen;  /* hw and prot addr len */
	WORD		opcode;
	eth_HwAddress	srcEthAddr;
	in_HwAddress	srcIPAddr;
	eth_HwAddress	dstEthAddr;
	in_HwAddress	dstIPAddr;
} arp_Header;

/*
 * Ethernet interface:
 *   sed_WaitPacket(0) => ptr to packet (beyond eth header)
 *                          or NIL if no packet ready.
 *   sed_Receive(ptr) - reenables receive on input buffer
 *   sed_FormatPacket(&ethdest, ethtype) => ptr to packet buffer
 *   sed_Send(packet_length) - send the buffer last formatted.
 */
#endif
