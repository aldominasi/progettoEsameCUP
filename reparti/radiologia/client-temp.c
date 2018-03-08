#include <stdio.h>
#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"
#include <string.h>

#define PORTA 3001

int main(int argc, char *argv[])
{
	int i,sockfd, n, codice = PRESTAZIONI_EROGABILI, length;
	struct sockaddr_in servaddr;
	char **bufferRead;
	if(argc != 2)
	{
		fprintf(stdout,"usage: %s <IP_address>\n",argv[0]);
		exit(1);
	}
	sockfd = Socket(AF_INET, SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET, argv[1], PORTA, &servaddr);
	Connessione(sockfd, servaddr);
	FullWrite(sockfd,&codice,sizeof(int));
	while(FullRead(sockfd,&n,sizeof(int)) > 0);
	bufferRead = (char **)malloc(n*sizeof(char *));
	for(i=0;i<n;i++)
	{
		while(FullRead(sockfd,&length,sizeof(int)) > 0);
		bufferRead[i] = (char *)malloc(length*sizeof(char));
		while(FullRead(sockfd,bufferRead[i],length) > 0);
	}
	for(i=0;i<n;i++)
		fprintf(stdout,"%s\n",bufferRead[i]);
	close(sockfd);
	exit(1);
}
