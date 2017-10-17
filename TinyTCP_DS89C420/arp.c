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
 * SAR: Simple Address Resolution Protocol Implementation
 * Written by Geoffrey Cooper, September 27, 1983
 * 
 * This package implements a very simple version of the Plummer Address
 * Resolution Protocol (RFC 826).  It allows clients to resolve Internet
 * addresses into Ethernet addresses, and knows how to respond to an
 * address resolution request (when the transmit buffer is free).
 * 
 * Routines:
 * 
 *  sar_CheckPacket( pb ) => 1, if ARP packet and processed, 0 otherwise
 *  sar_MapIn2Eth( ina, ethap ) => 1 if did it, 0 if couldn't.
 *
 * Copyright (C) 1983, 1986 IMAGEN Corporation
 *  "This code may be duplicated in whole or in part provided that [1] there
 *   is no commercial gain involved in the duplication, and [2] that this
 *   copyright notice is preserved on all copies.  Any other duplication
 *   requires written notice of the author (Geoffrey H. Cooper)."
 * 
 */

#include "tinytcp.h"
#define WARNING

extern BYTE sed_Send( WORD pkLengthInOctets );
extern BYTE sed_Receive( BYTE *buf );
extern void Move( BYTE *src, BYTE *dest, WORD numbytes );
extern BYTE *sed_IsPacket(void);
extern BYTE sed_CheckPacket( BYTE *BufLocation, WORD expectedType );
extern BYTE *sed_FormatPacket( BYTE *destEAddr, WORD ethType );

		extern void putb_ser(byte byte_data);
		
extern void print(byte *ch);

sar_CheckPacket(arp_Header *ap)
{
	arp_Header *op;
	unsigned char i ;
	char *ptr ;

	if ( ap->hwType != arp_TypeEther || /* have ethernet hardware, */
	ap->protType != 0x800 ||       /* and internet software, */
	ap->opcode != ARP_REQUEST ||   /* and be a resolution req. */
	ap->dstIPAddr != sin_lclINAddr ) /* for my addr. */ 
		return ( 0 );                  /* .... or we ignore it. */

	/* format response. */
	op = (arp_Header *)sed_FormatPacket(ap->srcEthAddr, 0x806);
	op->hwType = arp_TypeEther;
	op->protType = 0x800;
	op->hwProtAddrLen = (sizeof(eth_HwAddress) << 8) + sizeof(in_HwAddress);
	op->opcode = ARP_REPLY;
	op->srcIPAddr = sin_lclINAddr;
	Move(sed_lclEthAddr, op->srcEthAddr, sizeof(eth_HwAddress));
	op->dstIPAddr = ap->srcIPAddr;			/* Modified By Junku */
	Move(ap->srcEthAddr, op->dstEthAddr, sizeof(eth_HwAddress));

	sed_Send(sizeof(arp_Header));
    
	return ( 1 );
}

/* 
 * Do an address resolution bit.
 */
#ifndef WARNING
sar_MapIn2Eth(DWORD ina, eth_HwAddress *ethap )
{
	register arp_Header *op;
	extern in_HwAddress sin_lclINAddr;
	//register i;  /* never used */
	DWORD endTime;
	DWORD rxMitTime;

	sed_Receive( 0 );
	endTime = clock_ValueRough() + 2000;		/* 20ms * 2000 = 40 Sec */
	while ( endTime > clock_ValueRough() ) {
		op = (arp_Header *)sed_FormatPacket((BYTE *)&sed_ethBcastAddr[0], 0x0806);
		op->hwType = arp_TypeEther;
		op->protType = 0x800;
		op->hwProtAddrLen = (sizeof(eth_HwAddress) << 8) + sizeof(in_HwAddress);
		op->opcode = ARP_REQUEST;
		op->srcIPAddr = sin_lclINAddr;
		Move(sed_lclEthAddr, op->srcEthAddr, sizeof(eth_HwAddress));
		op->dstIPAddr = ina;

		/* ...and send the packet */
		sed_Send( sizeof(arp_Header) );

		rxMitTime = clock_ValueRough() + 400; /* 20ms * 200 = 4 Sec */
		while ( rxMitTime > clock_ValueRough() ) {
			op = (arp_Header *)sed_IsPacket();
			if ( op ) {
				if ( sed_CheckPacket(op, 0x806) == 1 && 
					op->protType == 0x800 &&
					op->srcIPAddr == ina &&
					op->opcode == ARP_REPLY ) {
					Move(op->srcEthAddr, (BYTE *)ethap, sizeof(eth_HwAddress));
					return ( 1 );
				}
				sed_Receive(op);
			}
		}
	}
	return ( 0 );
}
#endif
