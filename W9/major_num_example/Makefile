obj-m += dev_nr.o
KDIR = ~/workspace/Emb_sys/kernel
RESULT = major_num_example
SRC = $(RESULT).c
Compiler = aarch64-linux-gnu-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(Compiler) -o $(RESULT) $(SRC)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
