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

/* Cabeceras Built-in */

/* Imprime las varialbes de entorn */
int		env(t_mini *mini);

/* Añade en env una variable que exista en var */
int		export(t_mini *mini, char *var_name);

/* Borra una variable de las variables locales y de entorno */
int		unset(t_mini *mini, char *var_name);

/* Recive el nombre de una variable y devuelve su contenido */
char    *expand_var(char *name_var, char **env);

/* Añana de una variable a var */
void 	enter_var(t_mini *mini, char *enter_var);

/* Devuelve el nombre de la variable dentro de una cadena */
char	*name_var(char *env_var);

/* Crea una copia de **envp original para iniciar env y var */
void	init_env(t_mini *mini, char **envp);

/* Añade un nuevo char *str a un char **src */
char	**add_str(char **src, char *str, int *len);

/* Comprueba que la variable existe en la posicion i del char **src */
int		contain_var(char **src, char *env_var ,int i);

/* Borra un char *var_name de un char **src */
char	**del_var(char **src, char *var_name, int len);

/* Busca en el char **src una variable con el mismo nombre que  
char *new_value y cambia el contenido */
void	change_value(char **src, char *new_value);

/* Imprime las variables de env y var. SOLO DEBUG */ 
void	print_env(t_mini *mini);


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
char    *expand_var(char *name_var, char **env)
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

/* Imprime las variables de env y var. SOLO DEBUG */ 
void	print_env(t_mini *mini)
{
	int		i;

	i = 0;
	printf("-------------- env var: \n");
	while (mini->env[i])
	{
		printf("var[%d]: %s\n", i, mini->env[i]);
		i++;
	}
	printf("-------------- local var: \n");
	i = 0;
	while (mini->var[i])
	{
		printf("var [%d]: %s\n", i, mini->var[i]);
		i++;
	}
}

/* Imprime las varialbes de entorn */
int	env(t_mini *mini)
{
	int	i;

	i = 0;
	while (mini->env[i])
	{
		printf("%s\n",  mini->env[i]);
		i++;
	}
	return (0);
}

/* Añada un nuevo *char a un **char */
char	**add_str(char **src, char *str, int *len)
{
	char	**new;
	int		i;

	i = 0;
	new = malloc(sizeof(char *) * (*len + 2) + 1);
	while(src[i])
	{
		new[i] = ft_strdup(src[i]);
		i++;
	}
	new[i] = ft_strdup(str);
	i++;
	new[i] = NULL;
	free_split(src);
	*len += 1;
	return (new);
}

/* Comprueba que la variable existe en la posicion i del char **src */
int	contain_var(char **src, char *env_var ,int i)
{
	char	*name;
	char	*tmp;

	tmp = name_var(src[i]);
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

/* Borra un char *var_name de un char **src */
char	**del_var(char **src, char *var_name, int len)
{
	char	**new;
	int		i;
	int		j;

	i = 0;
	j = 0;
	new = malloc((sizeof(char *) * len) + 1);
	while (src[i])
	{
		if (!contain_var(src, var_name, i))
		{
			new[j] = ft_strdup(src[i]);
			j++;
			i++;
		}
		else
			i++;
	}
	new[j] = NULL;
	free_split(src);
	return (new);
}

/* Borra una variable de las variables locales y de entorno */
int	unset(t_mini *mini, char *var_name)
{
	char	*expand;

	if (expand = expand_var(var_name, mini->var))
	{
		free(expand);
		mini->var = del_var(mini->var, var_name, mini->var_len);
		mini->var_len--;
		if (expand = expand_var(var_name, mini->env))
		{
			free(expand);
			mini->env = del_var(mini->env, var_name, mini->env_len);
			mini->env_len--;
		}
		return (0);
	}
	return (1);
}

/* Busca en el char **src una variable con el mismo nombre que  
char *new_value y cambia el contenido */
void	change_value(char **src, char *new_value)
{
	int		i;
	char	*name;
	char	*expand;

	i = 0;
	name = name_var(new_value);
	expand = expand_var(name, src);
	if (expand)
	{
		free(expand);
		while (src[i])
		{
			if (contain_var(src, name, i))
			{
				free(src[i]);
				src[i] = ft_strdup(new_value);
			}
			i++;
		}
	}
	free(name);
}

/* Añade en env una variable que exista en var */
int	export(t_mini *mini, char *var_name)
{
	char	*expand;
	char	*enter_var;
	int		i;

	i = 0;
	if (expand = expand_var(var_name, mini->var))
	{
		free (expand);
		while (mini->var[i])
		{
			if (contain_var(mini->var, var_name, i))
				enter_var = ft_strdup(mini->var[i]);
			i++;
		}
		mini->env = add_str(mini->env, enter_var, &mini->env_len);
		free(enter_var);
		return (0);
	}
	return (1);
}

/* Añana de una variable a var */
void enter_var(t_mini *mini, char *enter_var)
{
	char	*name;
	char	*expand;

	name = name_var(enter_var);
	if (expand = expand_var(name, mini->var))
	{
		change_value(mini->var, enter_var);
		free(expand);
		if (expand = expand_var(name, mini->env))
		{
			change_value(mini->env, enter_var);
			free(expand);
		}
	}
	else
		mini->var = add_str(mini->var, enter_var, &mini->var_len);
	free(name);
}

/* Main para pruebas */
void	ft_void(void)
{
	system("leaks -q a.out");
}

int main(int argc, char **argv, char **envp)
{
	char    *str;
	t_mini	mini;

	//atexit(ft_void);
	init_env(&mini, envp);
	str = expand_var(argv[1], mini.env);
	if (str)
	{
		printf("%s\n", str);
		free(str);
	}
	enter_var(&mini, "VAR2=adios me voy");
	enter_var(&mini, "VAR1=hola");
	enter_var(&mini, "VAR3=holita");
	export(&mini, "VAR3");
	env(&mini);
	printf("-----------------------------------------\n");
	enter_var(&mini, "VAR1=adios");
	enter_var(&mini, "VAR2=ya me fui");
	enter_var(&mini, "VAR4=patatas");
	export(&mini, "VAR4");
	enter_var(&mini, "VAR6=patatas");
	enter_var(&mini, "VAR1=patata");
	export(&mini, "VAR1");
	unset(&mini, "USER");
	unset(&mini, "VAR4");
	env(&mini);
	printf("-----------------------------------------\n");
	str = expand_var("VAR1", mini.env);
	if (str)
	{
		printf("%s\n", str);
		free(str);
	}
	if (mini.env_len > 0)
		free_split(mini.env);
	if (mini.var_len > 0)
		free_split(mini.var);
	return (0);
}

/* Borra una  variable normal o de entorno en env (env_var sera el solo nombre de la variable)*/
// void	del_env(t_mini *mini, char *var_name)
// {
// 	char	**new;
// 	char	*expand;
// 	int		i;
// 	int		j;

// 	i = 0;
// 	j = 0;
// 	expand = expand_var(var_name, mini->env);
// 	if (expand)
// 	{
// 		free(expand);
// 		new = malloc((sizeof(char *) * mini->env_len) + 1);
// 		while (mini->env[i])
// 		{
// 			if (!contain_var(mini->env, var_name, i))
// 			{
// 				new[j] = ft_strdup(mini->env[i]);
// 				j++;
// 				i++;
// 			}
// 			else
// 				i++;
// 		}
// 		new[j] = NULL;
// 		free_split(mini->env);
// 		mini->env = new;
// 	}
// }

// char	**add_str(char **src, char *str, int *len)
// {
// 	char	**new;
// 	char	*name;
// 	int		i;

// 	i = 0;
// 	name = name_var(str);
// 	new = NULL;
// 	if (expand_var(name, src) == NULL)
// 	{
// 		new = malloc(sizeof(char *) * (*len + 2) + 1);
// 		while(src[i])
// 		{
// 			new[i] = ft_strdup(src[i]);
// 			i++;
// 		}
// 		new[i] = ft_strdup(str);
// 		i++;
// 		new[i] = NULL;
// 		free_split(src);
// 		*len += 1;
// 	}
// 	free(name);
// 	return (new);
// }

// void	init_var(t_mini *mini, char *local_var)
// {
// 	mini->var = malloc(sizeof(char*) * 2);
// 	mini->var[0] = ft_strdup(local_var); //revisar como llega local_var
// 	mini->var[1] = NULL;
// 	mini->var_len = 1;
// }

// /* dada una variable y su valor la añade o actualiza a un nuevo valor en var*/
// void enter_var(t_mini *mini, char *enter_var)
// {
// 	char	*name;
// 	char	*expand;

// 	if (mini->var_len == 0)
// 		init_var(mini, enter_var);
// 	else
// 	{
// 		name = name_var(enter_var);
// 		expand = expand_var(name, mini->var);
// 		if (expand)
// 		{
// 			change_value(mini->var, enter_var);
// 			free(expand);
// 			expand = expand_var(name, mini->env);
// 			if (expand)
// 			{
// 				change_value(mini->env, enter_var);
// 				free(expand);
// 			}
// 		}
// 		else
// 			mini->var = add_str(mini->var, enter_var, &mini->var_len);
// 		free(name);
// 	}
// }

/* dada una variable y su valor la añade o actualiza a un nuevo valor en env*/
// void enter_env(t_mini *mini, char *enter_var)
// {
// 	char	*name;
// 	char	*expand;

// 	name = name_var(enter_var);
// 	expand = expand_var(name, mini->env);
// 	if (expand)
// 	{
// 		change_value(mini->env, enter_var);
// 		free(expand);
// 	}
// 	else
// 		mini->env = add_str(mini->env, enter_var, &mini->env_len);
// 	free(name);
// }