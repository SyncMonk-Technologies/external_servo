CC=gcc
SW_ROOT=$(CURDIR)
SERVO=$(SW_ROOT)/servo
FILTER=$(SW_ROOT)/filter

export SW_ROOT
export FILTER
export SERVO


ifeq ($(DEBUG), 1)
	CFLAGS+= -g
else
	CFLAGS+= -O2
endif

CFLAGS+= -Wall -DLINUX_PTP

LINCS = -I$(SERVO) \
	-I$(SW_ROOT)\
	-I$(FILTER)\

LDLIBS = -lrt -lm

SRC_LIST=$(SW_ROOT)/clockadj.c\
	$(SW_ROOT)/logger.c\
	$(SW_ROOT)/msg.c\
	$(SW_ROOT)/tsproc.c\
	$(SW_ROOT)/uds.c\
	$(SW_ROOT)/main.c\

all:
	mkdir -p $(SW_ROOT)/obj
	make all -C $(SERVO) CFLAGS="$(CFLAGS)"
	make all -C $(FILTER) CFLAGS="$(CFLAGS)"

	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj
	$(CC) -o ext_servo $(SW_ROOT)/obj/*.o $(LDLIBS)

clean:
	rm -rf $(SW_ROOT)/obj
	rm -rf ext_servo





