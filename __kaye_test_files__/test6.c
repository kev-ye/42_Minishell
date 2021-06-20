/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test6.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/20 15:47:21 by kaye              #+#    #+#             */
/*   Updated: 2021/06/20 16:14:24 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libc.h>
#include "libft.h"

int main()
{
    char *s =  strdup("ll 1 lll 1 llll");
    char    *t;
    int ret = 1232138;
    int i =  0;
    while (s[i])
    {
        if (s[i] == '1')
        {
            printf("i : %d\n", i);
            ft_asprintf(&t, "%.*s%d%s", i , s, ret, s + i + 1);
            s = t;
            i += ft_nblen(ret);
        }
        else
            ++i;
    }
    printf("t : -> %s\n", t);
    printf("s : -> %s\n", s);
    // char *lrv = ft_itoa(ret);

    
}

// ll 1 lll 1 llll
// ll 128 lll 1 llll