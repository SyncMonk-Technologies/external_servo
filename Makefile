CC=gcc

MEDIAN=$(FILTER)/median
AVERAGE=$(FILTER)/average
export MEDIAN
export AVERAGE

SRC_LIST+=$(FILTER)/filter.c

LINCS += -I$(FILTER) \
	-I$(SW_ROOT)\
	-I$(MEDIAN)\
	-I$(AVERAGE)

all:
	make all -C $(MEDIAN) CFLAGS="$(CFLAGS)"
	make all -C $(AVERAGE) CFLAGS="$(CFLAGS)"
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj