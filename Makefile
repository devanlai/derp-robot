PROGRAM = robot

CHUMBYTOUCH = ChumbyTouchFrame

LIBS = -lvncclient
SRCS = main.c frame.c
OBJS = $(SRCS:.c=.o)
CHUMBY_OBJS = $(CHUMBYTOUCH)/screen.o $(CHUMBYTOUCH)/touch.o
LDFLAGS = -I/mnt/storage/usr/local/include -L/mnt/storage/usr/local/lib
CC = gcc
CFLAGS = -Wall

all: $(PROGRAM)

$(PROGRAM) : $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) -o $(PROGRAM)

chumby:
	cd $(CHUMBYTOUCH) && $(MAKE)

.PHONY: clean depend

clean:
	rm -f *.o $(PROGRAM) $(PROGRAM).d
	cd $(CHUMBYTOUCH) && $(MAKE) clean

depend: $(PROGRAM).d

#Download the ChumbyTouchFrame library from git, if you don't have it.
download:
	git clone git://github.com/biomood/ChumbyTouchFrame.git

$(PROGRAM).d:
	$(CC) -MM $(SRCS) > $(PROGRAM).d
main.o: main.c
	$(CC) $(LDFLAGS) $(LIBS) -c main.c -o main.o
#include $(PROGRAM).d
