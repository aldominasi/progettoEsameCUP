#include "disponibilita.h"
#include "frdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char *FILEDB = "disponibilita.csv";

void read_from_db(struct disponibilita *lista_disponibilita)
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

int write_into_db(struct disponibilita *lista_disponibilita)
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

void invia_prestazioni_erogabili(int sock,struct disponibilita *lista_disponibilita)
{
	char prestazioni[N_PRESTAZIONI][PRESTAZIONE],temp[PRESTAZIONE];
	int i,j=0, count = N_PRESTAZIONI, length;
	strcpy(temp,lista_disponibilita[0].prestazione);
	strcpy(prestazioni[0],lista_disponibilita[0].prestazione);
	for(i=1;i<ROW;i++)
	{
		if(strcmp(temp, lista_disponibilita[i].prestazione) != 0)
		{
			strcpy(temp,lista_disponibilita[i].prestazione);
			j++;
			strcpy(prestazioni[j],lista_disponibilita[i].prestazione);
		}
	}
	FullWrite(sock,&count,sizeof(int));
	for(i=0;i<count;i++)
	{
		length = (int)strlen(prestazioni[i]);
		FullWrite(sock,&length,sizeof(int));
		FullWrite(sock,prestazioni[i],length);
	}
}

void invia_date_disponibili(int sock, struct disponibilita *lista_disponibilita, char *prestazione)
{
	int i,count=0;
	for(i=0;i<ROW;i++)
		if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && lista_disponibilita[i].disponibile == '1')
			count += 1;
	FullWrite(sock,&count,sizeof(int));
	for(i=0;i<ROW;i++)
		if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && lista_disponibilita[i].disponibile == '1')
			FullWrite(sock,&lista_disponibilita[i],sizeof(struct disponibilita));
}
