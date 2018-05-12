#include "lib.h"
#include "frdwr.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

const char *FILEDBREPARTI = "reparti.csv";

void read_from_db_reparti(struct reparti **lista, int *rows)
{
	int i, fd_file, j, n;
	char buff;
	struct reparti *lista_reparti;
	if((fd_file = open(FILEDBREPARTI, O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	*rows = count_rows(fd_file);
	lseek(fd_file,0L,SEEK_SET);
	if(*rows > 0)
	{
		lista_reparti = (struct reparti *)malloc(*rows * sizeof(struct reparti));
		for(i=0;i<*rows;i++)
		{
			j=0;
			read(fd_file,&buff,1);
			while(buff != ';')
			{
				lista_reparti[i].reparto[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_reparti[i].reparto[j]='\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != '\n')
			{
				if(buff != ';')
					lista_reparti[i].porta[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
		}
	}
	*lista = lista_reparti;
	close(fd_file);
}

int count_rows(int fd)
{
	char buff;
	int count = 0;
	while(read(fd,&buff,1) > 0)
	{
		if(buff == '\n')
			count++;
	}
	return count;
}
