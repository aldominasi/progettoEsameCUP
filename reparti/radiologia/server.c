#include "mywrapper.h"
#include "pacchetti.h"
#include "disponibilita.h"
#include "frdwr.h"

#define PORTA 3001
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	struct disponibilita lista_disponibilita[ROW];
	char prestazione_scelta[PRESTAZIONE];
	int listenfd, connfd, codice_comunicazione, length;
	pid_t pid;
	struct sockaddr_in my_addr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	AssegnaIndirizzo(listenfd,my_addr);
	Ascolto(listenfd,CLIENT_QUEUE);
	for(;;)
	{
		connfd = AccettazioneNuovaRichiesta(listenfd);
		if((pid = fork()) < 0)
		{
			perror("fork");
			close(connfd);
		}
		else if(pid > 0) //Padre
			close(connfd);
		else //figlio
		{
			close(listenfd);
			read_from_db(lista_disponibilita);
			while(FullRead(connfd,&codice_comunicazione,sizeof(int)) > 0);
			//scelta dell'operazione in base al codice_comunicazione
			//invia al server del cup la lista delle prestazioni erogabili
			if(codice_comunicazione == PRESTAZIONI_EROGABILI)
				invia_prestazioni_erogabili(connfd,lista_disponibilita);
			//invia al server del cup le date disponibili per una prestazione
			else if(codice_comunicazione == DATE_DISPONIBILI)
			{
				while(FullRead(connfd,&length,sizeof(int)) > 0);
				while(FullRead(connfd,prestazione_scelta,length) > 0);
				invia_date_disponibili(connfd, lista_disponibilita,prestazione_scelta);
			}
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
