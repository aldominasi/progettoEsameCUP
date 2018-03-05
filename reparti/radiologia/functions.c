#include "disponibilita.h"
#include "mywrapper.h"
#include <string.h>

const char *FILEDB = "disponibilita.csv";

void loadDB(struct disponibilita *lista_disponibilita)
{
	int i, fd_file, j, n;
	char buff;
	if((fd_file = open(FILEDB, O_RDONLY)) == -1)
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
			read(fd_file, &buff,1);
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
			lista_disponibilita[i].orario[j] = buff;
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
	}
	close(fd_file);
}

int conferma_appuntamento(struct disponibilita *lista_disponibilita)
{
	int fd_file,i;
	char buff[PRESTAZIONE];
	if((fd_file = open(FILEDB,O_WRONLY)) == -1)
	{
		perror("open");
		return 0;
	}
	for(i = 0;i<ROW;i++)
	{
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].prestazione);
		FullWrite(fd_file,buff,strlen(lista_disponibilita[i].prestazione)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].data);
		FullWrite(fd_file,buff,strlen(lista_disponibilita[i].data)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].orario);
		FullWrite(fd_file,buff,strlen(lista_disponibilita[i].orario)+1);
		snprintf(buff,sizeof(buff),"%c\n",lista_disponibilita[i].disponibile);
		FullWrite(fd_file,buff,2);
	}
	close(fd_file);
	return 1;
}
