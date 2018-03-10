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

void conferma_appuntamento(int sock, struct disponibilita prestazione_da_prenotare)
{
	int i,index = -1,confermato;
	struct disponibilita lista_disponibilita[ROW];
	read_from_db(lista_disponibilita);
	for(i=0;i<ROW;i++)
	{
		if((strcmp(lista_disponibilita[i].prestazione, prestazione_da_prenotare.prestazione) == 0) && (strcmp(lista_disponibilita[i].data,prestazione_da_prenotare.data) == 0))
		{
			index = i;
			break;
		}
	}
	if(index >= 0) //data e prestazione trovate
	{
		if(lista_disponibilita[index].disponibile == '1') //l'appuntamento è disponibile
		{
			confermato = 1; //appuntamento confermato
			lista_disponibilita[index].disponibile = '0';
			write_into_db(lista_disponibilita);
			FullWrite(sock,&confermato,sizeof(int));
		}
		else //l'appuntamento non è disponibile
		{
			confermato = 0; //appuntamento non confermato
			FullWrite(sock,&confermato,sizeof(int));
		}
	}
	else //data e prestazione non trovate
	{
		confermato = 2; //appuntamento non trovato
		FullWrite(sock,&confermato,sizeof(int));
	}
}

void inserisci_prenotazione_in_agenda(int sock)
{
	int i,n, conferma = 1;
	struct prenotazione prenotazione, *lista_prenotazioni;
	while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //Riceve le info sulla prenotazione
	//Leggi dal file degli appuntamenti
	read_from_db_prenotazioni(lista_prenotazioni,&n);
	//bisogna inserire nel posto giusto la nuova prenotazione
	
	//una volta aggiunta la nuova prenotazione scrive l'intera banca dati nel file
	write_into_db_prenotazioni(lista_prenotazioni,n);
	FullWrite(sock,&conferma,sizeof(int));
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

void read_from_db_prenotazioni(struct prenotazione *lista_prenotazioni, int *lines)
{
	int i, fd_file, j, n;
	char buff;
	if((fd_file = open(FILEDB_PRENOTAZIONI, O_RDONLY)) == -1)
	{
		perror("open");
		exit(1);
	}
	*lines = count_lines(fd_file);
	lista_prenotazioni = (struct prenotazione *)malloc(*lines * sizeof(struct prenotazione));
	if(lines > 0)
	{
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
				lista_prenotazioni[i].data_appuntamento[j] = buff;
				read(fd_file,&buff,1);
				j++;
			}
			lista_prenotazioni[i].data_appuntamento[j] = '\0';
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


void cancella_prenotazione(int sock, char *codice_prenotazione)
{
	int i, c, count, trovato = 0,j;
	struct prenotazione *lista_prenotazioni, *lista_prenotazioni_aggiornata;
	struct disponibilita lista_disponibilita[ROW]; //Legge il file delle prenotazioni
	read_from_db_prenotazioni(lista_prenotazioni,&count);
	if(count > 0)
	{
		for (i=0;i<count;i++)
		{	
			if(strcmp(lista_prenotazioni[i].codice_prenotazione,codice_prenotazione) == 0)
			{
				trovato = 1;
				break;
			}
		}
		if(trovato == 1) //trovata la prenotazione
		{
			read_from_db(lista_disponibilita);
			for(j=0;j<ROW;j++)
			{
				if((strcmp(lista_prenotazioni[i].prestazione, lista_disponibilita[j].prestazione) == 0) && (strcmp(lista_prenotazioni[i].data_appuntamento, lista_disponibilita[j].data) == 0) && (strcmp(lista_prenotazioni[i].orario_appuntamento,lista_disponibilita[j].orario) == 0)) //trovata la disponibilita da settare a 1 per poterla rendere prenotabile
				{
					lista_disponibilita[j].disponibile = '1';
				}
			}
			lista_prenotazioni_aggiornata = (struct prenotazione *)malloc((count - 1) * sizeof(struct prenotazione));
			for(c=0;c<count;c++)
			{
				if(c < i)
				{
					strcpy(lista_prenotazioni_aggiornata[c].assistito.nome,lista_prenotazioni[c].assistito.nome);
					strcpy(lista_prenotazioni_aggiornata[c].assistito.cognome, lista_prenotazioni[c].assistito.cognome);
					strcpy(lista_prenotazioni_aggiornata[c].prestazione,lista_prenotazioni[c].prestazione);
					strcpy(lista_prenotazioni_aggiornata[c].data_appuntamento,lista_prenotazioni[c].data_appuntamento);
					strcpy(lista_prenotazioni_aggiornata[c].orario_appuntamento,lista_prenotazioni[c].orario_appuntamento);
					strcpy(lista_prenotazioni_aggiornata[c].codice_ricetta,lista_prenotazioni[c].codice_ricetta);
					strcpy(lista_prenotazioni_aggiornata[c].codice_prenotazione,lista_prenotazioni[c].codice_prenotazione);
				}
				else if(c > i)
				{
					strcpy(lista_prenotazioni_aggiornata[c-1].assistito.nome,lista_prenotazioni[c].assistito.nome);
					strcpy(lista_prenotazioni_aggiornata[c-1].assistito.cognome, lista_prenotazioni[c].assistito.cognome);
					strcpy(lista_prenotazioni_aggiornata[c-1].prestazione,lista_prenotazioni[c].prestazione);
					strcpy(lista_prenotazioni_aggiornata[c-1].data_appuntamento,lista_prenotazioni[c].data_appuntamento);
					strcpy(lista_prenotazioni_aggiornata[c-1].orario_appuntamento,lista_prenotazioni[c].orario_appuntamento);
					strcpy(lista_prenotazioni_aggiornata[c-1].codice_ricetta,lista_prenotazioni[c].codice_ricetta);
					strcpy(lista_prenotazioni_aggiornata[c-1].codice_prenotazione,lista_prenotazioni[c].codice_prenotazione);
				}
			}
			write_into_db_prenotazioni(lista_prenotazioni_aggiornata,count-1);
			write_into_db(lista_disponibilita);
			FullWrite(sock,&trovato,sizeof(int));
			free(lista_prenotazioni_aggiornata);
			free(lista_prenotazioni);
		}
		else //Prenotazione non trovata
		{
			trovato = 2;
			FullWrite(sock,&trovato,sizeof(int));
		}
	}
	else
		FullWrite(sock,&trovato,sizeof(int)); //Non Ci sono prenotazioni
}
