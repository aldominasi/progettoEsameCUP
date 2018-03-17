#include "mywrapper.h"
#include "disponibilita.h"
#include "frdwr.h"

#define PORTA 3001
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	char prestazione_scelta[PRESTAZIONE], data[DATA];
	int listenfd, connfd, codice_comunicazione = 9, length, enabled = 1;
	pid_t pid;
	struct sockaddr_in my_addr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}
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
			while(codice_comunicazione > 0)
			{
				while(FullRead(connfd,&codice_comunicazione,sizeof(int)) > 0);
				//scelta dell'operazione in base al codice_comunicazione
				//invia al server del cup la lista delle prestazioni erogabili
				if(codice_comunicazione == PRESTAZIONI_EROGABILI)
					invia_prestazioni_erogabili(connfd);
				//invia al server del cup le date disponibili per una prestazione
				else if(codice_comunicazione == DATE_DISPONIBILI)
				{
					while(FullRead(connfd,&length,sizeof(int)) > 0);
					while(FullRead(connfd,prestazione_scelta,length) > 0);
					invia_date_disponibili(connfd,prestazione_scelta);
				}
				else if(codice_comunicazione == LISTA_PRENOTAZIONI)
				{
					while(FullRead(connfd,&data,DATA) > 0);
					fprintf(stdout,"%s\n",data);
					invia_lista_prenotazioni(connfd,data);
				}
			}
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
