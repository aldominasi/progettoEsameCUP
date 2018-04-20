#include "disponibilita.h"
#include "frdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char *FILEDB = "disponibilita.csv";
const char *FILEDB_PRENOTAZIONI = "prenotazioni.csv";

void read_from_db(struct disponibilita **lista, int *lines)
{
	int i, fd_file, j, n;
	char buff;
	struct disponibilita *lista_disponibilita;
	if((fd_file = open(FILEDB, O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	*lines = count_lines(fd_file);
	lseek(fd_file,0L,SEEK_SET);
	if(*lines > 0)
	{
		lista_disponibilita = malloc(*lines * sizeof(struct disponibilita *));
		for(i=0;i<*lines;i++)
		{
			//lista_disponibilita[i] = malloc(sizeof(struct disponibilita));
			j=0;
			read(fd_file,&buff,1);
			while(buff != ';')
			{
				lista_disponibilita[i].prestazione[j] = buff;
				read(fd_file, &buff,1);
				j++;
			}
			lista_disponibilita[i].prestazione[j] = '\0';
			//fprintf(stdout,"%s\n",lista_disponibilita[i].prestazione);
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_disponibilita[i].data[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_disponibilita[i].data[j] = '\0';
			//fprintf(stdout,"%s ",lista_disponibilita[i].data);
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_disponibilita[i].orario[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_disponibilita[i].orario[j] = '\0';
			//fprintf(stdout,"%s ",lista_disponibilita[i].orario);
			read(fd_file,&buff,1);
			j=0;
			while(buff != '\n')
			{
				if(buff != ';')
					lista_disponibilita[i].disponibile = buff;
				read(fd_file,&buff,1);
				j++;
			}
			//fprintf(stdout,"%c\n",lista_disponibilita[i].disponibile);
		}
	}
	*lista = lista_disponibilita;
	//free(lista_disponibilita);
	close(fd_file);
}

int write_into_db(struct disponibilita *lista_disponibilita, int n)
{
	int fd_file,i;
	char buff[PRESTAZIONE+1];
	if((fd_file = open(FILEDB,O_WRONLY)) == -1)
	{
		perror("open");
		return 0;
	}
	for(i = 0;i<n;i++)
	{
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].prestazione);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].data);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].orario);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%c\n",lista_disponibilita[i].disponibile);
		FullWrite(fd_file,buff,strlen(buff));
/*
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].prestazione);
		fprintf(stdout,"%s",buff);
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].data);
		fprintf(stdout,"%s",buff);
		snprintf(buff,sizeof(buff),"%s;",lista_disponibilita[i].orario);
		fprintf(stdout,"%s",buff);
		snprintf(buff,sizeof(buff),"%c\n",lista_disponibilita[i].disponibile);
		fprintf(stdout,"%s",buff);
*/
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

void read_from_db_prenotazioni(struct prenotazione **lista_prenotazione, int *lines)
{
	int i, fd_file, j, n;
	char buff;
	struct prenotazione *lista_prenotazioni;
	if((fd_file = open(FILEDB_PRENOTAZIONI, O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	*lines = count_lines(fd_file);
	lseek(fd_file,0L,SEEK_SET);
	if(*lines > 0)
	{
		lista_prenotazioni = malloc(*lines * sizeof(struct prenotazione));
		for(i=0;i<*lines;i++)
		{
			j=0;
			read(fd_file,&buff,1);
			while(buff != ';')
			{
				lista_prenotazioni[i].assistito.nome[j] = buff;
				read(fd_file, &buff,1);
				j++;
			}
			lista_prenotazioni[i].assistito.nome[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i].assistito.cognome[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].assistito.cognome[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i].prestazione[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].prestazione[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i].data_appuntamento[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].data_appuntamento[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i].orario_appuntamento[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].orario_appuntamento[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != ';')
			{
				lista_prenotazioni[i].codice_ricetta[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].codice_ricetta[j] = '\0';
			read(fd_file,&buff,1);
			j=0;
			while(buff != '\n')
			{
				if(buff != ';')
					lista_prenotazioni[i].codice_prenotazione[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].codice_prenotazione[j] = '\0';
		}
	*lista_prenotazione = lista_prenotazioni;
	}
	close(fd_file);
}

int write_into_db_prenotazioni(struct prenotazione * lista_prenotazioni, int count)
{
	int fd_file,i;
	char buff[PRESTAZIONE+1];
	if((fd_file = open(FILEDB_PRENOTAZIONI,O_WRONLY | O_TRUNC)) == -1)
	{
		perror("open");
		return 0;
	}
	printf("write_into_db_prenotazioni\n");
	for(i = 0;i<count;i++)
	{
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].assistito.nome);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].assistito.cognome);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].prestazione);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].data_appuntamento);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].orario_appuntamento);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s;",lista_prenotazioni[i].codice_ricetta);
		FullWrite(fd_file,buff,strlen(buff));
		snprintf(buff,sizeof(buff),"%s\n",lista_prenotazioni[i].codice_prenotazione);
		FullWrite(fd_file,buff,strlen(buff));
	}
	close(fd_file);
	return 1;
}
