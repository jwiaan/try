id	equ 0xfee00020
svr	equ 0xfee000f0
icr	equ 0xfee00300

	org 0x7c00
	jmp 0:start
start	cli
	lgdt [gdtr]
	mov eax,cr0
	or eax,1
	mov cr0,eax
	jmp code-gdt:init

	bits 32
init	mov ax,data-gdt
	mov ds,ax
	mov es,ax

	mov ebx,[id]
	shr ebx,24
	jnz ap

	mov ecx,start-$$
	mov esi,$$
	mov edi,0x8000
	cld
	rep movsb

	or dword[svr],0x100
	mov dword[icr],0xc4500
	mov dword[icr],0xc4608

ap	mov eax,160
	mul ebx,
	add eax,0xb8000
show	inc byte[eax]
	jmp show

gdt	dq 0
code	dq 0xcf98000000ffff
data	dq 0xcf92000000ffff
gdtr	dw gdtr-gdt-1
	dd gdt
	times 510-($-$$) db 0
	dw 0xaa55
