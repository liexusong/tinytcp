                    TinyTCP, Embedded TCP/IP Protocol Suite

                     Dallas DS89C420(8051 Compatible) Port
			    for the Keil C Compiler



Acknowledgements
----------------

Most files in this TinyTCP is based on the Dallas DS89C420(8051 Compatible)
for the Keil C Compiler by TechiZ.

The TinyTCP source is available at 
http://www.csonline.net/bpaddock/tinytcp/default.htm

The TinyTCP port for the Dallas DS89C420(8051 Compatible) is available at
http://www.techiz.com/

Hi everyone.

This program was written in Korean(Comment and some more). 

This program was developed by TechiZ(The Company name).
TechiZ want to share this program with you who loves the 8051 & the TCP/IP.

You DO NOT USE THIS CODE FOR COMMERCIAL PURPOSE.
This code is ONLY FREE FOR THE STUDY. If you want more, send me the mail.

E-mail: techiz@techiz.com
( Subject is : [DS89C420 & TinyTCP] bla~ bla bla.... )

You DO NOT DELETE BELOW THE COPYRIGHT MESSAGE IN THE USING OF THIS CODE.

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

In the using of this code, TechiZ does NOT GUARANTEE ABOUT WORKING WITHOUT ERROR.

Below sentenses are not translated yet...  it's not a important information but
only bug repair. I don't know this source have any bug. If you found it, let me know..



REPORTING PROBLEMS
------------------

If you find a bug in this port, DO NOT hesitate to report the problem to me
(Gwang-Hyuk, Ahn.):

1) Through E-mail: techiz@techiz.com
( Subject is : [DS89C420 & TinyTCP] bla~ bla bla.... )

2) Use Q&A Board in www.techiz.com(Open Src menu, about 8051)
( Subject is : [DS89C420 & TinyTCP] bla~ bla bla.... )

Make sure that the problem you are reporting is in this port and not in your
application.

Enjoy!!



Hardware
--------
ROM : 0 - 0x7FFF
RAM : 0x8000 - 0xffff
RTL8019AS(Ethernet chip) : ROM area(base = 0x300)



Software
--------
Monitor program : downloadable and executable (from 0x8000) 
// We only use 4M ROM emulator which was made by comrade.
TinyTCP program ( it was repaired to port 8051. Original code is remarked as '// '.)
Ethernet Driver(RTL8019AS)



-------------------------------------------------------------------------------------
Korean Language Add-On ( We are so sorry about add-on,
                         We will translate later if many people want...  ^^;;  )

만든 패킷을 보내는 것이 성공을 했네요...

1. 버그라면... 연속된 패킷을 4개 또는 5개를 보내면 프로그램이 멈춰버리네요.. -,.-

2. 이제는 약간의 버그를 무시한채 , 그냥 패킷을 받으려고 하는데요. 아직 인터럽트 부분에서
   걸리는 것이 상당히 많습니다. 머냐면 케일 컴파일러가 인터럽트 벡터를 0003(절대번지)에 
   생성하기 때문에 저처럼 램(8000h)에 다운로딩하여 개발하는 경우는 동작이 안된다.
   아마도 startup부분에서 해결해야 할 것이다. 참고로 아르키메데스 컴파일러에서는 인터럽트
   부분에서 아무 이상이 없었는데    .  ㅠ.ㅠ 주르륵..

5. 인터럽트 부분 테스트까지 입니다. 케일의 인터럽트는 절대번지에 절대적으로 위치하기 때문에
   활용을 하려면 롬에뮬에다 집어넣고 하는 방법이 가장 좋은 것 같네요. 그리고 소스에서 에러나는
   부분은 DMA_read 부분인데, 여기에 관련된 byte *buf만 제거하면 컴파일에 문제는 없습니다.


6. 2000.11.12 : 인터럽트가 조금 이상하다. RTL8019AS의 IMR과 ISR의 정확한 관계를 모르겠다.
   예를 들면 IMR을 마스크하지 않아도 인터럽트는 발생했다.. 그래서 현재 IMR의 셋팅은 0x01로 했다.

7. 2000.11.16 DoS를 모방하려고 열나게 똑같은 패킷을 날렸을 때, 너무 빠르게 전송하면 보드가 멈춰
   버린다. 어쩔 때는 안 멈추고 잘 전송은 되지만, 아무런 delay없이 전송되면 바로 다운된다.

8. 초기화를 잘못해서 송신이 전혀 안되었다. 아래 소스는 현재 소스와의 차이만 나타낸 부분이다. 
   물론 동작이 안되는 소스이다. 왜 그런지는 정말 모르겠다. 결국에는 예전에 썼던 소스를 카피해서
   사용했다. 
	EN_CMD = EN_NODMA + EN_PAGE0 +  EN_STOP;  /* 00001010B: 
	EN0_ISR = 0x00;
	EN0_ISR = 0xff; 	
    					
	EN0_IMR = ENISR_ALL; 

	EN_CMD = EN_NODMA + EN_PAGE3 + EN_STOP;
	//CONFIG1 = 0x90;   
	//CONFIG2 = 0x20;
	//CONFIG3 = 0xf0;
	
	CONFIG1 = 0x90;  
	CONFIG2 = 0x00;
	CONFIG3 = 0xf0;
	CONFIG4 = 0x00;
	
	EN_CMD = EN_NODMA + EN_PAGE3 + EN_STOP;
	EN0_TXCR = ENTXCR_TXCONFIG;     
   
	EN0_RXCR = ENRXCR_RXCONFIG;      
	
	EN_CMD = EN_NODMA + EN_PAGE3 + EN_START;
	EthRxBufRdPtr = 0;
	EthRxBufWrPtr = EthRxBufRdPtr + 1;

9. ei_rx_overrun 함수에서 다음과 같이
	if (was_txing) { 
		if (!(EN0_ISR & (ENISR_TX + ENISR_TX_ERR))) must_resend = 1;
   	}
    바꾸어야 한다. 그리고 수시로 isr_all 부분을 참조하기 바란다.

10. 워치독 타이머0 비슷하게 사용하고 있어서 막히는 부분은 아싸리 초기화를 시키고 있구요...
    에러는 안 뜨지만, 시리얼을 인터럽트와 메인 루틴에서 동시에 쓰면 보드가 멈춰 버림니다.
    이 부분은 시리얼 인터럽트와 링버퍼를 사용하여 해결할 문제이고, 음....  만약 패킷만 정말로
    제대로 수신했다면 그 다음 문제는 tinytcp에서 실제로 확인을 하는 것만 남았습니다.

11. inportb & outportb로 레지스터를 액세스하면 이상한 값이 쓰인다. !!!!
    현재 대충 드라이버로서 동작은 하는것 같다. 한가지 이상한 값이 가끔 보인다. 아마도 inytcp와 
    결합한 후에 더 두보 볼일이다.

12. 현재 22.1184Mhz의 속도에서 8Khz의 timer interrupt가 걸렸다고 가정하고 ethernet 
    드라이버를 짜고 있다. 하지만 속도에서 상당한 문제가 있다. 우선 초당 8000개의 데이타를
    네트워크로 보내려면 패킷의 최대 1500바이트, 즉 8000/1500 = 5.3 그러니까 아무 딜레이
    없이 6개의 패킷을 보낸다면 음성데이타를 정확히 전달할 수 있다. 하지만 6개의 패킷을 
    받기도 해야하기 때문에 아직은 할 수가 없다.

13. 언터럽트 부하에 따라 클라이언트 프로그램(ADUC.C)은 변경되어야 한다. 실험한 결과 느리
    프로세서에서는 usleep(number)를 늘려줄 필요가 있다. 그렇게 하지 않으면 몇번 패킷을 
    받다가 깨진다.
14. IE를 셋팅하여(0x87) adpcm을 구동하면 ethernet이 동작을 하지 않는다. 루틴중에서
    EA = 0; 으로 만들어 주는 부분이 있는데, 아마도 조심해야 될것 같다.

15. drv_8019.c 에서 byte loop를 word loop로 바꾸어줌. 만약 이렇게 하지 않으면 255문자가 넘는
    데이타에 대해서는 에러를 일르켜 보드가 죽을 것이다.
    
16. 지금까지는 arp를 강제로 셋팅하여 사용하였다. 무쉭하게 -_-;   
    Cygwin에서  : arp -s 202.30.20.172 12-34-56-78-9a-bc
    Linux 에서  : arp -s 202.30.20.172 123456789abc
    이런방법으로 root의 권한을 얻어서 사용하였다. 하지만 이제부터는 그럴 필요가 없다. 
    버그를 잡았으니까..   내용인즉, 아래와 같이 2번설정을 하였다. 결국에는 0을 집어넣은
    결과가 되니까 안될 수 밖에..  그래서 아랫줄을 지우거나 주석처리해야한다. 
    
    EN0_RXCR = ENRXCR_RXCONFIG;      /* rx on(broadcasts, no multicast,errors */
    //EN0_RXCR = 0;      /* rx on(broadcasts, no multicast,errors */ 

17. 한번 응용 프로그램이 연결이 되면 broadcat를 disable시켜서 속도를 향상시킴
    tinymain.c의 WORD test_application( void )에 추가시킴.

18. hdr[3]이라고 되어 있는부분을 [4]고쳐야한다.