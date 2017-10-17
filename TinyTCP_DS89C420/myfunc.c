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
 
/* *src를 numbytes만큼 *dest으로 카피한다. */

void Move( unsigned char *src, unsigned char *dest, unsigned int numbytes )
{
	if ( numbytes <= 0 ) 
    		return;
	if ( src < dest ) {
		src += numbytes;
		dest += numbytes;
		do {
			*--dest = *--src;
		} while ( --numbytes > 0 );
	} else {
		do {
		*dest++ = *src++;
		} while ( --numbytes > 0 );
	}
}


