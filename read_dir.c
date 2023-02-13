#include <stdio.h>
#include <string.h>
#include <dirent.h>

int	list_dir(const char *str)
{
	DIR *dir = opendir(str);

	if (dir == NULL)
		return (1);
	struct dirent *entity;
	entity = readdir(dir);
	while (entity != NULL)
	{
		printf("%d %s\n", entity->d_type ,entity->d_name);
		entity = readdir(dir);	
	}
	closedir(dir);
	return (0);
}

int main(int argc, char **argv)
{
	list_dir(".");
	return (0);
}


//struct dirent
//{
//  long            d_ino;		          /* Always zero. */
//  unsigned short  d_reclen;		      /* Always sizeof struct dirent. */
//  unsigned short  d_namlen;		      /* Length of name in d_name. */
//  unsigned        d_type;		          /* File attributes */
//  char            d_name[FILENAME_MAX]; /* File name. */
//};
