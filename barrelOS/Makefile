#Kernel C source files
C_SOURCES := $(wildcard kernel/*.c kernel/idt/*.c drivers/*.c drivers/filesystems/*.c)
#Object files of each kernel C source file
OBJ := $(C_SOURCES:.c=.o)

#LibC C source files
LIBC_SOURCES := $(wildcard libc/*.c)
#Object files of each kernel C source file
LIBCOBJ := $(LIBC_SOURCES:.c=.o)

#Assembly files for boot sector
BOOT := $(wildcard boot/*.asm)

#Flags
CFLAGS := -I ./ -ffreestanding -fno-pie -m32 -c
CC := gcc
LDFLAGS :=  -m elf_i386 #--oformat elf32-i386
QEMUFLAGS := -debugcon stdio -m 4G -d int -no-reboot -no-shutdown -M smm=off -D qemu.log #-enable-kvm
DEVICEFLAGS := -drive format=raw,file=barrel.img,id=bootdrive -drive format=raw,file=/hdd/data/nvme.image,if=none,id=nvmedrive -device nvme,serial=nvme,drive=nvmedrive

run:all
	qemu-system-x86_64 $(QEMUFLAGS) $(DEVICEFLAGS)

all: barrel.img user/shell.bin user/ls.bin

barrel.img: boot/bootstrap.bin boot/pdt.bin kernel/kernel.bin
	cat $^ > $@

kernel/kernel.bin: kernel/initkernel.o kernel/idt/isrs.o ${OBJ} libc/string.o
	ld $^ $(LDFLAGS) -T linker.ld -o $@ 

boot/bootstrap.bin: boot/bootstrap.asm $(BOOT)
	nasm -fbin $< -o $@

boot/pdt.bin: boot/pdt.asm
	nasm -fbin $< -o $@

kernel/initkernel.o: kernel/initkernel.asm
	nasm -felf $< -o $@

kernel/idt/isrs.o: kernel/idt/isrs.asm
	nasm -felf $< -o $@


#--oformat elf32-i386

user/shell.bin: libc/crt0.o libc/libc.o user/shell.o
	ld $^ $(LDFLAGS) -T user/user.ld -o $@

user/ls.bin: libc/crt0.o libc/libc.o user/ls.o
	ld $^ $(LDFLAGS) -T user/user.ld -o $@

libc/libc.o: ${LIBCOBJ} libc/syscalls.o libc/init_libc.o
	ld $^ $(LDFLAGS) -r --oformat elf32-i386 -o $@

libc/syscalls.o: libc/syscalls.asm
	nasm -felf $< -o $@

libc/crt0.o: libc/crt0.asm 
	nasm -felf $< -o $@


%.c : *.h

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) kernel/initkernel.o kernel/idt/isrs.o kernel/kernel.bin boot/bootstrap.bin boot/pdt.bin *.img user/*.o user/*.bin libc/*.o
