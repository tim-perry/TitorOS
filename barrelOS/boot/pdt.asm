[org 0x300000]


%define kernel(a) (a == 0 || a == 0xc0000000)
%define heap(a) (a == 0xc0400000)
%define stack(a) (a == 0xefc00000)

pdt:
%assign address 0

%rep 1024

%if kernel(address)
dd code_pt + 0b100000000011
%elif heap(address)
dd heap_pt + 0b100000000011
%else
dd 0
%endif

%assign address address + 0x400000
%endrep

code_pt: ;identity map
%assign address 0
%assign flags 0b100000000011

%rep 1024
%assign entry address + flags
dd entry
%assign address address + 0x1000
%endrep

heap_pt:
%rep 1024
dd 0
%endrep