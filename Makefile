obj-m += ouichefs.o ouichefs_strategy_changer.o
ouichefs-objs := fs.o super.o inode.o file.o dir.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc ioctl_ouichefs.c -o ioctl_ouichefs
	cp ouichefs.ko ../PNL/share
	cp ouichefs_strategy_changer.ko ../PNL/share
	cp ioctl_ouichefs ../PNL/share

debug:
	make -C $(KERNELDIR) M=$(PWD) ccflags-y+="-DDEBUG -g" modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -rf *~

.PHONY: all clean
