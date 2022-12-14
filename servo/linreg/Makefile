CC=gcc

SRC_LIST+=$(LINREG)/linreg.c

INC_LIST+=$(LINREG)/linreg.h\
	$(SW_ROOT)/logger.h\
	$(SERVO)/servo.h
	

LINCS += -I$(SERVO) \
	-I$(LINREG)\
	-I$(SW_ROOT)\
	
all:
	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj

clean:
	rm -rf $(LINREG)/obj
