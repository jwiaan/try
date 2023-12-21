global	star
global	time
global	user

star:	push rax
	push rcx
	push rdx

	mov ecx, 0xc0000080
	rdmsr
	bts eax, 0
	wrmsr

	mov ecx, 0xc0000081
	xor eax, eax
	mov edx, 0x001b0008
	wrmsr

	mov ecx, 0xc0000082
	mov rdx, 0xffff800000000000
	add rdx, call
	mov rax, rdx
	shr rdx, 32
	wrmsr

	mov ecx, 0xc0000084
	xor edx, edx
	mov eax, 0x47700
	wrmsr

	pop rdx
	pop rcx
	pop rax
	ret

time:	push rax
	mov rax, 0xffff8000000b80a0
	mov byte[rax+1], 0xb
	inc byte[rax]
	mov rax, 0xffff807ffffff000
	mov dword[rax+0xb0], 0
	pop rax
	iretq

call:	mov rax, 0xffff8000000b8000
	mov byte[rax+1], 0xa
	inc byte[rax]
	o64 sysret

loop:	syscall
	jmp loop

user:	mov rax, loop
	push 35
	push 0xffff800000007000
	pushfq
	push 43
	push rax
	iretq
