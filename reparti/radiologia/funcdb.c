#include "disponibilita.h"
#include "frdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char *FILEDB = "disponibilita.csv";
const char *FILEDB_PRENOTAZIONI = "prenotazioni.csv";

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
	char buff[PRESTAZIONE+1];
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

int count_lines(int fd)
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

void read_from_db_prenotazioni(struct prenotazione **lista_prenotazioni, int *lines)
{
	int i, fd_file, j, n;
	char buff;
	if((fd_file = open(FILEDB_PRENOTAZIONI, O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	*lines = count_lines(fd_file);
	lseek(fd_file,0L,SEEK_SET);
	*lista_prenotazioni = (struct prenotazione *)malloc(*lines * sizeof(struct prenotazione));
	if(*lines > 0)
	{
		fprintf(stdout,"Totale Prenotazioni: %d\n",*lines);
		for(i=0;i<*lines;i++)
		{
			j=0;
			read(fd_file,&buff,1);
			while(buff != ';')
			{
				lista_prenotazioni[i]->assistito.nome[j] = buff;
				read(fd_file, &buff,1);
				j++;
			}
			lista_prenotazioni[i]->assistito.nome[j] = '\0';
			fprintf(stdout,"%s ",lista_prenotazioni[i]->assistito.nome);
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i]->assistito.cognome[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i]->assistito.cognome[j] = '\0';
			fprintf(stdout,"%s ",lista_prenotazioni[i]->assistito.cognome);
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i]->data_appuntamento[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i]->data_appuntamento[j] = '\0';
			fprintf(stdout,"%s ",lista_prenotazioni[i]->data_appuntamento);
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i]->codice_ricetta[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i]->codice_ricetta[j] = '\0';
			fprintf(stdout,"%s ",lista_prenotazioni[i]->codice_ricetta);
			read(fd_file,&buff,1);
			j=0;
			while(buff != '\n')
			{
				if(buff != ';')
					lista_prenotazioni[i]->codice_prenotazione[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			fprintf(stdout,"%s\n",lista_prenotazioni[i]->codice_prenotazione);
		}
	}
	close(fd_file);
}

int write_into_db_prenotazioni(struct prenotazione * lista_prenotazioni, int count)
{
	int fd_file,i;
	char buff[PRESTAZIONE+1];
	if((fd_file = open(FILEDB_PRENOTAZIONI,O_WRONLY)) == -1)
	{
		perror("open");
		return 0;
	}
	for(i = 0;i<count;i++)
	{
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].assistito.nome);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].assistito.nome)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].assistito.cognome);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].assistito.cognome)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].prestazione);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].prestazione)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].data_appuntamento);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].data_appuntamento)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].orario_appuntamento);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].orario_appuntamento)+1);
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].codice_ricetta);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].codice_ricetta)+1);
		snprintf(buff,sizeof(buff),"%s\n",lista_prenotazioni[i].codice_prenotazione);
		FullWrite(fd_file,buff,strlen(lista_prenotazioni[i].codice_prenotazione)+1);
	}
	close(fd_file);
	return 1;
}
