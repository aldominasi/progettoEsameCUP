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
//Ricerca di prestazioni erogabili
		for(i=1;i<n;i++)
		{
			if(strcmp(temp, lista_disponibilita[i].prestazione) != 0)
			{
				strcpy(temp,lista_disponibilita[i].prestazione);
				j++;
				strcpy(prestazioni[j],lista_disponibilita[i].prestazione);
			}
		}
		FullWrite(sock,&count,sizeof(int)); //Invia il numero di prestazioni erogabili
		//Invia le prestazioni
		for(i=0;i<count;i++)
		{
			length = (int)strlen(prestazioni[i]);
			FullWrite(sock,&length,sizeof(int)); //invia il numero di char della prestazione
			FullWrite(sock,prestazioni[i],length);
		}
	}
	else
		FullWrite(sock,&n,sizeof(int));
	free(lista_disponibilita);
}

/*
Invia le date disponibili di una singola prestazione
*/
void invia_date_disponibili(int sock, char *prestazione)
{
	int i,count=0,n;
	struct appuntamento appuntamento_da_inviare;
	struct disponibilita *lista_disponibilita;
	read_from_db(&lista_disponibilita,&n); //Recupera dal file le disponibilita del reparto
	if(n > 0) //se ci sono date
	{
		for(i=0;i<n;i++) // calcola il numero di date disponibili
			if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && lista_disponibilita[i].disponibile == '1')
				count += 1;
		FullWrite(sock,&count,sizeof(int)); //invia il numero di date disponibili
		if(count > 0) //se ci sono date disponibili
		{
			for(i=0;i<n;i++) //invia le date disponibili
				if((strcmp(prestazione,lista_disponibilita[i].prestazione) == 0) && (lista_disponibilita[i].disponibile == '1'))
				{
					strcpy(appuntamento_da_inviare.data,lista_disponibilita[i].data);
					strcpy(appuntamento_da_inviare.orario, lista_disponibilita[i].orario);
					FullWrite(sock,&appuntamento_da_inviare,sizeof(struct appuntamento));
				}
		}
	}
	else //se non ci sono date
		FullWrite(sock,&count,sizeof(int));
	free(lista_disponibilita);
}

/*
Coferma l'appuntamento salvandolo nel file degli appuntamenti disponibili e invia la conferma
*/
int conferma_appuntamento(int sock, struct prenotazione prestazione_da_prenotare)
{
	int i,index = -1,confermato, n;
	struct disponibilita *lista_disponibilita;
	read_from_db(&lista_disponibilita,&n); //legge il file degli appuntamenti disponibili
	if(n > 0) //se ci sono appuntamenti
	{
		for(i=0;i<n;i++) //recupera l'indice dell'appuntamento
		{
			if((strcmp(lista_disponibilita[i].prestazione, prestazione_da_prenotare.prestazione) == 0) && (strcmp(lista_disponibilita[i].data,prestazione_da_prenotare.data_appuntamento) == 0) && (strcmp(lista_disponibilita[i].orario,prestazione_da_prenotare.orario_appuntamento) == 0))
			{
				index = i;
				break;
			}
		}
		if(index >= 0) //appuntamento trovato
		{
			if(lista_disponibilita[index].disponibile == '1') //se l'appuntamento è disponibile
			{
				confermato = 1; //appuntamento confermato
				lista_disponibilita[index].disponibile = '0'; //rende l'appuntamento non disponibile
				write_into_db(lista_disponibilita,n); //scrive sul file degli appuntamenti disponibili
				FullWrite(sock,&confermato,sizeof(int)); //invia al richiedente lo stato dell'operazione
			}
			else //l'appuntamento non è disponibile
			{
				confermato = -1; //appuntamento non confermato
				FullWrite(sock,&confermato,sizeof(int)); //invia al richiedente lo stato dell'operazione
			}
		}
		else //data e prestazione non trovate
		{
			confermato = 2; //appuntamento non trovato
			FullWrite(sock,&confermato,sizeof(int)); //invia al richiedente lo stato dell'operazione
		}
	}
	else
	{
		confermato = 3; //Il file degli appuntamenti disponibili è vuoto
		FullWrite(sock,&confermato,sizeof(int)); //invia al richiedente lo stato dell'operazione
	}
	free(lista_disponibilita);
	return confermato;
}

/*
Inserisce la prenotazione nell'agenda del reparto
*/
void inserisci_prenotazione_in_agenda(int sock)
{
	int i,n, conferma = 1;
	struct prenotazione prenotazione, *lista_prenotazioni, *lista;
	while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //Riceve le info sulla prenotazione
	//Legge dal file delle prenotazioni
	read_from_db_prenotazioni(&lista_prenotazioni,&n);
	n += 1;
	lista = (struct prenotazione *) malloc(n*sizeof(struct prenotazione));
	if(n > 1) //se ci sono già prenotazioni effettuate crea una lista di quest'ultime
	{
		for(i=0;i<n;i++)
			assegna_prenotazione(&lista[i],lista_prenotazioni[i]);
		free(lista_prenotazioni);
	}
	//aggiunge la nuova prenotazione
	assegna_prenotazione(&lista[n-1],prenotazione);
	//una volta aggiunta la nuova prenotazione scrive l'intera banca dati nel file
	write_into_db_prenotazioni(lista,n);
}

/*
Restituisce le informazioni riguardanti una prenotazione
*/
void informazioni_prenotazione(int sock)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count); //legge le prenotazioni presenti in agenda
	while(FullRead(sock,codice_prenotazione,CODICE_PRENOTAZIONE) > 0); //riceve il codice prenotazione che identifica la prenotazione effettuata in precedenza
	if(count > 0) //Ci sono prenotazioni effettuate
	{
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
		free(lista_prenotazioni);
	}
	else
		FullWrite(sock,&trovato,sizeof(int));
}

/*
invia la lista delle prenotazioni di una specifica data
*/
void invia_lista_prenotazioni(int sock, char *data)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count); //legge le prenotazioni
	if(count > 0) //Ci sono prenotazioni effettuate
	{
		for(i=0;i<count;i++) //controlla se ci sono appuntamenti che si riferiscono alla data ricevuta
			if(strcmp(data,lista_prenotazioni[i].data_appuntamento) == 0)
				trovato += 1;
		FullWrite(sock,&trovato,sizeof(int)); //Invia lo stato dell'operazione
		if(trovato > 0) //se ci sono prenotazioni da inviare
			for(i=0;i<count;i++) //invia le prenotazioni trovate
				if(strcmp(data,lista_prenotazioni[i].data_appuntamento) == 0)
					FullWrite(sock,&lista_prenotazioni[i],sizeof(struct prenotazione));
	}
	else //non ci sono prenotazioni
		FullWrite(sock,&count,sizeof(int));
	free(lista_prenotazioni);
}

/*
invia tutte le prenotazioni del reparto
*/
void invia_lista_prenotazioni_senza_data(int sock)
{
	int i,count,trovato = 0;
	struct prenotazione *lista_prenotazioni;
	char codice_prenotazione[CODICE_PRENOTAZIONE];
	read_from_db_prenotazioni(&lista_prenotazioni,&count); //legge le prenotazioni
	FullWrite(sock,&count,sizeof(int));
	if(count > 0) //Ci sono prenotazioni effettuate
		for(i=0;i<count;i++) //invia le prenotazioni
			FullWrite(sock,&lista_prenotazioni[i],sizeof(struct prenotazione));
	free(lista_prenotazioni);
}

void assegna_prenotazione(struct prenotazione *prenotazioneA,struct prenotazione prenotazioneB)
{
	snprintf(prenotazioneA->assistito.nome, sizeof(prenotazioneA->assistito.nome), "%s",prenotazioneB.assistito.nome);
	strcpy(prenotazioneA->assistito.cognome, prenotazioneB.assistito.cognome);
	strcpy(prenotazioneA->prestazione, prenotazioneB.prestazione);
	strcpy(prenotazioneA->data_appuntamento, prenotazioneB.data_appuntamento);
	strcpy(prenotazioneA->orario_appuntamento, prenotazioneB.orario_appuntamento);
	strcpy(prenotazioneA->codice_ricetta, prenotazioneB.codice_ricetta);
	strcpy(prenotazioneA->codice_prenotazione, prenotazioneB.codice_prenotazione);
}

/*
cancella la prenotazione inviata e successivamente trovata
*/
void cancella_prenotazione(int sock)
{
	int cancellato=0, i, count, j=-1, n;
	struct prenotazione prenotazione, *lista_prenotazioni;
	struct disponibilita *lista_disponibilita;
	while(FullRead(sock,&prenotazione,sizeof(struct prenotazione)) > 0); //riceve la prenotazione da cancellare
	read_from_db_prenotazioni(&lista_prenotazioni,&count); //legge le prenotazioni
	read_from_db(&lista_disponibilita,&n); //legge gli appuntamenti disponibili
	for(i=0;i<count;i++) //se ci sono prenotazioni
	{
		if(strcmp(lista_prenotazioni[i].codice_prenotazione,prenotazione.codice_prenotazione) == 0) //cerca l'indice della prenotazione da cancellare
		{
			j=i;
			break;
		}
	}
	if(j>-1) //se la prenotazione è stata trovata viene cancellata e viene reso disponibile il relativo appuntamento
	{
		for(i=j;i<count-1;i++)
			lista_prenotazioni[i] = lista_prenotazioni[i+1];
		count -= 1;
		write_into_db_prenotazioni(lista_prenotazioni,count);
		cancellato = 1;
		free(lista_prenotazioni);
		for(i=0;i<n;i++)
		{
			if((strcmp(lista_disponibilita[i].data,prenotazione.data_appuntamento) == 0) && (strcmp(lista_disponibilita[i].orario,prenotazione.orario_appuntamento) == 0) &&  (lista_disponibilita[i].disponibile == '0'))
			{
				lista_disponibilita[i].disponibile = '1';
				break;
			}
		}
		write_into_db(lista_disponibilita,n);
		free(lista_disponibilita);
	}
	FullWrite(sock,&cancellato,sizeof(int)); //invia lo stato dell'operazione
}
