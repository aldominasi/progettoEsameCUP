#include "disponibilita.h"
#include "frdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
invia le prestazioni erogabili della struttura.
*/
void invia_prestazioni_erogabili(int sock)
{
	/*
	prestazioni contiene tutte le prestazioni erogabili da inviare, temp viene utilizzata per
	confrontare le prestazioni prima di inserirle nell'array da inviare
	*/
	char prestazioni[N_PRESTAZIONI][PRESTAZIONE],temp[PRESTAZIONE];
	int i,j=0, count = N_PRESTAZIONI, length, n;
	struct disponibilita *lista_disponibilita; //lista delle disponibilita
	read_from_db(&lista_disponibilita,&n); //Recupera dal file le disponibilita del reparto
	if (n > 0) //n contiene il numero di disponibilita presenti in lista_disponibilita
	{
		strcpy(temp,lista_disponibilita[0].prestazione);
		strcpy(prestazioni[0],lista_disponibilita[0].prestazione);
		//fprintf(stdout,"%s\n",prestazioni[0]);
		for(i=1;i<n;i++)
		{
			//fprintf(stdout,"proviamo %s\n",lista_disponibilita[i].prestazione);
			if(strcmp(temp, lista_disponibilita[i].prestazione) != 0)
			{
				strcpy(temp,lista_disponibilita[i].prestazione);
				j++;
				strcpy(prestazioni[j],lista_disponibilita[i].prestazione);
			}
		}
		FullWrite(sock,&count,sizeof(int)); //Invia il numero di prestazioni
		//Invia le prestazioni
		for(i=0;i<count;i++)
		{
			//fprintf(stdout,"%s\n",prestazioni[i]);
			length = (int)strlen(prestazioni[i]);
			FullWrite(sock,&length,sizeof(int));
			FullWrite(sock,prestazioni[i],length);
		}
	}
	else
		FullWrite(sock,&n,sizeof(int));
	free(lista_disponibilita); //Dealloca la memoria
}

/*
Invia le date disponibili
*/
void invia_date_disponibili(int sock, char *prestazione)
{
	int i,count=0,n;
	struct appuntamento appuntamento_da_inviare;
	struct disponibilita *lista_disponibilita;
	read_from_db(&lista_disponibilita,&n); //Recupera dal file le disponibilita del reparto
	if(n > 0)
	{
		for(i=0;i<n;i++)
			if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && lista_disponibilita[i].disponibile == '1')
				count += 1;
		FullWrite(sock,&count,sizeof(int));
		if(count > 0)
		{
			for(i=0;i<n;i++)
				if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && (lista_disponibilita[i].disponibile == '1'))
				{
					strcpy(appuntamento_da_inviare.data,lista_disponibilita[i].data);
					strcpy(appuntamento_da_inviare.orario, lista_disponibilita[i].orario);
					FullWrite(sock,&appuntamento_da_inviare,sizeof(struct appuntamento));
				}
		}
	}
	else
		FullWrite(sock,&count,sizeof(int));
	free(lista_disponibilita);
}

/*
Coferma l'appuntamento salvandolo nel file delle prenotazioni e invia la conferma
*/
void conferma_appuntamento(int sock, struct prenotazione prestazione_da_prenotare)
{
	int i,index = -1,confermato, n;
	struct disponibilita *lista_disponibilita;
	read_from_db(&lista_disponibilita,&n);
	if(n > 0)
	{
		for(i=0;i<n;i++)
		{
			if((strcmp(lista_disponibilita[i].prestazione, prestazione_da_prenotare.prestazione) == 0) && (strcmp(lista_disponibilita[i].data,prestazione_da_prenotare.data_appuntamento) == 0) && (strcmp(lista_disponibilita[i].orario,prestazione_da_prenotare.orario_appuntamento) == 0))
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
				write_into_db(lista_disponibilita,n);
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
	else
	{
		confermato = 3;
		FullWrite(sock,&confermato,sizeof(int));
	}
	free(lista_disponibilita);
}

void inserisci_prenotazione_in_agenda(int sock)
{
	int i,n, conferma = 1;
	struct prenotazione prenotazione, *lista_prenotazioni;
	while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //Riceve le info sulla prenotazione
	//Leggi dal file degli appuntamenti
	read_from_db_prenotazioni(&lista_prenotazioni,&n);
	n += 1;
	lista_prenotazioni = (struct prenotazione *)realloc(lista_prenotazioni,n*sizeof(struct prenotazione));
	assegna_prenotazione(&lista_prenotazioni[n],prenotazione);
	ordina_per_giorno_e_orario(&lista_prenotazioni,n);
	//una volta aggiunta la nuova prenotazione scrive l'intera banca dati nel file
	write_into_db_prenotazioni(lista_prenotazioni,n);
	FullWrite(sock,&conferma,sizeof(int));
}


void cancella_prenotazione(int sock, char *codice_prenotazione)
{
	int i, c, count, trovato = 0,j, n;
	struct prenotazione *lista_prenotazioni, *lista_prenotazioni_aggiornata;
	struct disponibilita *lista_disponibilita; //Legge il file delle prenotazioni
	read_from_db_prenotazioni(&lista_prenotazioni,&count);
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
			read_from_db(&lista_disponibilita, &n);
			for(j=0;j<n;j++)
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
			write_into_db(lista_disponibilita,n);
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

void informazioni_prenotazione(int sock)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count);
	FullWrite(sock,&count,sizeof(int));
	if(count > 0) //Ci sono prenotazioni effettuate
	{
		while(FullRead(sock,codice_prenotazione,CODICE_PRENOTAZIONE) > 0);
		for(i=0;i<count;i++)
		{
			if(strcmp(codice_prenotazione,lista_prenotazioni[i].codice_prenotazione) == 0)
			{
				trovato = 1;
				FullWrite(sock,&trovato,sizeof(int));
				FullWrite(sock,&lista_prenotazioni[i],sizeof(struct prenotazione));
				break;
			}
		}
		if(trovato == 0) //Non è stata trovata la prenotazione
			FullWrite(sock,&trovato,sizeof(int));
	}
	free(lista_prenotazioni);
}

void invia_lista_prenotazioni(int sock, char *data)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count);
	if(count > 0) //Ci sono prenotazioni effettuate
	{
		for(i=0;i<count;i++)
		{
			if(strcmp(data,lista_prenotazioni[i].data_appuntamento) == 0)
			{
				trovato += 1;
			}
		}
		FullWrite(sock,&trovato,sizeof(int));
		if(trovato > 0)
		{
			for(i=0;i<count;i++)
				if(strcmp(data,lista_prenotazioni[i].data_appuntamento) == 0)
					FullWrite(sock,&lista_prenotazioni[i],sizeof(struct prenotazione));
		}
	}
	else
		FullWrite(sock,&count,sizeof(int));
	free(lista_prenotazioni);
}

void invia_lista_prenotazioni_senza_data(int sock)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count);
	FullWrite(sock,&count,sizeof(int));
	if(count > 0) //Ci sono prenotazioni effettuate
	{
		for(i=0;i<count;i++)
			FullWrite(sock,&lista_prenotazioni[i],sizeof(struct prenotazione));
	}
	free(lista_prenotazioni);
}

void ordina_per_giorno_e_orario(struct prenotazione **lista_prenotazioni, int n)
{
	struct prenotazione temp;
	int i,j;
	for(i=1;i<n;i++)
	{
		assegna_prenotazione(&temp,*lista_prenotazioni[i]);
		j=i-1;
		while(j>=-1 && (confronta_giorno(lista_prenotazioni[j]->orario_appuntamento, lista_prenotazioni[i]->orario_appuntamento) == 2)) 
		{
			assegna_prenotazione(&(*lista_prenotazioni[j+1]),*lista_prenotazioni[j]);
			j -= 1;
		}
		assegna_prenotazione(&(*lista_prenotazioni[j+1]),temp);
	}
}

int confronta_giorno(char *prenotazioneA,char *prenotazioneB)
{
	int giornoA,giornoB,meseA,meseB,annoA,annoB;
	converti_data(&giornoA,&meseA,&annoA,prenotazioneA);
	converti_data(&giornoB,&meseB,&annoB,prenotazioneB);
	if(annoA < annoB)
		return 1;
	else if(annoA > annoB)
		return 2;
	if(meseA < meseB)
		return 1;
	else if(meseA > meseB)
		return 2;
	if(giornoA < giornoB)
		return 1;
	else if(giornoA > giornoB)
		return 2;
	return 0;
}

void converti_data(int *giorno,int *mese, int *anno, char *data)
{
	char day[3],month[3],year[5];
	if(data[0] == '0')
		*giorno = (int)(data[1] - '0');
	else
	{
		memcpy(day,&data[0],2);
		day[2] = '\0';
		*giorno = atoi(day);
	}
	if(data[3] == '0')
		*mese = (int)(data[4] - '0');
	else
	{
		memcpy(month,&data[3],2);
		month[2] = '\0';
		*mese = atoi(month);
	}
	memcpy(year,&data[6],4);
	year[4] = '\0';
	*anno = atoi(year);
}

void assegna_prenotazione(struct prenotazione *prenotazioneA,struct prenotazione prenotazioneB)
{
	strcpy(prenotazioneA->assistito.nome, prenotazioneB.assistito.nome);
	strcpy(prenotazioneA->assistito.cognome, prenotazioneB.assistito.cognome);
	strcpy(prenotazioneA->prestazione, prenotazioneB.prestazione);
	strcpy(prenotazioneA->data_appuntamento, prenotazioneB.data_appuntamento);
	strcpy(prenotazioneA->orario_appuntamento, prenotazioneB.orario_appuntamento);
	strcpy(prenotazioneA->codice_ricetta, prenotazioneB.codice_ricetta);
	strcpy(prenotazioneA->codice_prenotazione, prenotazioneB.codice_prenotazione);
}
