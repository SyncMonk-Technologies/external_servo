CC=gcc

SRC_LIST+=$(NTPSHM)/ntpshm.c

LINCS += -I$(SERVO) \
	-I$(NTPSHM)\
	-I$(SW_ROOT)\

all:
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj

clean:
	rm -rf $(NTPSHM)/obj
