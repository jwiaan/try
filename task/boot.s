	org 0x7c00
	cli
	cld

	in al, 0x92
	or al, 2
	out 0x92, al

	lgdt [gdt]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword 8:init

gdt	dw 23
	dd gdt+6
	dq 0
	dq 0xcf98000000ffff
	dq 0xcf92000000ffff

	bits 32
init	mov ax, 16
	mov ds, ax
	mov es, ax
	mov ss, ax

	xor eax, eax
	xor edi, edi
	mov ecx, 2048
	rep stosd

	mov eax, 0x7003
	mov edi, 0x101c
page	stosd
	add eax, 0x1000
	cmp eax, 0xc0003
	jne page

	xor eax, eax
	mov dword[eax], 0x1003
	mov dword[eax+2048], 0x1003
	mov dword[eax+4092], 3
	mov cr3, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	mov esp, 0x80008000
	mov ebp, esp
	sub esp, 16

	push 1
	push ebp
	call read

	movzx eax, word[ebp+42]
	mov [ebp-4], eax
	movzx eax, word[ebp+44]
	mov [ebp-8], eax
	mov eax, [ebp+24]
	mov [ebp-12], eax
	mov ebx, [ebp+28]
	add ebx, ebp

load	cmp dword[ebx], 1
	jne load2

	xor edx, edx
	mov eax, [ebx+4]
	mov ecx, 512
	div ecx
	inc eax
	push eax
	push dword[ebx+8]

	mov eax, [ebx+16]
	add eax, 511
	div ecx
	mov [ebp-16], eax

load1	call read
	inc dword[esp+4]
	add dword[esp], 512
	dec dword[ebp-16]
	jnz load1

load2	add ebx, [ebp-4]
	dec dword[ebp-8]
	jnz load

	mov esp, ebp
	jmp [ebp-12]

read	mov dx, 0x1f2
	mov al, 1
	out dx, al

	inc dx
	mov eax, [esp+8]
	mov ecx, 3
read1	out dx, al
	inc dx
	shr eax, 8
	loop read1
	or al, 0xe0
	out dx, al

	inc dx
	mov al, 0x20
	out dx, al
read2	in al, dx
	and al, 0x88
	cmp al, 0x08
	jne read2

	mov dx, 0x1f0
	mov edi, [esp+4]
	mov ecx, 256
read3	in ax, dx
	stosw
	loop read3
	ret

	times 510-($-$$) db 0
	dw 0xaa55
