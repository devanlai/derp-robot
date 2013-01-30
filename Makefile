PROGRAM = robot
LIBS = -lvncclient
SRCS = main.c
OBJS = $(SRCS:.c=.o)
LDFLAGS =
CC = gcc
CFLAGS = -Wall


$(PROGRAM) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROGRAM)

.PHONY: clean depend

clean:
	rm -f *.o $(PROGRAM) $(PROGRAM).d

depend: $(PROGRAM).d

$(PROGRAM).d:
	$(CC) -MM $(SRCS) > $(PROGRAM).d

include $(PROGRAM).d
