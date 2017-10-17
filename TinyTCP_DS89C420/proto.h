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

//		tinytcp application
extern tiny_tcp_main(void);
extern void ethernet_address(void);

// 		monitor program
extern byte ascii_to_hex(byte ch);
extern word address_ascii_to_hex(void);
extern word address_ascii_to_hex_echo(void);
extern void clear_external_ram(void);
extern void delay(word d);
extern void delay_1ms(int times);
extern void dump_memory(void);
extern byte get_serial(void);
extern byte get_serial_echo(void);
extern void help_message(void);
extern byte hex_to_ascii( byte hex);
extern void input_port1(void);
extern void loading_program(void);
extern byte mask_a_nibble ( byte hhex_lhex, byte HIGH_or_LOW );
extern void output_port0(void);
extern void output_port1(void);
extern void output_port2(void);
extern void output_port3(void);
extern void print(byte *ch);
extern void print_int(const byte *ch);
extern void print_logo(void);
extern void putb_ser(byte byte_data);
extern void quit_program(void);
extern void reset (void);
extern void system_init(void);
extern byte two_ascii_to_hex(void);
extern byte two_ascii_to_hex_echo(void);





// RTL8019 Driver

extern void ethernet_init(void);
extern void ethernet_test(void);
extern void DMA_read(word StartAddr, word Count);
extern void DMA_write(byte *buffer, word StartAddr, word Count);
extern void ei_receive(void) reentrant;

//void ei_input(byte *buf, word StartAddr, word Count);
void ei_input(byte *buf, word StartAddr, word Count) reentrant;
extern void ethernet_main(void);
extern void ethernet_register_test(void);
extern void ethernet_packet_tx_test(void);
extern void sed_reset(void);
extern void receive_test(void);
extern void ei_rx_overrun(void);
//extern void ethernet_get_8390_hdr(word StartAddr, word Count);
extern void ethernet_get_8390_hdr(word StartAddr, word Count) reentrant;
extern void DoS(void);