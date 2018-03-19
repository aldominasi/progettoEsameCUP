#include "mywrapper.h"
#include "prenotazioni.h"
#include "frdwr.h"
#include <stdio.h>

#define PORTA 3000

int main(int argc, char *argv[])
{
	int sockfd, count, codice_operazione;
	struct prenotazione *lista_prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1";
	char data_prenotazioni[DATA], scelta;
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
	scegli_reparto(sockfd);
	codice_operazione = scegli_operazione(sockfd);
	switch(codice_operazione)
	{
		case PRENOTA:
			prenota(sockfd);
			break;
		case CANCELLA_PRENOTAZIONE:
			cancella_prenotazione(sockfd);
			break;
		case INFO_PRENOTAZIONE:
			info_prenotazione(sockfd);
			break;
		case LISTA_PRENOTAZIONI_UTENTE:
			lista_prenotazioni(sockfd);
			break;
		case EXIT:
			invia_operazione(sockfd, EXIT);
			close(sockfd);
			exit(1);
			break;
	}
}
