CC=gcc


SRC_LIST+=$(AVERAGE)/mave.c

LINCS += -I$(FILTER) \
	-I$(AVERAGE)\
	-I$(SW_ROOT)\

all:
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj
