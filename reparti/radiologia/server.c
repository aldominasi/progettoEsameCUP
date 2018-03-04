#include "mywrapper.h"
#include "pacchetti.h"
#include "disponibilita.h"
#include <sys/types.h>
#include <fcntl.h>

#define ROW 45
#define COL 31

int main(int argc, char *argv[])
{
	int i,fd_file,j, n;
	struct disponibilita lista_disponibilita[ROW];
	char buff;
	if((fd_file = open("disponibilita.csv",O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	for(i=0;i<ROW;i++)
	{
		j=0;
		read(fd_file,&buff,1);
		while(buff != ';')
		{
			lista_disponibilita[i].prestazione[j] = buff;
			read(fd_file,&buff,1);
			j++;
		}
		lista_disponibilita[i].prestazione[j] = '\0';
		read(fd_file,&buff,1);
		j=0;
		while(buff != ';')
		{
			lista_disponibilita[i].data[j] = buff;
			read(fd_file,&buff,1);
			j++;
		}
		lista_disponibilita[i].data[j] = '\0';
		read(fd_file,&buff,1);
		j=0;
		while(buff != ';')
		{
			lista_disponibilita[i].orario[j]  = buff;
			read(fd_file,&buff,1);
			j++;
		}
		lista_disponibilita[i].orario[j] = '\0';
		read(fd_file,&buff,1);
		j=0;
		while(buff != '\n')
		{
			if(buff != ';')
				lista_disponibilita[i].disponibile = buff;
			read(fd_file,&buff,1);
			j++;
		}
		fprintf(stdout,"%s\t%s\t%s\t%c\n",lista_disponibilita[i].prestazione,lista_disponibilita[i].data,lista_disponibilita[i].orario,lista_disponibilita[i].disponibile);
	}
	close(fd_file);
	exit(0);
}
