#include "frdwr.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>

/*
invia i reparti disponibili al client ed attende la sua scelta
*/
int scelta_reparto(int sock)
{
	int n, i, length, reparto_scelto;
	struct reparti *lista_reparti;
	read_from_db_reparti(&lista_reparti,&n); //legge dal file i reparti disponibili
	FullWrite(sock,&n,sizeof(int)); //invia il numero di reparti disponibili
	//invia i reparti disponibili al client
	for(i=0;i<n;i++)
	{
		length = strlen(lista_reparti[i].reparto) + 1;
		FullWrite(sock,&length,sizeof(int));
		FullWrite(sock,lista_reparti[i].reparto,length);
	}
	while(FullRead(sock,&reparto_scelto,sizeof(int)) > 0); //riceve la scelta del client
	return atoi(lista_reparti[reparto_scelto].porta);
}

/*
invia al client le prestazioni ricevute in precedenza dal reparto
*/
int invia_prestazioni_erogabili(int sock, char **prestazioni, int n)
{
	int i, length, scelta;
	FullWrite(sock,&n,sizeof(int)); //invia al client il numero di prestazioni
	//invia al client le prestazioni
	for(i=0;i<n;i++)
	{
		length = strlen(prestazioni[i]) + 1;
		FullWrite(sock,&length,sizeof(int));
		FullWrite(sock,prestazioni[i],length);
	}
	while(FullRead(sock,&scelta,sizeof(int)) > 0); //riceve la scelta del client
	return scelta;
}

/*
invia al client le disponibilità
*/
int scelta_data_orario_disponibile(int sock, struct appuntamento *date, int n)
{
	int i, scelta,count_orari=0;
	FullWrite(sock,&n,sizeof(int)); //invia il numero di disponibilità
	//invia le disponibilità
	for(i=0;i<n;i++)
		FullWrite(sock,&date[i],sizeof(struct appuntamento));
	while(FullRead(sock,&scelta,sizeof(int)) > 0); //riceve la scelta del client
	return scelta;
}

void invia_conferma_data(int sock,int conferma)
{
	FullWrite(sock,&conferma,sizeof(int));
}
