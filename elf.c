#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <elf.h>

int main(int c, char **v)
{
	assert(c == 2);
	int fd = open(v[1], O_RDONLY);
	assert(fd != -1);

	Elf64_Ehdr e;
	ssize_t n = read(fd, &e, sizeof(e));
	assert(n == sizeof(e));
	printf("entry=%lx\nphoff=%lu\nphentsize=%hu\nphnum=%hu\n\n",
	       e.e_entry, e.e_phoff, e.e_phentsize, e.e_phnum);

	off_t off = lseek(fd, e.e_phoff, SEEK_SET);
	assert(off == e.e_phoff);

	for (Elf64_Half i = 0; i < e.e_phnum; i++) {
		Elf64_Phdr p;
		n = read(fd, &p, sizeof(p));
		assert(n == sizeof(p));
		if (p.p_type == 1)
			printf("offset=%lx\nvaddr=%lx\nfilesz=%lx\n\n",
			       p.p_offset, p.p_vaddr, p.p_filesz);
	}

	printf("entry=%zu\nphoff=%zu\nphentsize=%zu\nphnum=%zu\n\n",
	       offsetof(Elf64_Ehdr, e_entry), offsetof(Elf64_Ehdr, e_phoff),
	       offsetof(Elf64_Ehdr, e_phentsize), offsetof(Elf64_Ehdr,
							   e_phnum));

	printf("type=%zu\noffset=%zu\nvaddr=%zu\nfilesz=%zu\n\n",
	       offsetof(Elf64_Phdr, p_type), offsetof(Elf64_Phdr, p_offset),
	       offsetof(Elf64_Phdr, p_vaddr), offsetof(Elf64_Phdr, p_filesz));
}
