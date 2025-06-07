/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etaquet <etaquet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:54:26 by etaquet           #+#    #+#             */
/*   Updated: 2025/06/07 14:11:00 by etaquet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
#define FT_NM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <ctype.h>

typedef struct s_elf64
{
	Elf64_Ehdr  *elf;
	Elf64_Shdr  *sections;
	Elf64_Shdr  *symtab;
	Elf64_Shdr  *symname;
}	t_elf64;

typedef struct s_elf32
{
	Elf32_Ehdr  *elf;
	Elf32_Shdr  *sections;
	Elf32_Shdr  *symtab;
	Elf32_Shdr  *symname;
}	t_elf32;

typedef struct s_info
{
	char *name;
	char *address;
	char symbol;
}	t_info;

typedef struct s_all
{
	t_elf32	*elf32;
	t_elf64	*elf64;
	t_info	*info;
	int section_nb;
}	t_all;

void	quicksort(t_info *info, int low, int high);

#endif