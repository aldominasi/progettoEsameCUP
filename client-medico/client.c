#include "mywrapper.h"
#include "disponibilita.h"
#include <stdio.h>

#define PORTA 3001
#define LISTA_PRENOTAZIONI 5
#define DATA 11

int main(int argc, char *argv)
{
	int sockfd, i, operazione = LISTA_PRENOTAZIONI, count;
	struct prenotazione *lista_prenotazioni;
	struct sockaddr_in servaddr;
	const char *addr = "127.0.0.1";
	char data_prenotazioni[DATA];
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
	fprintf(stdout,"Inserisci la data per la ricerca di prenotazioni (DD/MM/YYYY): ");
	fscanf(stdin,"%s",data_prenotazioni);
	data_prenotazioni[10] = '\0';
	FullWrite(sockfd,&operazione,sizeof(int));
	FullWrite(sockfd,data_prenotazioni,DATA);
	while(FullRead(sockfd,&count,sizeof(int)) > 0);
	if(count > 0) //Ci sono prenotazioni
	{
		lista_prenotazioni = (struct prenotazione *) malloc(count * sizeof(struct prenotazione));
		for(i=0;i<count;i++)
			fprintf(stdout,"%s %s %s %s\n", lista_prenotazioni[i].codice_prenotazione, lista_prenotazioni[i].orario_appuntamento, lista_prenotazioni[i].assistito.nome,lista_prenotazioni[i].assistito.cognome);
	}
	else
		fprintf(stdout,"Non ci sono prenotazioni nella data inserita\n");
	close(sockfd);
	exit(0);
}
