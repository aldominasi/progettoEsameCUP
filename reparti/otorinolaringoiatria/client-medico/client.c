#include "mywrapper.h"
#include "prenotazioni.h"
#include "frdwr.h"
#include <stdio.h>

#define PORTA 3002 //porta del server del reparto a cui fa riferimento questo client

int main(int argc, char *argv)
{
//Dichiarazioni delle variabili utilizzate
	int sockfd, count;
	struct prenotazione *lista_prenotazioni;
	struct sockaddr_in servaddr;
	char *addr = "127.0.0.1";
	char data_prenotazioni[DATA], scelta;
	sockfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoClient(AF_INET,addr,PORTA,&servaddr);
	Connessione(sockfd,servaddr);
//L'utente sceglie se ricercare le prenotazioni per un singolo giorno o visualizzarle tutte
	fprintf(stdout,"Vuoi ricevere la lista delle prenotazioni per un singolo giorno (y/n)? ");
	fscanf(stdin,"%c",&scelta);
	if(scelta == 'y') //Ricerca delle prenotazioni in un singolo giorno
	{
		invia_operazione(sockfd,LISTA_PRENOTAZIONI); //Segnala al server del reparto l'operazione che si vuole eseguire
		invia_data(sockfd); //Invia la data inserita dall'utente al server del reparto
	}
	else //Ricerca di tutte le prenotazioni del reparto
		invia_operazione(sockfd,LISTA_TUTTE_PRENOTAZIONI);
	ricevi_prenotazioni(sockfd,&lista_prenotazioni,&count); //riceve le prenotazioni dal server del reparto
	if(count > 0) //se ci sono prenotazioni
	{
		if(scelta == 'y' || scelta == 'Y') //l'utente ha scelto il giorno e quindi le prenotazioni vengono ordinate in base all'orario
			ordina_per_orario(lista_prenotazioni, count);
		else //l'utente non ha scelto la data quindi le prenotazioni vengono ordinate per giorno
			ordina_per_giorno_e_orario(lista_prenotazioni, count);
		visualizza_lista_prenotazioni(lista_prenotazioni,count); //Visualizza la lista delle prenotazioni ricevute dal server del reparto
		free(lista_prenotazioni);
	}
	else //se non ci sono prenotazioni
		fprintf(stdout,"Non ci sono prenotazioni nella data inserita\n");
	invia_operazione(sockfd,EXIT); //notifica al server la chiusura del socket
	close(sockfd);
	exit(0);
}
