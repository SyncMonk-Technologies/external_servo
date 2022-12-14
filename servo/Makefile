CC=gcc

LINREG=$(SERVO)/linreg
NTPSHM=$(SERVO)/ntpshm
PI=$(SERVO)/pi

export SERVO
export SW_ROOT
export LINREG
export NTPSHM
export PI

SRC_LIST=$(SERVO)/servo.c
LINCS += -I$(SERVO) \
	-I$(SW_ROOT)\
	-I$(LINREG)\
	-I$(NTPSHM)\
	-I$(PI)

all:
	make all -C $(LINREG) CFLAGS="$(CFLAGS)"
	make all -C $(NTPSHM) CFLAGS="$(CFLAGS)"
	make all -C $(PI) CFLAGS="$(CFLAGS)"

	$(CC) -c $(SRC_LIST) $(LINCS) $(CFLAGS)
	mv *.o $(SW_ROOT)/obj
	
clean:
	rm -rf $(LINREG)/obj
	rm -rf $(NTPSHM)/obj
	rm -rf $(PI)/obj
	
