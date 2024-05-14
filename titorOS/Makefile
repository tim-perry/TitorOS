#Files
IMAGE := titor.img

STAGE2_CSOURCES := $(wildcard bootloader/stage2/*.c)
STAGE2_HEADERS := $(wildcard bootloader/stage2/*.h)
STAGE2_ASMSOURCES := $(wildcard bootloader/stage2/*.asm)
STAGE2_OBJS := $(patsubst %.c,build/%.o,$(STAGE2_CSOURCES)) $(patsubst %.asm,build/%.o,$(STAGE2_ASMSOURCES))

KERNEL_CSOURCES := $(wildcard kernel/*.c)
KERNEL_HEADERS := $(wildcard kernel/*.h)
KERNEL_ASMSOURCES := $(wildcard kernel/*.asm)
KERNEL_OBJS := $(patsubst %.c,build/%.o,$(KERNEL_CSOURCES)) $(patsubst %.asm,build/%.o,$(KERNEL_ASMSOURCES))

#C Flags
CC := gcc
COPT := 3
CFLAGS := -Wall -Wextra -I ./ -ffreestanding -fno-pie -ffunction-sections

#QEMU
QEMUSYSTEM := qemu-system-x86_64
QEMUFLAGS := -debugcon stdio -m 8G -d int -no-reboot -no-shutdown -M smm=off -D qemu.log -enable-kvm
DRIVEFLAGS := -drive format=raw,file=build/$(IMAGE)


#GENERAL
.PHONY: run clean 

run: build/$(IMAGE)
	$(QEMUSYSTEM) $(QEMUFLAGS) $(DRIVEFLAGS)

build/$(IMAGE): build/bootloader/bootloader.bin build/kernel/kernel.elf
	cat $^ > $@

clean:
	rm -r build


#BOOTLOADER
build/bootloader/bootloader.bin: bootloader/stage1/bootsector.asm bootloader/stage1/bootlib.asm build/bootloader/stage2/stage2.bin
	nasm -fbin $< -o $@

build/bootloader/stage2/stage2.bin: $(STAGE2_OBJS)
	ld -nostdlib -static -m elf_i386 -T bootloader/stage2/linker.ld $^ -o $@

build/bootloader/stage2/%.o: bootloader/stage2/%.c $(STAGE2_HEADERS)
	mkdir -p ${dir $@}
	$(CC) $(CFLAGS) -O$(COPT) -m32 -c $< -o $@

build/bootloader/stage2/%.o: bootloader/stage2/%.asm
	mkdir -p ${dir $@}
	nasm -felf32 $< -o $@


#KERNEL
build/kernel/kernel.elf: $(KERNEL_OBJS)
	ld -nostdlib -static -T kernel/linker.ld $^ -o $@

build/kernel/%.o: kernel/%.c $(KERNEL_HEADERS)
	mkdir -p ${dir $@}
	$(CC) $(CFLAGS) -O$(COPT) -mcmodel=kernel -m64 -c $< -o $@

build/kernel/%.o: kernel/%.asm
	mkdir -p ${dir $@}
	nasm -felf64 $< -o $@