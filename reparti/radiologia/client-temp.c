#include <stdio.h>
#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"
#include <string.h>

#define PORTA 3001

int main(int argc, char *argv[])
{
	int i,sockfd, n, codice = LISTA_PRENOTAZIONI, length;
	struct sockaddr_in servaddr;
	char data[DATA];
	struct prenotazione *lista_prenotazioni;
	if(argc != 2)
	{
		fprintf(stdout,"usage: %s <IP_address>\n",argv[0]);
		exit(1);
	}
	sockfd = Socket(AF_INET, SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET, argv[1], PORTA, &servaddr);
	Connessione(sockfd, servaddr);
	FullWrite(sockfd,&codice,sizeof(int));
	fprintf(stdout,"Inserisci la data: ");
	fscanf(stdin,"%s",data);
	FullWrite(sockfd,data,DATA);
	while(FullRead(sockfd,&n,sizeof(int)) > 0);
	if(n > 0)
	{
		lista_prenotazioni = (struct prenotazione *)malloc(n*sizeof(struct prenotazione));
		for(i=0;i<n;i++)
			while(FullRead(sockfd,&lista_prenotazioni[i],sizeof(struct prenotazione)) > 0);
		for(i=0;i<n;i++)
			fprintf(stdout,"%s\n",lista_prenotazioni[i].codice_prenotazione);
	}
	else
		fprintf(stdout,"Non ci sono prenotazioni\n");
	close(sockfd);
	exit(1);
}
