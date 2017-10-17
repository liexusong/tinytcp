#/*
# * Copyright (C) 2002 by TechiZ. All rights reserved.
# *
# * This program was written in Korean(Comment and some more). 
# *
# * This program was developed by TechiZ(The Company name).
# * TechiZ want to share this program with you who loves the 8051 & the TCP/IP.
# * 
# * You DO NOT USE THIS CODE FOR COMMERCIAL PURPOSE.
# * This code is ONLY FREE FOR THE STUDY. 
# * If you want more, send me E-mail.
# *
# * E-mail: techiz@techiz.com
# * ( Subject is : [DS89C420 & TinyTCP] bla~ bla bla.... )
# *
# * Homepage: http://www.techiz.com
# * 
# * You DO NOT DELETE THIS COPYRIGHT MESSAGE IN THE USING OF THIS CODE.
# *
# * In the using of this code, TechiZ does NOT GUARANTEE ABOUT WORKING WITHOUT ERROR.
# */
COMPILER	= Keil 8051 C-Compiler V5.10 
PROJ_DIR	= c:\cpu\C51\EXAMPLE\

AS_EXE		= c:\CPU\KEIL\BIN\A51
CC_EXE		= c:\CPU\KEIL\BIN\C51
LD_EXE		= c:\CPU\KEIL\BIN\BL51
HEX_EXE		= c:\CPU\KEIL\BIN\OH51

RM			= del
AS_CMD		= MACRO EP PR XR
CC_CMD		= CD LA SB WL(1) REGPARMS
# cc_cmd options : REGPARMS
# cc_cmd options : WL(1)       ; warning level
# If you use "SRC" option in CC_CMD line, $(FILE_NAME).OBJ is not created!! Remember this....
LD_CMD		= RS(128) CO(0000H) XDATA(8000H) IX RF($(TARGET).REG)
#LD_CMD		= RS(256) CO(0000H) IX RF($(OUTPUT).REG)
# If you wanna download program at 8000H, you must rewrite CO(8000H)
# STACK DATA IDATA Option is modified by urself. (I don't want to setup this Command Line anymore  -_-;)

OBJ1 = monitor.obj
OBJ2 = drv_8019.obj
OBJ3 = init8051.obj
OBJ4 = main.obj
OBJ5 = tinymain.obj
OBJ6 = timer.obj
OBJ7 = arp.obj
OBJ8 = myfunc.obj
OBJ9 = tinytcp.obj



OBJS		= $(OBJ1) \
			$(OBJ2) \
			$(OBJ3) \
			$(OBJ4) \
			$(OBJ5) \
			$(OBJ6) \
			$(OBJ7) \
			$(OBJ8) \
			$(OBJ9) 			
			
SRCS		= $(OBJS:.obj=.c)			
TARGET 		= TEST
HEADER		= PROTO.H INCLUDE.H GLOBAL.H TINYTCP.H DEPEND.H GLOBAL_T.H REG_8019.H
#####################################################################
        
all: $(OBJS)
	$(LD_EXE) @<<DEBUG.LNK
$(OBJ1),$(OBJ2),$(OBJ3),$(OBJ4),$(OBJ5),$(OBJ6),$(OBJ7),$(OBJ8),$(OBJ9) TO $(TARGET).ABS $(LD_CMD)
<<
	$(HEX_EXE) $(TARGET).ABS
#	COPY $(TARGET).HEX $(XTALK_LOCATION)
#	COPY $(TARGET).HEX $(SKCOMM_LOCATION)
.c.obj: 
	$(CC_EXE) $*.c $(CC_CMD)
.a51.obj:
	$(AS_EXE) $(TARGET).a51 $(AS_CMD) 
main.obj:	$(HEADER)

#####################################################################

clean: 
	$(RM) *.obj
    $(RM) *.lst
	$(RM) *.abs
	$(RM) *.hex
	$(RM) *.m51

#####################################################################
#	$*	확장자가 없는 현재의 목표화일(TARGET)
#	$@	현재의 목표화일
#	$<	현재의 목표화일보다 더 최근에 갱신된 화일이름
#	$?	현재의 목표화일보다 더 최근에 갱신된 화일이름(The list)

#$(OBJ1),$(OBJ2),$(OBJ3),$(OBJ4),$(OBJ5),$(OBJ6),$(OBJ7),$(OBJ8),$(OBJ9),$(OBJ10) TO $(TARGET).ABS $(LD_CMD)