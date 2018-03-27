#include "mywrapper.h"
#include "lib.h"
#include "frdwr.h"
#include <stdio.h>

#define PORTA 3000

int main(int argc, char *argv[])
{
	int sockfd, count, codice_operazione;
	struct prenotazione *prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1";
	char data_prenotazioni[DATA], scelta;
	char *reparto;
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
	scegli_reparto(sockfd,&reparto);
	codice_operazione = scegli_operazione(sockfd);
	invia_operazione(sockfd,codice_operazione);
	switch(codice_operazione)
	{
		case PRENOTA:
			prenota(sockfd,reparto);
			invia_operazione(sockfd, EXIT);
			close(sockfd);
			exit(1);
			break;
		case CANCELLA_PRENOTAZIONE:
			cancella_prenotazione(sockfd);
			invia_operazione(sockfd, EXIT);
			close(sockfd);
			exit(1);
			break;
		case INFO_PRENOTAZIONE:
			info_prenotazione(sockfd);
			invia_operazione(sockfd, EXIT);
			close(sockfd);
			exit(1);
			break;
		case LISTA_PRENOTAZIONI_UTENTE:
			lista_prenotazioni(sockfd);
			invia_operazione(sockfd, EXIT);
			close(sockfd);
			exit(1);
			break;
		case EXIT:
			close(sockfd);
			exit(1);
			break;
	}
}
