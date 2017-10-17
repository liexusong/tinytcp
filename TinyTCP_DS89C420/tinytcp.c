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
 * tinytcp.c - Tiny Implementation of the Transmission Control Protocol
 *
 * Written March 28, 1986 by Geoffrey Cooper, IMAGEN Corporation.
 *
 * This code is a small implementation of the TCP and IP protocols, suitable
 * for burning into ROM.  The implementation is bare-bones and represents
 * two days' coding efforts.  A timer and an ethernet board are assumed.  The
 * implementation is based on busy-waiting, but the tcp_handler procedure
 * could easily be integrated into an interrupt driven scheme.
 *
 * IP routing is accomplished on active opens by broadcasting the tcp SYN
 * packet when ARP mapping fails.  If anyone answers, the ethernet address
 * used is saved for future use.  This also allows IP routing on incoming
 * connections.
 * 
 * The TCP does not implement urgent pointers (easy to add), and discards
 * segments that are received out of order.  It ignores the received window
 * and always offers a fixed window size on input (i.e., it is not flow
 * controlled).
 *
 * Special care is taken to access the ethernet buffers only in word
 * mode.  This is to support boards that only allow word accesses.
 *
 * Copyright (C) 1986, IMAGEN Corporation
 *  "This code may be duplicated in whole or in part provided that [1] there
 *   is no commercial gain involved in the duplication, and [2] that this
 *   copyright notice is preserved on all copies.  Any other duplication
 *   requires written notice of the author (Geoffrey H. Cooper)."
 */

#include "tinytcp.h"
#include "include.h"

#define WARNING
/*  Local IP address  */
//in_HwAddress sin_lclINAddr;
/* IP identification numbers */

WORD tcp_id;
tcp_Socket *tcp_allsocs;

		extern void putb_ser(byte byte_data);
		
/* Timer definitions */
#define tcp_RETRANSMITTIME 100     /* interval at which retransmitter is called */
#define tcp_LONGTIMEOUT 3100       /* timeout for opens */
#define tcp_TIMEOUT 1000           /* timeout during a connection */

#ifdef DEBUG
/** Primitive logging facility **/
#define tcp_LOGPACKETS 1        /* log packet headers */
WORD tcp_logState;
#endif
void tcp_ProcessData(tcp_Socket *s, tcp_Header *tp, WORD len);
int checksum(WORD *dp, WORD length);
void tcp_Send(tcp_Socket *s);
void tcp_Unthread(tcp_Socket *ds);
void tcp_Handler( in_Header *ip );
void tcp_Flush(tcp_Socket *s);
void tcp_Abort(tcp_Socket *s);
void tcp_Retransmitter(void);
void tcp_Reset( in_Header *ip, tcp_Header *tp );

in_Header *ip_tcp;
DWORD timeout_tcp, start_tcp;
DWORD x_tcp;


//extern WORD i_test;		//	from test_application
//extern BYTE tbuf_test[80];

extern WORD i_hand;

extern BYTE *sed_IsPacket(void);
extern BYTE *sed_FormatPacket( BYTE *destEAddr, WORD ethType );
extern BYTE sed_Send( WORD pkLengthInOctets );
extern sar_MapIn2Eth(DWORD ina, eth_HwAddress *ethap );
extern BYTE sed_Receive( BYTE *buf );
extern BYTE sed_CheckPacket( BYTE *BufLocation, WORD expectedType );
extern void sar_CheckPacket(arp_Header *ap);
extern WORD test_dataHandler( tcp_Socket *s, BYTE *dp, WORD len );
extern WORD test_application( void );
extern void print(BYTE *ch);

/************** Initialize the tcp implementation   ********************************/

void tcp_Init(void)
{
	//extern eth_HwAddress sed_lclEthAddr;

	/* initialize ethernet interface */
	ethernet_init();
	print("Ethernet initialize..\r\n");
	//sed_Init();

	tcp_allsocs = NIL;
	tcp_id = 0;

	/* hack - assume the network number */
	sin_lclINAddr = LOCAL_IP_ADDR;
	
}

/*
 * Actively open a TCP connection to a particular destination.
 */
#ifndef WARNING
//void tcp_Open(tcp_Socket *s, WORD lport, in_HwAddress ina, WORD port, procref datahandler)
void tcp_Open(tcp_Socket *s, WORD lport, in_HwAddress ina, WORD port,procref datahandler) 
{
	//extern eth_HwAddress sed_ethBcastAddr;

	s->state = tcp_StateSYNSENT;
	s->timeout = tcp_LONGTIMEOUT;
	if ( lport == 0 ) lport = clock_ValueRough();
	s->myport = lport;
	if ( ! sar_MapIn2Eth(ina, &s->hisethaddr[0]) ) {
#ifdef DEBUG
		print(" : defaulting ethernet address to broadcast\r\n\r");
#endif
		Move(&sed_ethBcastAddr[0], &s->hisethaddr[0], sizeof(eth_HwAddress));
	}
	s->hisaddr = ina;
	s->hisport = port;
	s->seqnum = 0;
	s->dataSize = 0;
	s->flags = tcp_FlagSYN;
	s->unhappy = true;
	s->dataHandler = datahandler;
	s->next = tcp_allsocs;
	tcp_allsocs = s;
	tcp_Send(s);
}
#endif

/*
 * Passive open: listen for a connection on a particular port
 */
//void tcp_Listen(tcp_Socket *s, WORD port, procref datahandler, DWORD timeout)
void tcp_Listen(tcp_Socket *s, WORD port, DWORD timeout)
{
	s->state = tcp_StateLISTEN;
	if ( timeout == 0 ) s->timeout = 0x7ffffff; /* forever... */
	else s->timeout = timeout;
	s->myport = port;
	s->hisport = 0;
	s->seqnum = 0;
	s->dataSize = 0;
	s->flags = 0;
	s->unhappy = 0;
	//s->dataHandler = datahandler;
//	s->dataHandler = (void *)test_dataHandler( (tcp_Socket *)s, (BYTE *)0, (WORD)0);
	
	s->next = tcp_allsocs;
	tcp_allsocs = s;
}

/*
 * Send a FIN on a particular port -- only works if it is open
 */

#ifndef WARNING 
void tcp_Close(tcp_Socket *s)
{
	if ( s->state == tcp_StateESTAB || s->state == tcp_StateSYNREC ) {
		s->flags = tcp_FlagACK | tcp_FlagFIN;
		s->state = tcp_StateFINWT1;
		s->unhappy = true;
		tcp_Send(s);
	}
}
#endif
/*
 * Abort a tcp connection
 */

void tcp_Abort(tcp_Socket *s)
{
	if ( s->state != tcp_StateLISTEN && s->state != tcp_StateCLOSED ) {
		s->flags = tcp_FlagRST | tcp_FlagACK;
		tcp_Send(s);
	}
	s->unhappy = 0;
	s->dataSize = 0;
	s->state = tcp_StateCLOSED;
//	s->dataHandler((tcp_Socket*)s,(BYTE *)0, (WORD *)-1); 
	DATAHANDLER( (tcp_Socket *)s,(BYTE *)0, (WORD *)0 ) ;

//	s->dataHandler(0,0,0); 
	tcp_Unthread(s);
}

/*
 * Retransmitter - called periodically to perform tcp retransmissions
 */
void tcp_Retransmitter(void)
{
	tcp_Socket *s;
	BOOL x;

	for ( s = tcp_allsocs; s; s = s->next ) {
		x = false;
		if ( s->dataSize > 0 || s->unhappy ) {	/* if we didn't received ack( dataSize > 0 ) or unhappy, then re-xmit it */
			tcp_Send(s);
			x = true;
		}
		if ( x || s->state != tcp_StateESTAB )
			s->timeout -= tcp_RETRANSMITTIME;
		if ( s->timeout <= 0 ) {
			if ( s->state == tcp_StateTIMEWT ) {
#ifdef DEBUG
				print("Closed.    \r\n");
#endif
				s->state = tcp_StateCLOSED;
//				s->dataHandler((tcp_Socket *)s,(BYTE *)0, (WORD *)0);
				DATAHANDLER((tcp_Socket *)s,(BYTE *)0, 0);
				tcp_Unthread(s);
			} else {
#ifdef DEBUG
				print(" : Timeout, aborting\r\n");
#endif
				tcp_Abort(s);
			}
		}
	}
}

/*
 * Unthread a socket from the socket list, if it's there 
 */
void tcp_Unthread(tcp_Socket *ds)
{
	tcp_Socket *s, **sp;

	sp = &tcp_allsocs;
	for (;;) {
		s = *sp;
		if ( s == ds ) {
			*sp = s->next;
			break;
		}
		if ( s == NIL ) break;
		sp = &s->next;
	}
}

/*
 * busy-wait loop for tcp.  Also calls an "application proc"
 */
//void tcp(procref application)
void tcp(void) reentrant
{
	unsigned char i ;
	char *ptr ;
	
	//in_Header *ip;
	//DWORD timeout, start;
	//DWORD x;

	sed_Receive(0);

	timeout_tcp = clock_ValueRough() + tcp_RETRANSMITTIME;
	while ( tcp_allsocs ) {
		start_tcp = clock_ValueRough();
		ip_tcp = (in_Header *)sed_IsPacket();

		/* Modified by junku 
		 * Retransmitter is on this 
		 */
		if ( ip_tcp == NIL ) {
			if ( clock_ValueRough() > timeout_tcp ) {
				tcp_Retransmitter();
				timeout_tcp = clock_ValueRough() + tcp_RETRANSMITTIME;
			}

			//application();
			//print("before test_application\n\r");
			test_application();
			//print("after test_application\n\r");
			continue;
		}
		
		if ( sed_CheckPacket(ip_tcp, 0x806) == 1 ) {
#ifdef DEBUG
			/* do arp */
			print("arp packet") ;
			for ( i=0,ptr=(char *)ip_tcp ; i<sizeof( arp_Header ); i++) {
				putb_ser( *ptr++ ) ;
			}
			print("\r\n") ;
#endif
			sar_CheckPacket(ip_tcp);
		} else if ( sed_CheckPacket(ip_tcp, 0x800) == 1 ) {
#ifdef	DEBUG		/* do IP */
			print("ip packet : ") ;
			for ( i=0,ptr=(char *)ip_tcp ; i<sizeof( in_Header ); i++) {
				putb_ser( *ptr++ ) ;
				print("#");
			}
			print("\r\n") ;
#endif
			if ( ip_tcp->destination == sin_lclINAddr && in_GetProtocol(ip_tcp) == 6 
				&& checksum((WORD *)ip_tcp, in_GetHdrlenBytes(ip_tcp)) == 0xFFFF )
			{
					//print("it's our ip\r\n") ;
					tcp_Handler(ip_tcp);
					//print("\n\r1");
			} 
		}
	        /* recycle buffer */
        	sed_Receive(ip_tcp);
			//print("\n\r2");
        	x_tcp = clock_ValueRough() - start_tcp;
        	timeout_tcp -= x_tcp;
        }
	return;
}

/*
 * Write data to a connection.
 * Returns number of bytes written, == 0 when connection is not in
 * established state.
 */
//void tcp_Write( tcp_Socket *s, BYTE *dp, WORD len )
WORD tcp_Write( tcp_Socket *s, BYTE *dp, WORD len )
{
	WORD x;

	if ( s->state != tcp_StateESTAB ) len = 0;
	if ( len > (x = tcp_MaxData - s->dataSize) ) len = x;
	if ( len > 0 ) {
		Move(dp, &s->datas[s->dataSize], len);
		s->dataSize += len;
		tcp_Flush(s);
	}

	return ( len );
}

/*
 * Send pending data
 */
void tcp_Flush(tcp_Socket *s)
{
    if ( s->dataSize > 0 ) {
        s->flags |= tcp_FlagPUSH;
        tcp_Send(s);
    }
}

/*
 * Handler for incoming packets.
 */
void tcp_Handler( in_Header *ip )
{
	tcp_Header *tp;
	tcp_PseudoHeader ph;
	WORD len;
	BYTE *dp;
	WORD x, diff;
	tcp_Socket *s;
	WORD flags;

	len = in_GetHdrlenBytes(ip);
	tp = (tcp_Header *)((BYTE *)ip + len);
	len = ip->length - len;

	/* demux to active sockets */
	for ( s = tcp_allsocs; s; s = s->next )
		if ( s->hisport != 0 &&
		tp->dstPort == s->myport &&
		tp->srcPort == s->hisport &&
		ip->source == s->hisaddr ) break;
	if ( s == NIL ) {
		/* demux to passive sockets */
		for ( s = tcp_allsocs; s; s = s->next )
		if ( s->hisport == 0 && tp->dstPort == s->myport ) break;
	}
	if ( s == NIL ) {
		tcp_Reset( ip, tp );
#ifdef DEBUG
		if ( tcp_logState & tcp_LOGPACKETS ) tcp_DumpHeader(ip, tp, "Discarding");
#endif
		return;
	}

#ifdef DEBUG
	if ( tcp_logState & tcp_LOGPACKETS )
		tcp_DumpHeader(ip, tp, "Received");
#endif

	/* save his ethernet address */
	Move(&((((eth_Header *)ip) - 1)->source[0]), &s->hisethaddr[0], sizeof(eth_HwAddress));

	ph.src = ip->source;
	ph.dst = ip->destination;
	ph.mbz = 0;
	ph.protocol = 6;
	ph.length = len;
	ph.checksum = checksum((WORD *)tp, len);
	if ( checksum((WORD *)&ph, sizeof ph) != 0xFFFF )
	{
#ifdef DEBUG
		print("bad tcp checksum, received anyway\r\n");
#endif
	}

	flags = tp->flags;
	if ( flags & tcp_FlagRST ) {
#ifdef DEBUG
		print("connection reset\r\n");
#endif
		s->state = tcp_StateCLOSED;
//		s->dataHandler((tcp_Socket *)s,(BYTE*) 0, (WORD *)-1);
		DATAHANDLER((tcp_Socket *)s,(BYTE*) 0,-1);
		tcp_Unthread(s);
		return;
	}

	switch ( s->state ) {
		case tcp_StateLISTEN:
			if ( flags & tcp_FlagSYN ) {
				s->acknum = tp->seqnum + 1;
				s->hisport = tp->srcPort;
				s->hisaddr = ip->source;
				s->flags = tcp_FlagSYN | tcp_FlagACK;
				tcp_Send(s);
				s->state = tcp_StateSYNREC;
				s->unhappy = true;
				s->timeout = tcp_TIMEOUT;
#ifdef DEBUG
				print("Syn from 0x%x#%d (seq 0x%x)\r\n", s->hisaddr, s->hisport, tp->seqnum);
#endif
	        	}
			break;

		case tcp_StateSYNSENT:
			if ( flags & tcp_FlagSYN ) {
				s->acknum++;
				s->flags = tcp_FlagACK;
				s->timeout = tcp_TIMEOUT;
				if ( (flags & tcp_FlagACK) && tp->acknum == (s->seqnum + 1) ) {
#ifdef DEBUG
					print("Open\r\n");
#endif
					s->state = tcp_StateESTAB;
					s->seqnum++;
					s->acknum = tp->seqnum + 1;
					s->unhappy = false;
				} else {
					s->state = tcp_StateSYNREC;
				}
			}
			break;

		case tcp_StateSYNREC:
			if ( flags & tcp_FlagSYN ) {
				s->flags = tcp_FlagSYN | tcp_FlagACK;
				tcp_Send(s);
				s->timeout = tcp_TIMEOUT;
#ifdef DEBUG
				print(" retransmit of original syn\r\n");
#endif				
			}
			if ( (flags & tcp_FlagACK) && tp->acknum == (s->seqnum + 1) ) {
				s->flags = tcp_FlagACK;
				tcp_Send(s);
				s->seqnum++;
				s->unhappy = false;
            			s->state = tcp_StateESTAB;
				s->timeout = tcp_TIMEOUT;
#ifdef DEBUG
				print("Synack received - connection established\r\n");
#endif
			}
			break;

		case tcp_StateESTAB:
			if ( (flags & tcp_FlagACK) == 0 ) return;
			/* process ack value in packet */
			diff = tp->acknum - s->seqnum;
			if ( diff > 0 ) {
				Move(&s->datas[diff], &s->datas[0], diff);
				s->dataSize -= diff;
				s->seqnum += diff;
			}
			s->flags = tcp_FlagACK;
			tcp_ProcessData(s, tp, len);
			break;

		case tcp_StateFINWT1:
			if ( (flags & tcp_FlagACK) == 0 ) return;
			diff = tp->acknum - s->seqnum - 1;
			s->flags = tcp_FlagACK | tcp_FlagFIN;

			/*
			 * This is modified by Jun-Ku.
			 * If we send data and it's unacked. then send FIN. the receiver send diff = dataSize+1 (and it's ACK on FIN ).
			 * if ( diff == 0 ) {
			 */

			if ( diff == 0 || diff == s->dataSize ) {
				s->state = tcp_StateFINWT2;
				s->flags = tcp_FlagACK;
				s->seqnum += (diff+1);
				s->dataSize -= diff;
#ifdef DEBUG
				print("finack received.\r\n");
#endif
			}
			
			tcp_ProcessData(s, tp, len);
			break;

		case tcp_StateFINWT2:
			s->flags = tcp_FlagACK;
			tcp_ProcessData(s, tp, len);
			break;

		case tcp_StateCLOSING:
			if ( tp->acknum == (s->seqnum + 1) ) {
				s->state = tcp_StateTIMEWT;
				s->timeout = tcp_TIMEOUT;
			}
			break;

		case tcp_StateLASTACK:
			if ( tp->acknum == (s->seqnum + 1) ) {
				s->state = tcp_StateCLOSED;
				s->unhappy = false;
				s->dataSize = 0;
//				s->dataHandler((tcp_Socket *)s,(BYTE *)0, (WORD *)0);
				DATAHANDLER((tcp_Socket *)s,(BYTE *)0,0);
				tcp_Unthread(s);
#ifdef DEBUG
				print("Closed.    \r\n");
#endif
			} else {
				s->flags = tcp_FlagACK | tcp_FlagFIN;
				tcp_Send(s);
				s->timeout = tcp_TIMEOUT;
#ifdef DEBUG
				print("retransmitting FIN\r\n");
#endif
			}
			break;

		case tcp_StateTIMEWT:
			s->flags = tcp_FlagACK;
			tcp_Send(s);
	}
}

/*
 * Process the data in an incoming packet.
 * Called from all states where incoming data can be received: established,
 * fin-wait-1, fin-wait-2
 */
void tcp_ProcessData(tcp_Socket *s, tcp_Header *tp, WORD len)
{
	short diff, x;
	WORD flags;
	BYTE *dp;

	flags = tp->flags;
	diff = s->acknum - tp->seqnum;
	if ( flags & tcp_FlagSYN ) diff--;
	x = tcp_GetDataOffset(tp) << 2;		/* x = hdrlen in bytes */
	dp = (BYTE *)tp + x;			/* get first pointer of data */
	len -= x;					/* len is data length now */
	if ( diff >= 0 ) {
		dp += diff;
		len -= diff;
		s->acknum += len;
//		s->dataHandler((tcp_Socket *)s,(BYTE *)dp, (WORD *)len);
		DATAHANDLER((tcp_Socket *)s,(BYTE *)dp,len);
		if ( flags & tcp_FlagFIN ) {
			s->acknum++;
#ifdef DEBUG
			print("consumed fin.\r\n");
#endif
			switch(s->state) {
				case tcp_StateESTAB:
					/* note: skip state CLOSEWT by automatically closing conn */
					x = tcp_StateLASTACK;
					s->flags |= tcp_FlagFIN;
					s->unhappy = true;
#ifdef DEBUG
					print("fin on estab.\r\n");
#endif
					break;
				case tcp_StateFINWT1:
					x = tcp_StateCLOSING;
					break;
				case tcp_StateFINWT2:
					x = tcp_StateTIMEWT;
					break;
			}
			s->state = x;
		}
	}
	s->timeout = tcp_TIMEOUT;
	tcp_Send(s);
}

/*
 * Format and send an outgoing segment
 */
void tcp_Send(tcp_Socket *s)
{
	tcp_PseudoHeader ph;
	struct _pkt {
		in_Header in;
		tcp_Header tcp;
		DWORD maxsegopt;
	} *pkt;
	BYTE *dp;

	pkt = (struct _pkt *)sed_FormatPacket(&s->hisethaddr[0], 0x800);
	dp = (BYTE *)&pkt->maxsegopt;

	pkt->in.length = sizeof(in_Header) + sizeof(tcp_Header) + s->dataSize;

	/* tcp header */
	pkt->tcp.srcPort = s->myport;
	pkt->tcp.dstPort = s->hisport;
	pkt->tcp.seqnum = s->seqnum;
	pkt->tcp.acknum = s->acknum;
	pkt->tcp.window = 1024;
	pkt->tcp.flags = s->flags | 0x5000;		/* Header length = 20bytes ;; no option */
	pkt->tcp.checksum = 0;
	pkt->tcp.urgentPointer = 0;
	if ( s->flags & tcp_FlagSYN ) {
		pkt->tcp.flags += 0x1000;
		pkt->in.length += 4;
		pkt->maxsegopt = 0x02040578; /* 1400 bytes */
		dp += 4;
	}
	Move(s->datas, dp, s->dataSize);		/* copy data to pointer which is next to tcp header */

	/* internet header */
	pkt->in.vht = 0x4500;   /* version 4, hdrlen 5, tos 0 */
	pkt->in.identification = tcp_id++;
	pkt->in.frag = 0;
	pkt->in.ttlProtocol = (250<<8) + 6;
	pkt->in.checksum = 0;
	pkt->in.source = sin_lclINAddr;
	pkt->in.destination = s->hisaddr;
	pkt->in.checksum = ~(checksum((WORD *)&pkt->in, sizeof(in_Header)));

	/* compute tcp checksum */
	ph.src = pkt->in.source;
	ph.dst = pkt->in.destination;
	ph.mbz = 0;
	ph.protocol = 6;
	ph.length = pkt->in.length - sizeof(in_Header);
	ph.checksum = checksum((WORD *)&pkt->tcp, ph.length);
	pkt->tcp.checksum = ~checksum((WORD *)&ph, sizeof ph);

	sed_Send(pkt->in.length);
}

/*
 * Format and send an outgoing reset segment
 */
void tcp_Reset( in_Header *ip, tcp_Header *tp )
{
	tcp_PseudoHeader ph;
	struct _pkt {
		in_Header in;
		tcp_Header tcp;
		DWORD maxsegopt;
	} *pkt;

	pkt = (struct _pkt *)sed_FormatPacket(&(((eth_Header *)ip)-1)->source[0], 0x800);
	pkt->in.length = sizeof(in_Header) + sizeof(tcp_Header);

	/* tcp header */
	pkt->tcp.srcPort = tp->dstPort;
	pkt->tcp.dstPort = tp->srcPort;
	pkt->tcp.seqnum = tp->seqnum;
	pkt->tcp.acknum = tp->acknum;
	pkt->tcp.window = 1024;
	pkt->tcp.flags = tcp_FlagRST | 0x5000;		/* Header length = 20bytes ;; no option */
	pkt->tcp.checksum = 0;
	pkt->tcp.urgentPointer = 0;

	/* internet header */
	pkt->in.vht = 0x4500;   /* version 4, hdrlen 5, tos 0 */
	pkt->in.identification = tcp_id++;
	pkt->in.frag = 0;
	pkt->in.ttlProtocol = (250<<8) + 6;
	pkt->in.checksum = 0;
	pkt->in.source = sin_lclINAddr;
	pkt->in.destination = ip->source;
	pkt->in.checksum = ~(checksum((WORD *)&pkt->in, sizeof(in_Header)));

	/* compute tcp checksum */
	ph.src = pkt->in.source;
	ph.dst = pkt->in.destination;
	ph.mbz = 0;
	ph.protocol = 6;
	ph.length = pkt->in.length - sizeof(in_Header);
	ph.checksum = checksum((WORD *)&pkt->tcp, ph.length);
	pkt->tcp.checksum = ~checksum((WORD *)&ph, sizeof ph);

	sed_Send(pkt->in.length);
}

/*
 * Do a one's complement checksum
 */
int checksum(WORD *dp, WORD length)
{
	WORD len;
	DWORD sum;

	len = length >> 1;
	sum = 0;
	while ( len-- > 0 ) sum += *dp++;
	if ( length & 1 ) sum += (*dp & 0xFF00);
	sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);
	sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);

	return ( sum );
}


/* Dump the tcp protocol header of a packet */
#ifdef DEBUG
void tcp_DumpHeader( in_Header *ip, tcp_Header *tp, BYTE *mesg )
{
	return;
	/*
	tcp_Header *tp = (tcp_Header *)((BYTE *)ip + in_GetHdrlenBytes(ip));
	static char *flags[] = { "FIN", "SYN", "RST", "PUSH", "ACK", "URG" };
	int len;
	WORD f;

	len =  ip->length - ((tcp_GetDataOffset(tp) + in_GetHdrlen(ip)) << 2);
	printf("TCP: %s packet:\r\nS: %x; D: %x; SN=%x ACK=%x W=%d DLen=%d\r\n",
           mesg, tp->srcPort, tp->dstPort, tp->seqnum, tp->acknum,
           tp->window, len);
	printf("DO=%d, C=%x U=%d",
           tcp_GetDataOffset(tp), tp->checksum, tp->urgentPointer);
    f = tp->flags;
    for ( len = 0; len < 6; len++ )
        if ( f & (1 << len) ) printf(" %s", flags[len]);
    printf("\r\n");
   */
}
#endif