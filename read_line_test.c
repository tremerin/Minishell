/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line_test.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgalan-r <fgalan-r@student.42malaga.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 18:52:50 by fgalan-r          #+#    #+#             */
/*   Updated: 2022/11/27 18:52:52 by fgalan-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(void)
{
	char	*input;
	int		exe;
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("Current dir: %s\n", cwd);
	}
	exe = 1;
	while (exe)
	{
		input = readline("MiniHell> ");
		if (ft_strlen(input) == 4)
			if (ft_memcmp("exit", input, 4) == 0)
				exe = 0;
		add_history(input);
		printf("%s\n", input);
	}
	return (0);
}

/* para compilar readline: -lreadline */
/* readline: lee la linea */
/* add_history: almacena la linea */
/* getcwd: ruta absoluta actual */
