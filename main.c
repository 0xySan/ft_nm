/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etaquet <etaquet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:05:30 by etaquet           #+#    #+#             */
/*   Updated: 2025/06/07 15:16:50 by etaquet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include "ft_dprintf/ft_printf.h"
#include <elf.h>

char getSymbol64(t_all elf, Elf64_Sym *symbol)
{
	int stype;
	int sbind;
	int sec_flag;
	int sec_type;
	
	stype = ELF64_ST_TYPE(symbol->st_info);
	sbind = ELF64_ST_BIND(symbol->st_info);

	sec_flag = 0;
	sec_type = 0;
	if (symbol->st_shndx <= elf.section_nb)
	{
		sec_flag = elf.elf64->sections[symbol->st_shndx].sh_flags;
		sec_type = elf.elf64->sections[symbol->st_shndx].sh_type;
	}

	if (sbind == STB_WEAK && stype == STT_OBJECT)
		return (symbol->st_shndx == 0) ? 'v' : 'V';
	if (sbind == STB_WEAK)
		return (symbol->st_shndx == 0) ? 'w' : 'W';
	if (stype == STT_GNU_IFUNC)
		return 'i';
	if (stype == STT_FILE)
		return (sbind != STB_GLOBAL) ? 'a' : 'A';
	if (sec_type == SHT_NOBITS)
		return (sbind != STB_GLOBAL) ? 'b' : 'B';
	if (sec_flag == (SHF_MERGE | SHF_STRINGS) || (!sec_flag && symbol->st_shndx))
		return (sbind != STB_GLOBAL) ? 'n' : 'N';
	if (sec_flag & SHF_EXECINSTR)
		return (sbind != STB_GLOBAL) ? 't' : 'T';
	if (sec_flag == (SHF_WRITE | SHF_ALLOC))
		return (sbind != STB_GLOBAL) ? 'd' : 'D';
	if (sec_flag & SHF_ALLOC)
		return (sbind != STB_GLOBAL) ? 'r' : 'R';
	if (stype == STT_COMMON)
		return 'C';
	if (!symbol->st_shndx)
		return (sbind != STB_GLOBAL) ? 'u' : 'U';
	return '?';
}

void	itoa_hex(size_t value, char *buf)
{
	const char	*hex = "0123456789abcdef";
	size_t		i;
	size_t		tmp;

	i = 0;
	tmp = value;
	if (value == 0)
		buf[i++] = '0';
	while (tmp)
	{
		tmp /= 16;
		i++;
	}
	buf[i] = '\0';
	while (i--)
	{
		buf[i] = hex[value % 16];
		value /= 16;
	}
}

void printfFullAdress(char *address, char symbol)
{
	if (address && address[0] == '0' && symbol == 'a')
	{
		ft_dprintf(1, "0000000000000000");
		return ;
	}
	if (address && address[0] == '0')
	{
		ft_dprintf(1, "                ");
		return ;
	}
	size_t len = 17 - strlen(address);
	while (--len)
		write(1, "0", 1);
	ft_dprintf(1, "%s", address);
}

int getAndPrintElf(t_all elf, t_elf64 *elf64, int fd, size_t filesize, char *filename, int argc)
{
	void *mapped = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}
	elf64->elf = mapped;
	elf64->sections = (void *)((char *)mapped + elf64->elf->e_shoff);
	// Elf64_Shdr *strtab = &elf.sections[elf.elf->e_shstrndx];
	// printf("%d %lu\n", elf.elf->e_shnum, elf.elf->e_shoff);
	// for (int i = 0; i < elf.elf->e_shnum; i++)
	//     printf("%u\n", elf.elf->e_shstrndx);
	// write(1, (char *)elf.elf + strtab->sh_offset, strtab->sh_size);
	// write(1, "\n", 1);
	elf64->symname = &elf64->sections[elf64->elf->e_shstrndx - 1];
	elf64->symtab = &elf64->sections[elf64->elf->e_shstrndx - 2];
	Elf64_Sym *sym = (void *)elf64->elf + elf64->symtab->sh_offset + sizeof(Elf64_Sym);
	int symnb = elf64->symtab->sh_size / sizeof(Elf64_Sym) - 1;

	elf.info = malloc(sizeof(t_info) * (symnb + 1));
	
	elf.section_nb = symnb;
	
	for (int i = 0; i < symnb; i++)
	{
		// printf("more info = %u, %u, %u\n", sym[i].st_info, sym[i].st_shndx, symnb);
		// ft_dprintf(1, "symname = %x : %s\n", sym[i].st_value, (char *)elf64->elf + elf64->symname->sh_offset + sym[i].st_name);
		if (!sym)
			continue ;
		elf.info[i].name = strdup((char *)elf64->elf + elf64->symname->sh_offset + sym[i].st_name);
		elf.info[i].address = malloc(sizeof(char) * (16 + 1));
		elf.info[i].symbol = getSymbol64(elf, &sym[i]);
		// printf("%d, %d\n", symnb, sym[i].st_shndx);
		itoa_hex(sym[i].st_value, elf.info[i].address);
	}
	elf.info[symnb].name = NULL;
	elf.info[symnb].address = NULL;

	if (!elf.info[0].name)
		return (free(elf.info), munmap(mapped, filesize), 1);

	quicksort(elf.info, 0, symnb - 1);

	if (argc > 2)
		ft_dprintf(1, "\n%s:\n", filename);

	for (int i = 0; i < symnb; i++)
	{
		if (elf.info[i].symbol == 'a')
			continue ;
		printfFullAdress(elf.info[i].address, elf.info[i].symbol);
		dprintf(1, " %c %s\n", elf.info[i].symbol, elf.info[i].name);
	}

	for (int i = 0; i < symnb; i++)
	{
		free(elf.info[i].name);
		free(elf.info[i].address);
	}
	free(elf.info);

	if (munmap(mapped, filesize) == -1)
		perror("munmap");

	return 0;
}

int doAllFiles(int fd, char *filename, int argc)
{
	struct stat sb;
	if (fstat(fd, &sb) == -1)
	{
		perror("fstat");
		return (0);
	}

	size_t filesize = sb.st_size;

	t_all     elf;

	elf.elf64 = malloc(sizeof(Elf64_Shdr));
	if (getAndPrintElf(elf, elf.elf64, fd, filesize, filename, argc))
		return (free(elf.elf64), 1);
	free(elf.elf64);
	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	int i = 1;

	if (argc < 2)
		return (1);
	while (argv[i])
	{
		fd = open(argv[i], O_RDONLY);
		if (fd == -1)
			return (1);
		if (doAllFiles(fd, argv[i], argc))
			ft_dprintf(1, "%s: %s: no symbols\n", argv[0], argv[i]);
		close(fd);
		i++;
	}
	return 0;
}