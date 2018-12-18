#############################################################################
#
# Makefile for librf24 on Raspberry Pi
#
# License: GPL (General Public License)
# Author:  Fergus Leahy <fergus.leahy@gmail.com>
# Date:    25/09/2014 (version 1.0)
#
LIBDIR=/usr/local/lib
LIBNAME=librf24.so.1.0

# The recommended compiler flags for the Raspberry Pi
CCFLAGS= -Wall -pg -W -fPIC
CFLAGS= -Wall -pg -W -fPIC -pthread

# make all
ifeq ($(platform), pi)
	CCFLAGS+=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
endif

OBJECTS = rf24.o spi.o gpio.o compatibility.o tsqueue.o queue.o rf24Stats.o

all: lib

lib: $(OBJECTS)
	rm -f librf24.a 
	ar r librf24.a $(OBJECTS)
	ranlib librf24.a 


# Library parts
rf24.o: rf24.c rf24.h spi.h gpio.h tsqueue.o
queue.o: queue.c queue.h
tsqueue.o: tsqueue.c tsqueue.h queue.o
gpio.o: gpio.c gpio.h
spi.o: spi.c spi.h
interrupts.o: interrupts.c interrupts.h
rf24Stats.o: rf24Stats.c rf24Stats.h

pingtest: pingtest.c ${OBJECTS}
	gcc ${CFLAGS} pingtest.c ${OBJECTS} -o pingtest -lbcm2835

receiver: receiver.c ${OBJECTS}
	gcc ${CFLAGS} receiver.c ${OBJECTS} -o receiver -lbcm2835

sender: sender.c ${OBJECTS}
	gcc ${CFLAGS} sender.c ${OBJECTS} -o sender -lbcm2835

compatibility.o: compatibility.c compatibility.h
# clear build files
clean:
	rm -rf *o ${LIBNAME}

# Install the library to LIBPATH
install: rf24.o
	gcc -pg rf24.o -shared -o $(LIBNAME)
	cp librf24.so.1.0 ${LIBDIR}/${LIBNAME}
	ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/librf24.so.1
	ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/librf24.so
	ldconfig

# Button_test compilation and linking commands here
button_test:
	gcc -c microphone_setup.c button_test.c
	gcc -Wall -o button microphone_setup.o button_test.o -lwiringPi -lpthread -lasound

uninstall: 
	rm ${LIBDIR}/${LIBNAME} ${LIBDIR}/librf24.so.1 ${LIBDIR}/librf24.so
	ldconfig
	
.PHONY: clean lib all install uninstall

