cmd_/root/code/gs/kernel/hello.o := ld -m elf_x86_64   -r -o /root/code/gs/kernel/hello.o /root/code/gs/kernel/hello_core.o /root/code/gs/kernel/hello_sysfs.o ; scripts/mod/modpost /root/code/gs/kernel/hello.o
