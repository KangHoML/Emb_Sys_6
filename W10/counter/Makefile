obj-m += seg_driver.o
obj-m += button_driver.o
KDIR = ~/workspace/Emb_sys/kernel
RESULT = counter
SRC = $(RESULT).c
Compiler = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(Compiler) -o $(RESULT) $(SRC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
