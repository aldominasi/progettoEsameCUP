#include "lib.h"
#include "frdwr.h"
#include <string.h>
#include <stdio.h>

/*
riceve le prestazioni erogabili dal reparto
*/
void prestazioni_erogabili(int sock, char ***prestazioni, int *count)
{
	int codice_operazione = PRESTAZIONI_EROGABILI,i,length;
	char **lista;
	FullWrite(sock,&codice_operazione,sizeof(int));
	while(FullRead(sock,count,sizeof(int)) > 0);
	lista = (char**)malloc(*count * sizeof(char*));
	fprintf(stdout,"Lista delle prestazioni\n");
	for(i=0;i<*count;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		lista[i] = (char *)malloc(PRESTAZIONE*sizeof(char));
		while(FullRead(sock,lista[i],length) > 0);
		fprintf(stdout,"%s\n",lista[i]);
	}
	*prestazioni = lista;
}

/*
riceve le date disponibili in base alla prestazione inviata al reparto
*/
void ricevi_date_disponibili(int sock, struct appuntamento **date,int *count, struct prenotazione prenotazione)
{
	int i,length, operazione = DATE_DISPONIBILI;
	struct appuntamento appuntamento_ricevuto;
	struct appuntamento *appuntamenti;
	length = strlen(prenotazione.prestazione)+1;
	FullWrite(sock,&operazione,sizeof(int));
	FullWrite(sock,&length,sizeof(int)); //invia al reparto il numero di char della prestazione
	FullWrite(sock,prenotazione.prestazione,length); //invia al reparto la prestazione
	while(FullRead(sock,count,sizeof(int)) > 0); //riceve il numero di date disponibili
	if (*count > 0) //se ci sono date disponibili
	{
		appuntamenti = (struct appuntamento *)malloc(*count * sizeof(struct appuntamento));
		for(i=0;i<*count;i++)
			while(FullRead(sock,&appuntamenti[i],sizeof(struct appuntamento)) > 0); //riceve data e ora delle disponibilità
		*date = appuntamenti;
	}
}

/*
invia l'appuntameto da prenotare ed attende conferma
*/
int conferma_appuntamento(int sock, struct prenotazione prenotazione_da_confermare)
{
	int risposta,operazione = CONFERMA_DATA;
	fprintf(stdout,"Operazione di conferma dell'appuntamento: %s %s %s\n",prenotazione_da_confermare.prestazione,prenotazione_da_confermare.data_appuntamento,prenotazione_da_confermare.orario_appuntamento);
	FullWrite(sock,&operazione,sizeof(int));
	FullWrite(sock,&prenotazione_da_confermare,sizeof(struct prenotazione)); //invia l'appuntamento
	while(FullRead(sock,&risposta,sizeof(int)) > 0); //riceve l'esito dell'operazione
	return risposta;
}

/*
invia il codice di prenotazione per ottenere dal reparto le info della prenotazione ad esso associata
*/
void ricevi_info_prenotazione(int sock, char *codice_prenotazione, struct prenotazione *prenotazione, int *trovato)
{
	struct prenotazione prenot;
	FullWrite(sock,codice_prenotazione,CODICE_PRENOTAZIONE); //invia il codice della prenotazione
	while(FullRead(sock,trovato,sizeof(int)) > 0); //riceve l'esito della ricerca
	if(*trovato == 1) //se è stata trovata la prenotazione
	{
		while(FullRead(sock,&prenot,sizeof(struct prenotazione)) > 0); //riceve le info della prenotazione trovata
		*prenotazione = prenot;
	}
}

/*
invia la prenotazione da cancellare al reparto
*/
int cancella_prenotazione(int sock, struct prenotazione prenotazione)
{
	int cancellato;
	FullWrite(sock,&prenotazione,sizeof(struct prenotazione));
	while(FullRead(sock,&cancellato,sizeof(int)) > 0);
	return cancellato;
}
