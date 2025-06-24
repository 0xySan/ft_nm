/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quick_sort.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etaquet <etaquet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 13:02:57 by etaquet           #+#    #+#             */
/*   Updated: 2025/06/24 23:12:24 by etaquet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"

void	swap(t_info *a, t_info *b)
{
	t_info	tmp;
	
	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	str_lower_cmp(const char *a, const char *b)
{
	const char	*orig_a = a;
	const char	*orig_b = b;
	char		ca, cb;

	int leading_unders_a = 0;
	int leading_unders_b = 0;
	while (*a == '_')
	{
		leading_unders_a++;
		a++;
	}
	while (*b == '_')
	{
		leading_unders_b++;
		b++;
	}
	const char *fa = a;
	const char *fb = b;
	while (*fa && *fb)
	{
		while (*fa == '_' || *fa == '@' || *fa == '.')
			fa++;
		while (*fb == '_' || *fb == '@' || *fb == '.')
			fb++;
		ca = tolower((unsigned char)*fa);
		cb = tolower((unsigned char)*fb);
		if (!ca || !cb)
			break;
		if (ca != cb)
			return ca - cb;
		fa++;
		fb++;
	}
	while (*fa == '_' || *fa == '@' || *fa == '.')
		fa++;
	while (*fb == '_' || *fb == '@' || *fb == '.')
		fb++;
	if (!*fa && !*fb)
	{
		if (leading_unders_a != leading_unders_b)
			return leading_unders_b - leading_unders_a;
		return strcmp(orig_a, orig_b);
	}
	return tolower((unsigned char)*fa) - tolower((unsigned char)*fb);
}


int	partition(t_info *info, int low, int high)
{
	char	*pivot;
	int		i;
	int		j;

	pivot = info[high].name;
	i = low - 1;
	j = low - 1;
	while (++j < high)
	{
		if (str_lower_cmp(info[j].name, pivot) <= 0)
		{
			i++;
			swap(&info[i], &info[j]);
		}
	}
	swap(&info[i + 1], &info[high]);
	return (i + 1);
}

void	quicksort(t_info *info, int low, int high)
{
	int	pi;

	if (low < high)
	{
		pi = partition(info, low, high);
		quicksort(info, low, pi - 1);
		quicksort(info, pi + 1, high);
	}
}