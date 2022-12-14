CC=gcc


SRC_LIST+=$(MEDIAN)/mmedian.c

LINCS += -I$(FILTER) \
	-I$(MEDIAN)\
	-I$(SW_ROOT)\

all:
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj
