#include <stdint.h>

void init();
void reset();
void clock();
void getid();
void print();
void start();

struct {
	uint32_t __, **esp0;
	uint16_t ss0, _[46], io;
} tss = {.ss0 = 16,.io = 103 };

struct {
	uint32_t id, stack[100], *esp;
} tasks[3], *task = tasks;

uint32_t *schedule(uint32_t top)
{
	task->esp = &top;
	if (++task == tasks + sizeof(tasks) / sizeof(tasks[0]))
		task = tasks;

	tss.esp0 = &task->esp;
	return task->esp;
}

uint32_t clone(void)
{
	static uint32_t m = 0x100000;

	uint32_t *s = (uint32_t *) 0xfffff000, *d = (uint32_t *) 0xffffe000;
	s[1022] = m | 7;
	d[512] = s[512];
	d[0] = s[0] = m + 0x1000 | 7;

	uint32_t *pt0 = (uint32_t *) 0xffc00000;
	pt0[0] = (uint32_t) start & 0x7ffff000 | 7;
	s[0] = s[1022] = 0;
	reset();

	uint32_t cr3 = m;
	m += 0x2000;
	return cr3;
}

void init_task(uint32_t i)
{
	tasks[i].id = i;
	tasks[i].stack[0] = clone();
	tasks[i].stack[1] = 35;
	tasks[i].stack[2] = 35;
	tasks[i].stack[11] = (uint32_t) start & 0xfff;
	tasks[i].stack[12] = 27;
	tasks[i].stack[13] = 0x202;
	tasks[i].stack[14] = 4096;
	tasks[i].stack[15] = 35;
	tasks[i].esp = tasks[i].stack;
}

uint64_t make_segment(const void *base, uint32_t limit, uint16_t attr)
{
	uint16_t d[4] = { limit, (uint32_t) base };
	((uint8_t *) d)[4] = (uint32_t) base >> 16;
	((uint8_t *) d)[7] = (uint32_t) base >> 24;
	((uint8_t *) d)[5] = attr;
	((uint8_t *) d)[6] = attr >> 8 | limit >> 16;
	return *(uint64_t *) d;
}

uint64_t make_gate(void f(void), uint8_t a)
{
	uint16_t d[4] = { (uint32_t) f, 8, 0, (uint32_t) f >> 16 };
	((uint8_t *) d)[5] = a;
	return *(uint64_t *) d;
}

void _start(void)
{
	for (uint32_t i = 1; i < sizeof(tasks) / sizeof(tasks[0]); ++i)
		init_task(i);

	static uint64_t idt[256], gdt[6] = { 0,
		0xcf98000000ffff,
		0xcf92000000ffff,
		0xcff8000000ffff,
		0xcff2000000ffff
	};

	gdt[5] = make_segment(&tss, 103, 0x89);
	idt[0x20] = make_gate(clock, 0x8e);
	idt[0x80] = make_gate(getid, 0xee);
	idt[0x81] = make_gate(print, 0xee);

	uint16_t gdtr[3] = { sizeof(gdt) - 1 };
	*(uint64_t **) (gdtr + 1) = gdt;
	uint16_t idtr[3] = { sizeof(idt) - 1 };
	*(uint64_t **) (idtr + 1) = idt;

	init(gdtr, idtr, 40);
	start();
}
