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
 
//  이 프로그램은 Linux 또는 Cygwin에서 컴파일하여 실행한다. 
//그러면 8051과 이더넷을 통해서 메시지를 주고 받는다.

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#define PORT	9999

int main()
{
	int sockfd;
	int len;
	int i;
	char ch;
	struct sockaddr_in address; /*AF_INET 도메인에서 주소는 sockaddr_in이라 불리는 구조체를 */
					/*사용하여 명시한다. 이것은 netinet/in.h 에 정의한다*/
	int result;
	int cmd;
	char buf[1600];
    
	i = 0;
	
	sockfd = socket( AF_INET, SOCK_STREAM, 0);/*소켓을 생선한다.*/
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr( "210.107.198.178" );
	address.sin_port = htons(PORT);
    
	len = sizeof(address);

	result = connect( sockfd, (struct sockaddr *)&address, len);
    
    	if ( result == -1) {
		perror("oops: client1");
		exit(1);
	}
	printf("Connection Established..\n");
	while(1) {
		i++;
		//usleep(100000);
		write(sockfd,"I'm CSL..!" ,80);
		printf("\t send number : %d \r\n",i);
		memset(buf, 0, 512);
		len = read(sockfd, buf, 1600);
		if (len > 0){
	    	printf("%s\r\n",buf);
		}      
	}
	
	close(sockfd);
}
