obj-m += led_driver.o
obj-m += lcd_driver.o
obj-m += pwm_driver.o
KDIR = ~/workspace/kernel

RESULT1 = term_server
SRC1 = $(RESULT1).c
RESULT2 = term_client
SRC2 = $(RESULT2).c
Compiler = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(Compiler) -o $(RESULT1) $(SRC1)
	$(Compiler) -o $(RESULT2) $(SRC2)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT1)
	rm -f $(RESULT2)
