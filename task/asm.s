extern	task
extern	schedule

global	init
global	clock
global	reset
global	getid
global	print
global	start

init	mov eax, [esp+4]
	lgdt [eax]

	mov eax, [esp+8]
	lidt [eax]

	ltr [esp+12]

	mov al, 0x11
	out 0x20, al
	out 0xa0, al

	mov al, 0x20
	out 0x21, al
	mov al, 0x28
	out 0xa1, al

	mov al, 0x04
	out 0x21, al
	mov al, 0x02
	out 0xa1, al

	mov al, 0x01
	out 0x21, al
	out 0xa1, al

	mov al, 0xfe
	out 0x21, al
	mov al, 0xff
	out 0xa1, al

	sti
	ret

clock	pusha
	push ds
	push es
	mov eax, cr3
	push eax
	call schedule
	mov esp, eax

	mov al, 0x20
	out 0x20, al

	pop eax
	mov cr3, eax
	pop es
	pop ds
	popa
	iret

reset	mov eax, cr3
	mov cr3, eax
	ret

getid	mov eax, [task]
	mov eax, [eax]
	iret

print	mov [eax], dx
	iret

start	int 0x80
	mov edx, 160
	mul edx
	add eax, 0x800b8000
	mov dh, 0xa
start1	int 0x81
	inc dl
	jmp start1
