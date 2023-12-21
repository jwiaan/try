#include <stdint.h>

extern char time;
void star(void);
void user(void);

struct {
	uint32_t u32;
	uint64_t u64[12];
	uint16_t u16[2];
} __attribute__((packed)) tss = {.u64[0] = 0xffff800000006000,.u16[1] = 104 };

struct s80 {
	uint16_t u16;
	void *p;
} __attribute__((packed));

union u128 {
	struct {
		uint16_t u16[2];
		uint8_t u8[4];
		uint32_t u32[2];
	} __attribute__((packed));
	uint64_t u64[2];
};

struct s128 {
	uint16_t u16[4];
	uint32_t u32[2];
} idt[33] = { };

uint64_t gdt[8] = {
	0,
	0x0020980000000000,
	0x0000920000000000,
	0,
	0x0000f20000000000,
	0x0020f80000000000,
};

struct s128 gate(void *p)
{
	uint64_t u = (uint64_t) p;
	struct s128 s = { {u, 8, 0x8e00, u >> 16}, {u >> 32} };
	return s;
}

union u128 tssd(void)
{
	uint64_t p = (uint64_t) & tss;
	union u128 u = { {{104, p}, {p >> 16, 0x89, 0, p >> 24}, {p >> 32}} };
	return u;
}

void init(void)
{
	union u128 u = tssd();
	gdt[6] = u.u64[0];
	gdt[7] = u.u64[1];
	idt[32] = gate(&time);

	struct s80 g = { sizeof(gdt) - 1, gdt };
	struct s80 i = { sizeof(idt) - 1, idt };
	uint16_t t = 48;
	asm("lgdt %0;lidt %1;ltr %2;sti"::"m"(g), "m"(i), "m"(t));
}

int main(void)
{
	star();
	init();
	user();
}
