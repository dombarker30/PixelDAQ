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

#CC	=	g++
CC 	= 	`root-config --cxx`
CXXFLAGS=	`root-config --cflags`
ROOTLIBS = 	`root-config --glibs`
COPTS	=	-fPIC -DLINUX -O2  -std=c++11
SQL     =       -lpqxx -lpq

FLAGS  	=       -Wall 
#FLAGS	=	-soname -s
#FLAGS	=       -Wall,-soname -s
#FLAGS	=	-Wall,-soname -nostartfiles -s
#FLAGS	=	-Wall,-soname

DEPLIBS	=	-lCAENDigitizer -lCAENComm

LIBS	=	-L..

INCLUDEDIR =	-I./include

OBJS	=	$(OBJDIR)/keyb.o $(OBJDIR)/DAQDriver.o $(OBJDIR)/PixelReadout.o  $(OBJDIR)/ChannelMap.o $(OBJDIR)/OnlineMonitor.o  $(OBJDIR)/ArduinoSetup.o $(OBJDIR)/OnlineEventDisplay.o $(OBJDIR)/OnlineDataBase.o
INCLUDES =	./include/*.hh	$(INCLUDEONLINEDIR)/*.hh

INCLUDEONLINEDIR = /home/argonshef/LArAnalysis/srcs/TPC/OnlineAnalysis/

##########################################################################################################################################################

all	:	$(OUT) 

clean	:	
		rm -rf $(OBJS) $(OUT)

$(OUT)	:	$(OBJS) 
		@echo Compiling 
		/bin/rm -f $(OUT)
		if [ ! -d $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
		$(CC) $(CXXFLAGS) $(FLAGS) -o $(OUT) $(OBJS) $(ROOTLIBS) $(SQL) $(DEPLIBS)

$(OBJS) :       $(INCLUDES) Makefile

$(OBJDIR)/%.o	:	$(SRCSDIR)/%.c 
		@echo Building $@ 
		if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

$(OBJDIR)/%.o	:	$(SRCSDIR)/%.cc 
		@echo Building $@ 
		if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
		$(CC) $(CXXFLAGS) $(COPTS) -I$(ROOTSYS)/include $(INCLUDEDIR) -I$(INCLUDEONLINEDIR) -c -o $@ $<

$(OBJDIR)/%.o	:	$(INCLUDEONLINEDIR)/%.cc
		@echo Building $@ 
		if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
		$(CC) $(CXXFLAGS) $(COPTS) -I$(ROOTSYS)/include $(SQL) $(INCLUDEDIR) -I$(INCLUDEONLINEDIR) -c -o $@  $<

.SILENT		:

