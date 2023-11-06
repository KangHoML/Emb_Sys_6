obj-m += seg_driver.o
KDIR = ~/workspace/Emb_sys/kernel
RESULT = seg_example
SRC = $(RESULT).c
RESULT2 = seg_example2
SRC2 = $(RESULT).c
Compiler = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(Compiler) -o $(RESULT) $(SRC)
	$(Compiler) -o $(RESULT2) $(SRC2)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
	rm -f $(RESULT2)
