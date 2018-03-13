#include <stdio.h>
#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"
#include <string.h>

#define PORTA 3001

int main(int argc, char *argv[])
{
	int i,sockfd, n, codice = DATE_DISPONIBILI, length;
	struct sockaddr_in servaddr;
	char **data, *prestazione = "TAC";
	struct appuntamento *lista_appuntamenti;
	if(argc != 2)
	{
		fprintf(stdout,"usage: %s <IP_address>\n",argv[0]);
		exit(1);
	}
	sockfd = Socket(AF_INET, SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET, argv[1], PORTA, &servaddr);
	Connessione(sockfd, servaddr);
	length = strlen(prestazione);
	FullWrite(sockfd,&codice,sizeof(int));
	FullWrite(sockfd,&length,sizeof(int));
	FullWrite(sockfd,prestazione,length);
	while(FullRead(sockfd,&n,sizeof(int)) > 0);
	if(n > 0)
	{
		lista_appuntamenti = (struct appuntamento *)malloc(n*sizeof(struct appuntamento));
		for(i=0;i<n;i++)
			while(FullRead(sockfd,&lista_appuntamenti[i],sizeof(struct appuntamento)) > 0);
		for(i=0;i<n;i++)
			fprintf(stdout,"%s %s\n",lista_appuntamenti[i].data,lista_appuntamenti[i].orario);
		free(lista_appuntamenti);
	}
	else
		fprintf(stdout,"Non ci sono prenotazioni\n");
	close(sockfd);
	exit(1);
}
