#include "lib.h"
#include "frdwr.h"
#include <string.h>
#include <stdio.h>

void prestazioni_erogabili(int sock, char ***prestazioni, int *count)
{
	int codice_operazione = PRESTAZIONI_EROGABILI,i,length;
	char **lista;
	FullWrite(sock,&codice_operazione,sizeof(int));
	while(FullRead(sock,count,sizeof(int)) > 0);
	//fprintf(stdout,"%d\n",*count);
	lista = (char**)malloc(*count * sizeof(char*));
	for(i=0;i<*count;i++)
	{
		while(FullRead(sock,&length,sizeof(int)) > 0);
		lista[i] = (char *)malloc(PRESTAZIONE*sizeof(char));
		while(FullRead(sock,lista[i],length) > 0);
		fprintf(stdout,"%s\n",lista[i]);
	}
	*prestazioni = lista;
}

void ricevi_date_disponibili(int sock, struct appuntamento **date,int *count, struct prenotazione prenotazione)
{
	int i,length, operazione = DATE_DISPONIBILI;
	struct appuntamento appuntamento_ricevuto;
	struct appuntamento *appuntamenti;
	fprintf(stdout,"ricevi_date_appuntamento %s\n",prenotazione.prestazione);
	length = strlen(prenotazione.prestazione)+1;
	FullWrite(sock,&operazione,sizeof(int));
	FullWrite(sock,&length,sizeof(int));
	FullWrite(sock,prenotazione.prestazione,length);
	while(FullRead(sock,count,sizeof(int)) > 0);
	if (*count > 0)
	{
		appuntamenti = (struct appuntamento *)malloc(*count * sizeof(struct appuntamento));
		for(i=0;i<*count;i++)
			while(FullRead(sock,&appuntamenti[i],sizeof(struct appuntamento)) > 0);
		*date = appuntamenti;
	}
}

int conferma_appuntamento(int sock, struct prenotazione prenotazione_da_confermare)
{
	int risposta;
	FullWrite(sock,&prenotazione_da_confermare,sizeof(struct prenotazione));
	while(FullRead(sock,&risposta,sizeof(int)) > 0);
	if (risposta == 1)
		return 1;
	else
		return 0;
}

void invia_prenotazione_completa(int sock, struct prenotazione prenotazione)
{
	FullWrite(sock,&prenotazione,sizeof(struct prenotazione));
}
