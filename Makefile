PROGRAM = robot

CHUMBYTOUCH = ChumbyTouchFrame

LIBS = -lvncclient
SRCS = main.c
OBJS = $(SRCS:.c=.o)
CHUMBY_OBJS = $(CHUMBYTOUCH)/screen.o $(CHUMBYTOUCH)/touch.o
LDFLAGS =
CC = gcc
CFLAGS = -Wall

all: $(PROGRAM)

$(PROGRAM) : $(OBJS) chumby
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) $(CHUMBY_OBJS) -o $(PROGRAM)

chumby:
	cd $(CHUMBYTOUCH) && $(MAKE)

.PHONY: clean depend

clean:
	rm -f *.o $(PROGRAM) $(PROGRAM).d
	cd $(CHUMBYTOUCH) && $(MAKE) clean

depend: $(PROGRAM).d

$(PROGRAM).d:
	$(CC) -MM $(SRCS) > $(PROGRAM).d

include $(PROGRAM).d
