#include <stdint.h>

typedef struct idt_entry {
	uint16_t base_lo;
	uint16_t segment;
	uint8_t reserved;
	uint8_t flags;
	uint16_t base_hi;
} __attribute__((packed)) idt_entry_t;

struct idt_descriptor {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void init_pic();
void init_idt();