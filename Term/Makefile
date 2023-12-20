obj-m += lcd_driver.o
obj-m += motor_driver.o
KDIR = ~/workspace/kernel

RESULT = term_device
SRC = $(RESULT).c
Compiler = gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(Compiler) -o $(RESULT) $(SRC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
