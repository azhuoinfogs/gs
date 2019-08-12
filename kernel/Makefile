KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
obj-m := hello.o
hello-objs :=hello_core.o hello_sysfs.o
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm Module.symvers *.o *mod.c *.order *.ko
