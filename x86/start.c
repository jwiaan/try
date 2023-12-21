#include <stdint.h>

enum { N = 3 };

struct {
	uint32_t stack[100], esp;
} task[N], *curr = task;

struct {
	uint32_t __, *esp0;
	uint16_t ss0, _[46], io;
} tss = {.ss0 = 16,.io = 103 };

#define save(m) asm("pusha;push %ds;push %es");asm("mov %%esp,%0;out %1,$0x20":"=m"(m):"a"((uint8_t)0x20))
#define load(m) asm("mov %0,%%esp;pop %%es;pop %%ds;popa;iret"::"m"(m))
__attribute__((naked))
void clock(void)
{
	save(curr->esp);
	if (++curr == task + N)
		curr = task;

	tss.esp0 = &curr->esp;
	load(curr->esp);
}

void start(int i)
{
	i *= 160;
	i += 0xb8000;
	while (1)
		asm("incb (%0)"::"r"(i));
}

void init(int i)
{
	uint32_t esp = 0x100000 + 0x1000 * i - 4;
	*(int *)esp = i;
	task[i].stack[0] = 35;
	task[i].stack[1] = 35;
	task[i].stack[10] = (uint32_t) start;
	task[i].stack[11] = 27;
	task[i].stack[12] = 0x202;
	task[i].stack[13] = esp - 4;
	task[i].stack[14] = 35;
	task[i].esp = (uint32_t) task[i].stack;
}

void setup(void)
{
	static uint64_t gdt[6] = { 0,
		0xcf98000000ffff,
		0xcf92000000ffff,
		0xcff8000000ffff,
		0xcff2000000ffff,
	};
	gdt[5] = 0x890000000067 + ((uint32_t) & tss << 16);
	uint16_t gdtr[3] = { sizeof(gdt) - 1 };
	*(uint64_t **) (gdtr + 1) = gdt;
	asm("lgdt %0;ltr %1"::"m"(gdtr), "r"((uint16_t) 40));

	static uint64_t idt[256];
	idt[8] = 0x8e0000080000 + (uint32_t) clock;
	uint16_t idtr[3] = { sizeof(idt) - 1 };
	*(uint64_t **) (idtr + 1) = idt;
	asm("lidt %0;sti"::"m"(idtr));
}

void _start(void)
{
	for (int i = 1; i < N; ++i)
		init(i);

	setup();
	start(0);
}
