#Variable declaration

OBJECTS := $(wildcard ./*/*.o)
BINS := $(wildcard ./*/*.bin)

all:kernel.img

run:all
	qemu-system-x86_64 -debugcon stdio -enable-kvm -drive format=raw,file=kernel.img

kernel.img: ./boot/bootstrap.bin ./kernel/kernel.bin
	cat $^ > $@



./boot/bootstrap.bin: ./boot/bootstrap.asm ./boot/stdio.asm ./boot/diskio.asm ./boot/gdt.asm
	nasm -fbin $< -I './' -o $@

./kernel/kernel.bin: ./kernel/initkernel.o ./kernel/kernel.o ./drivers/screen.o ./kernel/idt.o ./kernel/isrs.o ./drivers/utils.o ./kernel/interrupt.o ./drivers/keyboard.o ./drivers/timer.o
	ld $^ -m elf_i386 -Ttext 0x100000 --oformat binary -o $@

./drivers/%.o: ./drivers/%.c
	gcc $< -I ./ -Wpedantic -fno-pie -m32 -ffreestanding -c -o $@

./kernel/%.o: ./kernel/%.c
	gcc $< -I ./ -Wpedantic -fno-pie -m32 -ffreestanding -c -o $@



./kernel/initkernel.o: ./kernel/initkernel.asm
	nasm -felf $^ -o $@

./kernel/isrs.o: ./kernel/isrs.asm
	nasm -felf $^ -o $@





clean:
	rm -r $(OBJECTS) $(BINS) kernel.img
