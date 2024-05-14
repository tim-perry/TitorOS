# TitorOS
Note: The codebase is currently being reworked, so BarrelOS contains more features but is sketchy (for code demonstration purposes only)
TitorOS is (or hopes to be) a unix-like POSIX-compliant self-hosting x86-64 operating system (and bootloader) designed and developed from scratch

## Build Instructions
Requires: nasm gcc ld make
Running ``make`` in root directory (of TitorOS) will generate a build/ directory with titor.img
titor.img can be run in qemu/bochs/vm or be burnt to live media and booted on real hardware (see makefile for qemu flags)

## TITOR TODO
### CROSS-COMPILER
check osdev wiki page (aparently good)

### KERNEL
pmm
vmm (just kernel heap or think ahead to userspace usages?)
idt
vfs (as api for devices/drivers?)
drivers
    keyboard
    screen/terminal/grahpics mode/vga?
    PIC (apic? iopic? ioapic?) TIMER
    Maybe nvme/sata and filesystem

### USERSPACE
elf loader
scheduler
    process blocking
libc
syscalls
    proper malloc that is implemented in libc except for sbrk etc...
    file access
    Look into POSIX
    fork()
INIT process?
crt0
stdin/stdout
program arguments and environment? (int argc, char** argv)

### APPLICATIONS
shell (with fs navigation builtins)
cat/less
text editor
assembler
linker
C compiler
### +++++SELF-HOSTING++++++
### Then
usb driver
kernel selection in bootloader
graphics driver? opengl? :/
audio driver?
audio file playback (decode wav,mp3,etc)
mouse driver
network driver
networking libraries?
wget? basic html browser? (with url clicking?)
image editor (encoding for png,jpg,bmp,gif?)
