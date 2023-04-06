#include "./Libft/libft.h"
#include <stdio.h>

typedef struct s_mini
{
	char    **env;		 //varialbes de entorno
	char	**var;		 //variables locales
	int     env_len;	 //len de env
	int		var_len;	 //len de var (debe comenzar en 0)
	char	*p_exit;     //valor devuleto al finalizar programa
} t_mini;

void free_split(char **split)
{
	int		i;

	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

/* recive el nombre de una variable y devuelve su contenido */
char    *expand_env(char *name_var, char **env)
{
	char    *var;
	char    *tmp;
	int     i;

	i = 0;
	tmp = ft_strjoin(name_var, "=");
	while (env[i])
	{
		var = ft_strnstr(env[i], tmp, ft_strlen(tmp));
		if (var)
		{
			var = ft_strdup(env[i] + ft_strlen(tmp));
			break ;
		}
		i++;
	}
	free(tmp);
	return (var);
}

/* Devuelve el nombre de la variable dentro de la string */
char	*name_var(char *env_var)
{
	char	*name;
	int		i;

	i = 0;
	while (env_var[i] != '=')
		i++;
	name = malloc(sizeof(char) * i + 1);
	i = 0;
	while (env_var[i] != '=')
	{
		name[i] = env_var[i];
		i++;
	}
	name[i] = '\0';
	return (name);
}

/* Crea una copia de **envp para env y var */
void	init_env(t_mini *mini, char **envp)
{
	mini->env_len = 0;
	while (envp[mini->env_len])
		mini->env_len++;
	mini->env = malloc(sizeof(char *) * (mini->env_len + 1) + 1);
	mini->var = malloc(sizeof(char *) * (mini->env_len + 1) + 1);
	mini->env_len = 0;
	while (envp[mini->env_len])
	{
		mini->env[mini->env_len] = ft_strdup(envp[mini->env_len]);
		mini->var[mini->env_len] = ft_strdup(envp[mini->env_len]);
		mini->env_len++;
	}
	mini->env[mini->env_len] = NULL;
	mini->var[mini->env_len] = NULL;
	mini->var_len = mini->env_len;
}

/* Imprime las variables de env*/
void	print_env(t_mini *mini)
{
	int		i;

	i = 0;
	while (mini->env[i])
	{
		printf("var[%d]: %s\n", i, mini->env[i]);
		i++;
	}
}

/* Imprime las variables de var*/
void	print_var(t_mini *mini)
{
	int		i;

	i = 0;
	while (mini->var[i])
	{
		printf("var[%d]: %s\n", i, mini->var[i]);
		i++;
	}
}

/* Añade una variable a env */
void	add_env(t_mini *mini, char *env_var)
{
	char	**new;
	char	*name;
	int		i;

	i = 0;
	name = name_var(env_var);
	if (expand_env(name, mini->env) == NULL)
	{
		new = malloc(sizeof(char *) * (mini->env_len + 2) + 1);
		while(mini->env[i])
		{
			new[i] = ft_strdup(mini->env[i]);
			i++;
		}
		new[i] = ft_strdup(env_var);
		i++;
		new[i] = NULL;
		free_split(mini->env);
		mini->env = new;
		mini->env_len += 1;
	}
	free(name);
}

int	contain_var(t_mini *mini, char *env_var ,int i)
{
	char	*name;
	char	*tmp;

	tmp = name_var(mini->env[i]);
	name = ft_strjoin(tmp, "=");
	free(tmp);
	tmp = ft_strjoin(env_var, "=");
	if (ft_strnstr(name, tmp, ft_strlen(tmp)) == NULL)
	{
		free(tmp);
		free(name);
		return (0);
	}
	free(tmp);
	free(name);
	return (1);
}

/* Borra una  variable normal o de entorno en env (env_var sera el solo nombre de la variable)*/
void	del_env(t_mini *mini, char *var_name)
{
	char	**new;
	char	*expand;
	int		i;
	int		j;

	i = 0;
	j = 0;
	expand = expand_env(var_name, mini->env);
	if (expand)
	{
		free(expand);
		new = malloc((sizeof(char *) * mini->env_len) + 1);
		while (mini->env[i])
		{
			if (!contain_var(mini, var_name, i))
			{
				new[j] = ft_strdup(mini->env[i]);
				j++;
				i++;
			}
			else
				i++;
		}
		new[j] = NULL;
		free_split(mini->env);
		mini->env = new;
	}
}

/* Cambia el contenido de una  variable */
void	change_value(t_mini *mini, char *new_value)
{
	int		i;
	char	*name;
	char	*expand;

	i = 0;
	name = name_var(new_value);
	expand = expand_env(name, mini->env);
	if (expand)
	{
		free(expand);
		while (mini->env[i])
		{
			if (contain_var(mini, name, i))
			{
				free(mini->env[i]);
				mini->env[i] = ft_strdup(new_value);
			}
			i++;
		}
	}
	free(name);
}

/* dada una variable y su valor la añade o actualiza a un nuevo valor */
void enter_var(t_mini *mini, char *enter_var)
{
	char	*name;
	char	*expand;

	name = name_var(enter_var);
	expand = expand_env(name, mini->env);
	if (expand)
	{
		change_value(mini, enter_var);
		free(expand);
	}
	else
		add_env(mini, enter_var);
	free(name);
}

int main(int argc, char **argv, char **envp)
{
	char    *str;
	int     *len;
	char    **split;
	t_mini	mini;

	//atexit(ft_void);
	init_env(&mini, envp);
	mini.var_len = 0;
	str = expand_env(argv[1], mini.env);
	if (str)
		printf("%s\n", str);
	//print_env(&mini);
	//printf("-----------------------------------------\n");
	//printf("len: %d\n", mini.env_len);
	enter_var(&mini, "VAR1=hola");
	enter_var(&mini, "VAR2=adios me voy");
	enter_var(&mini, "VAR3=holita");
	print_env(&mini);
	printf("-----------------------------------------\n");
	enter_var(&mini, "VAR1=adios");
	enter_var(&mini, "VAR2=ya me fui");
	enter_var(&mini, "VAR4=patata");
	//enter_var(&mini, "VAR5=patata");
	//enter_var(&mini, "VAR6=patata");
	//del_env(&mini, "VAR2");
	//del_env(&mini, "USER");
	//del_env(&mini, "HOME");
	//del_env(&mini, "_");
	print_env(&mini);
	printf("-----------------------------------------\n");
	// str = expand_env("VAR1", mini.env);
	// if (str)
	//  	printf("%s\n", str);
	//free_split(mini.env);
	//enter_var(&mini, "VAR1=hola");
	//print_env(&mini);
	return (0);
}



void	ft_void(void)
{
	system("leaks -q a.out");
}
