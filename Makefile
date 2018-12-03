########################################################################
# Makefile for the sheffield rig pixel experiment
# Modified from CAEN sample library  file by A. Lucchesi
# D. Barker 29.08.2018
#########################################################################
ARCH	=	`uname -m`

OUTDIR  =    	./bin/$(ARCH)/Release/
OUTNAME =    	DAQDriver.bin
OUT     =    	$(OUTDIR)/$(OUTNAME)
OBJDIR	=	./bin/objdir
SRCSDIR	=	./srcs

CC	=	g++

COPTS	=	-fPIC -DLINUX -O2  -std=c++11

FLAGS  	=       -Wall 
#FLAGS	=	-soname -s
#FLAGS	=       -Wall,-soname -s
#FLAGS	=	-Wall,-soname -nostartfiles -s
#FLAGS	=	-Wall,-soname

DEPLIBS	=	-lCAENDigitizer

LIBS	=	-L..

INCLUDEDIR =	-I./include

OBJS	=	$(OBJDIR)/keyb.o $(OBJDIR)/DAQDriver.o $(OBJDIR)/PixelReadout.o 

INCLUDES =	./include/*

#########################################################################

all	:	$(OUT)

clean	:
		/bin/rm -f $(OBJS) $(OUT)

$(OUT)	:	$(OBJS)
		/bin/rm -f $(OUT)
		if [ ! -d $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
		$(CC) $(FLAGS) -o $(OUT) $(OBJS) $(DEPLIBS)

$(OBJS) :       $(INCLUDES) Makefile


$(OBJDIR)/%.o	:	$(SRCSDIR)/%.c
		if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

$(OBJDIR)/%.o	:	$(SRCSDIR)/%.cc
		if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

.SILENT		:
