/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 22:58:17 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define COMPLETE_METACHARACTERS "|&;()<> \t"
#define METACHARACTERS "|;<> \t"
#define CTRL_OPERATORS "\n"

int	got_metacharacter(void);

int	got_quotes(int add_single, int add_double, int reinit)
{
	static int	s_quote = 0;
	static int	d_quote = 0;

	if (reinit)
	{
		s_quote = 0;
		d_quote = 0;
	}
	s_quote += add_single;
	d_quote += add_double;
	return ((s_quote % 2 == 1) || (d_quote % 2 == 1));
}

static int	found_str_limit(char *s, size_t i, size_t j)
{
	got_quotes((s[j] == '\''), (s[j] == '"'), 0);
	if (!got_quotes(0, 0, 0) && (s[j] == '|' || s[j] == ';'))
	{
		ft_lstadd_back(&singleton()->lst, ft_lstnew(ft_substr(s, i, j - i)));
		return (1);
	}
	else if (!got_quotes(0, 0, 0) && !s[j + 1])
	{
		ft_lstadd_back(&singleton()->lst, ft_lstnew(ft_substr(s, i, j + 1)));
		return (1);
	}
	return (0);
}

void	ft_parse(char *s)
{
	size_t	i;
	size_t	j;

	ft_lstclear(&singleton()->lst, free);
	got_quotes(0, 0, 1);
	i = 0;
	while (s[i])
	{
		j = i;
		while (s[j])
		{
			if (found_str_limit(s, i, j))
				break ;
			++j;
		}
		i = j + 1;
	}
}
