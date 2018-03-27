#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

int scelta_reparto(int sock)
{
	int n, i, length, reparto_scelto;
	struct reparti *lista_reparti;
	read_from_db_reparti(&lista_reparti,&n);
	FullWrite(sock,&n,sizeof(int));
	for(i=0;i<n;i++)
	{
		length = strlen(lista_reparti[i].reparto) + 1;
		FullWrite(sock,&length,sizeof(int));
		FullWrite(sock,lista_reparti[i].reparto,length);
	}
	while(FullRead(sock,&reparto_scelto,sizeof(int)) > 0);
	return atoi(lista_reparti[reparto_scelto].porta);
}

int invia_prestazioni_erogabili(int sock, char **prestazioni, int n)
{
	int i, length, scelta;
	FullWrite(sock,&n,sizeof(int));
	for(i=0;i<n;i++)
	{
		length = strlen(prestazioni[i]) + 1;
		FullWrite(sock,prestazioni[i],length);
	}
	while(FullRead(sock,&scelta,sizeof(int)) > 0);
	return scelta;
}

int scelta_data_orario_disponibile(int sock, struct appuntamento *date, int n)
{
	int i, scelta,count_orari=0;
	FullWrite(sock,&n,sizeof(int));
	for(i=0;i<n;i++)
		FullWrite(sock,&date[i],sizeof(struct appuntamento));
	while(FullRead(sock,&scelta,sizeof(int)) > 0);
	return scelta;
}

void invia_conferma_data(int sock,int conferma)
{
	FullWrite(sock,&conferma,sizeof(int));
}

void genera_codice_prenotazione(char *codice)
{
	char i;
	for(i=0;i<CODICE_PRENOTAZIONE;i++)
		codice[i] = (char)((rand()%10)+48);
}

void invia_codice_prenotazione_client(int sock,char *codice)
{
	FullWrite(sock,codice,CODICE_PRENOTAZIONE);
}
