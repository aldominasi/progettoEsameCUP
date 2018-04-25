#include "mywrapper.h"
#include "prenotazioni.h"
#include "frdwr.h"
#include <stdio.h>

#define PORTA 3001

int main(int argc, char *argv)
{
	int sockfd, count;
	struct prenotazione *lista_prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1";
	char data_prenotazioni[DATA], scelta;
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
	fprintf(stdout,"Vuoi ricevere la lista delle prenotazioni per un singolo giorno (y/n)? ");
	fscanf(stdin,"%c",&scelta);
	if(scelta == 'y')
	{
		invia_operazione(sockfd,LISTA_PRENOTAZIONI);
		invia_data(sockfd);
	}
	else
		invia_operazione(sockfd,LISTA_TUTTE_PRENOTAZIONI);
	ricevi_prenotazioni(sockfd,&lista_prenotazioni,&count);
	if(count > 0)
	{
		if(scelta == 'y' || scelta == 'Y')
			ordina_per_orario(lista_prenotazioni, count);
		else
			ordina_per_giorno_e_orario(lista_prenotazioni, count);
		visualizza_lista_prenotazioni(lista_prenotazioni,count);
		free(lista_prenotazioni);
	}
	else
		fprintf(stdout,"Non ci sono prenotazioni nella data inserita\n");
	invia_operazione(sockfd,EXIT);
	close(sockfd);
	exit(0);
}
