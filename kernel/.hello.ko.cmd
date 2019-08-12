cmd_/root/code/gs/kernel/hello.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /root/code/gs/kernel/hello.ko /root/code/gs/kernel/hello.o /root/code/gs/kernel/hello.mod.o
