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
#include <stdio.h>
#include <aduc812.h>

#define     XBYTE           ((unsigned char *) 0x20000L)
#define     HIGH            1
#define     LOW             0
#define		BYTE	byte
#define		WORD	word
#define 	write_ram(address,value)		XBYTE [address] = value
#define		read_ram(address)		   		XBYTE [address] 
#define 	inportb(address)		   		XBYTE [address]
#define 	outportb(address,value)			XBYTE [address] = value
#define		outp(address,value)				XBYTE [address] = value
typedef 	unsigned long longword;		/* 32 bits          */
typedef 	unsigned int word;			/* 16 bits          */
typedef 	unsigned char byte;         /*  8 bits          */

extern data word buf_addr;
extern data word buf_addr2;
extern data byte loading_ready;
extern data byte dump_auto;
extern data word dump_address;


byte ascii_to_hex(byte ch);
word address_ascii_to_hex(void);
word address_ascii_to_hex_echo(void);
void clear_external_ram(void);
void delay(word d);
void delay_1ms(int times);
void dump_memory(void);
byte get_serial(void);
byte get_serial_echo(void);
void help_message(void);
byte hex_to_ascii( byte hex);
void input_port1(void);
void loading_program(void);
byte mask_a_nibble ( byte hhex_lhex, byte HIGH_or_LOW );
void output_port0(void);
void output_port1(void);
void output_port2(void);
void output_port3(void);
void print(byte *ch);
void print_int(const byte *ch);
void print_logo(void);
void putb_ser(byte byte_data);
void quit_program(void);
void reset (void);
void system_init(void);
byte two_ascii_to_hex(void);
byte two_ascii_to_hex_echo(void);

/*****************************************************************************/
/*          Monitor & Serial routine for string control                      */
/*****************************************************************************/

byte ascii_to_hex( byte ch )                     /*   ex) 30h = '0'    */
{
	if (ch >= 'A' && ch <= 'F'){
		 return ( ch - 'A' + 10 );
	}
	else if (ch >= 'a' && ch <= 'f'){ 
		return (ch - 'a' + 10);
	}
	else return ( ch - '0');
}

word address_ascii_to_hex(void)
{
    union d {
	word serial_four;
	byte serial_temp[2];
	}s ;

		s.serial_temp[0] = two_ascii_to_hex();    /*  high address 1 byte */
        s.serial_temp[1] = two_ascii_to_hex();    /*  low  address 2 byte */
	return (s.serial_four);
}

word address_ascii_to_hex_echo(void)
{
	union d {
	word serial_four;
	byte serial_temp[2];
	}s ;
	
	s.serial_temp[0] = two_ascii_to_hex_echo();    /* high address 1 byte */
	s.serial_temp[1] = two_ascii_to_hex_echo();    /* low  address 2 byte */
	return (s.serial_four);
}

void delay(word d)
{
	data word i;
	for(i=0;i<d*100;i++);
}

/* --------------------------- 1mS 시간지연 프로그램 ------------------------- */
/* [인수] void								    */
/* [참조] 1mS 시간 지연 						    */
/* ------------------------------------------------------------------------ */
void delay_1ms(int times)
{
	for(times = 0;times < 198;times++);        /* Delay Loop */
}


void clear_external_ram(void)                     /*  clear data memory from 8000h  */
{
	word address,i;
	
	print("lear external RAM from 0000...");
	address = 0xffff;
	
	for(i=0;i<address;i++){
		write_ram(i,0xff);
	}
}
void dump_memory( void )                          /*  dump from xxxx 16line  */
{
	byte num1,num2;

	if (dump_auto != 1){
		print("ump Address at = "); buf_addr = address_ascii_to_hex_echo();
		dump_address = buf_addr;
	}
	else {
		dump_address = dump_address + 0x100;
		buf_addr = dump_address;
		printf("%x",dump_address);
	}
	
	print("\n\r");
	print("------------------------------------------------------------------------ \n\r");
	print("ADDR   +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F    ASCII CODE\n\r");
	print("------------------------------------------------------------------------ \n\r");

	buf_addr = buf_addr & 0xfff0;
	buf_addr2= buf_addr & 0xfff0;
	for( num2=0x10; !(num2 == 0x00); num2--){

		printf("%x",buf_addr);
		print(" : ");

		for( num1 = 0x10; num1 != 0x00; buf_addr++, num1--){
			putb_ser(read_ram(buf_addr));
			print(" ");
		}
		print(" ");
		for( num1 = 0x10; num1 != 0x00; buf_addr2++, num1--){
			if (read_ram(buf_addr2) < 0x20 || read_ram(buf_addr2) > 0x7f){
				print(".");
			}
			else{
				putchar(read_ram(buf_addr2));
			}
		}
		print("\n\r");
	}
	print("------------------------------------------------------------------------ \n");
}

byte get_serial(void)                 /*  one character receive from keyboard  */
{
	while(!RI);
	RI = 0;
	return(SBUF);
}

byte get_serial_echo(void)            /*  one character receive and display it  */
{
	while(!RI);
	RI = 0;

    putchar((char)SBUF);
	return(SBUF);
}

void help_message(void)               /*  manual for this program   */
{
    print("elp message...\n\r");
    print("0 : Output xx to the PORT 0\n\r");
    print("1 : Output xx to the PORT 1\n\r");
    print("2 : Output xx to the PORT 2\n\r");
    print("3 : Output xx to the PORT 3\n\r");
    print("4 : Ethernet Register reading..\n\r");
    
    print("C : Clear external Memory from 8000h\n\r");
    print("D : Dump xxxx\n\r");
    print("H : This screen for help\n\r");
    print("I : Input xx from the PORT 1\n\r");
    print("L : Down loading program (autodetect ':')\n\r");
    print("O : Output xx to the PORT 1\n\r");
    print("Q : GO program from 8000h\n\r");
    print("R : Reset by software\n\r");
    print("S : Show Ethernet & IP address\n\r");
    print("t : tinymain execute..\n\r");
    delay(10); /* only use to prevent warning */
}

void input_port1(void)
{                                          /*  read PORT1 pin level (HIGH/LOW)   */
	byte temp;
	
	print("n port 1 ...  Press RESET to stop Test !\n\r");
	print("\n       Note ! ...  Only detect LOW level condition.\n\r");
        print("\n       PORT  ||  P1.7  P1.6  P1.5  P1.4  P1.3  P1.2  P1.1  P1.0\n\r");
	while(1){
		temp = P1;
		printf("       %x  ||   %d     %d     %d     %d     %d     %d     %d     %d\r",temp,P1_7,P1_6,P1_5,P1_4,P1_3,P1_2,P1_1,P1_0);
	}
}

byte hex_to_ascii( byte hex )         /*  convert hex to ascii 4bit   */
{
	byte ascii_hex_nibble;
	if ( (0x00 <= hex) && (hex <= 0x09) ) {
		ascii_hex_nibble = hex + 0x30;
	} else if ( 0xA <= hex && hex <= 0xF ) {
			ascii_hex_nibble = hex + 0x41 - 10;
        } else ascii_hex_nibble = '#';     /*  just a detect a error but not used   */
	return ascii_hex_nibble;
}

void loading_program(void)                   /*  down_loading intel format file.hex at 8000h */
{
	byte checksum,serial_temp2;
	word count,address;
	if (loading_ready == 1) goto cont1;
	print("Down Loading Program !!\n\r");

cont:;
	if (get_serial() == ':'){

cont1:;
		count = two_ascii_to_hex(); 		 			 /* read serial data number  */
                if (count == 0) goto no_data;            /* detect down ending       */
                address = address_ascii_to_hex();        /* ram start address        */
                two_ascii_to_hex();                      /*       intel dummy code   */
		while (count){
			count -=1;
			serial_temp2 = two_ascii_to_hex();
			write_ram(address,serial_temp2);
			address++;
		}
	serial_temp2 = two_ascii_to_hex();		 /*  read checksum data */
	checksum =+ serial_temp2;
	}
	goto cont;
no_data:;
	print("\nDownloading Complete!\n\r");
	loading_ready = 0;
}

byte mask_a_nibble ( byte hhex_lhex, byte HIGH_or_LOW )     /* select 4bit */
{
	byte a_nibble;
	if ( HIGH_or_LOW == HIGH ){
		a_nibble = ( hhex_lhex & 0xF0 ) >> 4;
	} else if ( HIGH_or_LOW == LOW ){
		a_nibble = ( hhex_lhex & 0x0F );
	} else a_nibble = '#';
	return a_nibble;
}

void output_port0(void)                                     /* PORT0 output pin level */
{
    byte temp;
    
    print(" Port = ");
	temp = two_ascii_to_hex_echo();
	print("h");
    P0 = temp;
}

void output_port1(void)                                     /* PORT1 output pin level */
{
    byte temp;
    
    print(" Port = ");
	temp = two_ascii_to_hex_echo();
	print("h");
	P1 = temp;
}

void output_port2(void)                                     /* PORT2 output pin level */
{
    byte temp;
    
    print(" Port = ");
	temp = two_ascii_to_hex_echo();
	print("h");
    P2 = temp;
}

void output_port3(void)                                     /* PORT3 output pin level */
{
    byte temp;
    
    print(" Port = ");
	temp = two_ascii_to_hex_echo();
	print("h");
        P3 = temp;
}

void print(const byte *ch)                 /* display string ,u must understand pointer concept  */
{
	while(*ch){
		while(!TI);
		TI =0;
		SBUF = *ch++;
	}
}

void print_int(const byte *ch)                 /* display string ,u must understand pointer concept  */
{
	while(*ch){
		while(!TI);
		TI =0;
		SBUF = *ch++;
	}
}

void print_logo(void)                /*  if u press RESET key, show this message */
{
    print("\n\n\n\r");
    print("     Ver 1.1       C.S.L.  Technology Co., Ltd.        \n\r");
    print("                   Press   'H' for Information.      \n\r");
    print("                   since 1997.07.26 by @KYOUNG-SOFT    \n\r");
    print("                   @ copyleft girle999@yahoo.co.kr    \n\r");
}

void putb_ser(byte byte_data)        /* display ex)  0xf9  --> 'f' and '9'  */
{
    putchar( (char)hex_to_ascii(	mask_a_nibble( byte_data, HIGH ) ) );
    putchar( (char)hex_to_ascii(	mask_a_nibble( byte_data, LOW  ) ) );
}

void quit_program(void)
{
    print("\n\r Quit Monitor.. Excute Downloading program from 8000h \n\r");
	((void (code *) (void)) 0x8000) ();
	 /* void (*pfunc)(void);             variable with no parameter and no return value  */
	 /* pfunc = (void(*)(void)) 0;       make a reset vector 0000h and save to pfunc     */
	 /* (*pfunc)();                      excute at 8000h pointer function = "jmp 0000h"  */
	//_opc(0x02);                     /*  #asm	 ljmp	*/
	//_opc(0x80);						/*  #asm     0x80	*/
	//_opc(0x00);						/*  #asm	 0x00	*/
}

void reset (void)
{
	((void (code *) (void)) 0x0000) ();
	print_int("\n\rSoftware RESET !");
	
}

byte two_ascii_to_hex(void)     /*  display ex) '6' and 'e' --> 0x6e   */
{
	byte serial_two;
	byte serial_two2;

	serial_two = ascii_to_hex(get_serial()) << 4;
	serial_two2 = ascii_to_hex(get_serial());
	serial_two |= serial_two2;
	return (serial_two);
}

byte two_ascii_to_hex_echo(void)        /*  two_ascii_to_hex() + display  */
{
	byte serial_two;
	byte serial_two2;
	
	serial_two = ascii_to_hex(get_serial_echo()) << 4;
	serial_two2 = ascii_to_hex(get_serial_echo());
	serial_two |= serial_two2;
	return (serial_two);
}

