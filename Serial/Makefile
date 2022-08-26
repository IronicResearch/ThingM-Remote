# Makefile for Serial Port IO console test apps

CC	?= gcc

OBJS	= SerialPortIO.o

LIBS	= -framework IOKit -framework CoreFoundation

EXES	= SerialTerm SerialScript SerialPortSample

.PHONY:	all
all:	$(EXES)

SerialTerm:	SerialTerm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o SerialTerm SerialTerm.o $(LIBS)

SerialScript:	SerialScript.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o SerialScript SerialScript.o $(LIBS)

SerialPortSample:	SerialPortSample.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o SerialPortSample SerialPortSample.o $(LIBS)

.PHONY:	clean
clean: 
	rm -f $(OBJS) $(EXES) *.o
