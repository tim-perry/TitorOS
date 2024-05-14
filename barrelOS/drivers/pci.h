#include <stdint.h>

void init_pci();
uint32_t detect_nvme();
uint32_t getaddress(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
uint32_t pciread(uint32_t address);
void pciwrite(uint32_t address, uint32_t data);
struct pci_header_t getHeader(uint32_t address);

struct pci_header_t{
    uint16_t vendor_id;
    uint16_t device_id;
    
    uint16_t command;
    uint16_t status;
    
    uint8_t revision;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t class;

    uint8_t cache;
    uint8_t latency_timer;
    uint8_t header;
    uint8_t BIST;

    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;

    uint32_t CISpointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t ROM_base;
    uint8_t cap_pointer;
    uint16_t reserved1;
    uint8_t reserved2;
    uint32_t reserved3;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;

}__attribute__((packed));
