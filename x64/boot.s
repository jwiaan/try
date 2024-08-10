	org 0x7c00
	cli

	mov al, 0xff
	out 0x21, al
	out 0xa1, al

	in al, 0x92
	or al, 2
	out 0x92, al

	push dword 0
	push dword 1
	push dword 0x7e00
	push dword 0x10010
	mov si, sp
	call read

	mov word[si+8], 9
	mov word[si+4], 0x8000
	mov word[si+2], 2
	call read

	lgdt [gdt]
	mov eax, cr0
	bts eax, 0
	mov cr0, eax
	jmp 24:.32

read:	mov dl, 0x80
	mov ah, 0x42
	int 0x13
	ret

gdt:	dw 31
	dd gdt+6
	dq 0
	dq 0x20980000000000
	dq 0xcf92000000ffff
	dq 0xcf98000000ffff

	bits 32
.32:	mov ax, 16
	mov ds, ax
	mov es, ax
	mov ss, ax

	xor eax, eax
	xor edi, edi
	mov ecx, 0x1000
	rep stosd

	mov dword[0x0000], 0x1007
	mov dword[0x0800], 0x1007
	mov dword[0x1000], 0x2007
	mov dword[0x2000], 0x0087
	mov dword[0x1ff8], 0x3007
	mov dword[0x3ff8], 0x4007
	mov dword[0x4ff8], 0xfee00007

	mov cr3, eax
	mov eax, cr4
	bts eax, 5
	mov cr4, eax

	mov ecx, 0xc0000080
	rdmsr
	bts eax, 8
	wrmsr

	mov eax, cr0
	bts eax, 31
	mov cr0, eax
	jmp 8:.64

	bits 64
.64	mov rax, 0xffff807ffffff000
	mov dword[rax+0x320], 0x20020
	mov dword[rax+0x380], 0x10000
	mov dword[rax+0x3e0], 0xb

	mov rax, 0xffff800000000000
	mov rsp, 0x8000
	add rsp, rax
	mov rbx, 0x7e00
	add rax, [rbx+24]
	jmp rax

	times 510-($-$$) db 0
	dw 0xaa55
