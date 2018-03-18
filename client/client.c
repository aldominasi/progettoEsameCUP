#include "mywrapper.h"
#include "prenotazioni.h"
#include "frdwr.h"
#include <stdio.h>

#define PORTA 3000

int main(int argc, char *argv[])
{
	int sockfd, count;
	struct prenotazione *lista_prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1";
	char data_prenotazioni[DATA], scelta;
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
}
