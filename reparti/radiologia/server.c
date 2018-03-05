#include "mywrapper.h"
#include "pacchetti.h"
#include "disponibilita.h"
#include "frdwr.h"

#define PORTA 3001
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	struct disponibilita lista_disponibilita[ROW];
	int listenfd, connfd, codice_comunicazione;
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
			if(codice_comunicazione == PRESTAZIONI_EROGABILI)
				invia_prestazioni_erogabili(connfd,lista_disponibilita);
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
