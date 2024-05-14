#include <stdint.h>
#include <drivers/utils.h>
#include <drivers/pci.h>
#include <drivers/screen.h>
#include <kernel/vmm.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

uint32_t pci_devices[32];

void init_pci() {

	uint8_t no_pci_devices = 0;
	uint32_t address;

	for (uint8_t bus = 0; bus < 255; bus++) {
		for (uint8_t device = 0; device < 32; device++) {
			address = getaddress(bus, device, 0, 0);
			if (pciread(address) != 0xFFFFFFFF) {
				//add to list of pci devices
				pci_devices[no_pci_devices] = address;
				no_pci_devices++;
			}
		}	
	}
	pci_devices[no_pci_devices] = 0xFFFFFFFF; //dummy pci device that indicates end of list
}

uint32_t detect_nvme() {
	uint32_t nvme;
	int i = 0;
	do {
		nvme = pci_devices[i];
		if (pciread(nvme + 0x8) >> 16 == 0x0108) return nvme;
		i++;
	} while (nvme != 0xFFFFFFFF);
	kpanic("NO NVME DRIVE DETECTED");
}

uint32_t getaddress(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)device;
    uint32_t lfunc = (uint32_t)function;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    return address;
}

uint32_t pciread(uint32_t address) {
    outdword(CONFIG_ADDRESS, address);
    return indword(CONFIG_DATA);
}

void pciwrite(uint32_t address, uint32_t data) {
    outdword(CONFIG_ADDRESS, address);
    outdword(CONFIG_DATA, data);
}


struct pci_header_t getHeader(uint32_t address) { //must have offset of zero
    uint32_t* registers = kmalloc();
    for (int i = 0; i < 0xF; i++) {
        registers[i] = pciread(address + i * 4);
    }
    struct pci_header_t* header;
    header = (struct pci_header_t*) registers;

    return *header;
}