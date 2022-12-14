CC=gcc


SRC_LIST+=$(PI)/pi.c

LINCS += -I$(SERVO) \
	-I$(PI)\
	-I$(SW_ROOT)\

all:
	mkdir -p $(PI)/obj
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj

clean:
	rm -rf $(PI)/obj
