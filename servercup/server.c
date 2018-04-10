#include "mywrapper.h"
#include "frdwr.h"
#include "lib.h"
#include <time.h>
#include <string.h>

#define PORTA 3000
#define CLIENT_QUEUE 100

int main(int argc, char *argv[])
{
	int listenfd, connfd, enabled = 1, operazione, reparto,scelta_prestazione,n_prestazioni, sockreparto;
	int n_appuntamenti, scelta_appuntamento, appuntamento_confermato;
	char **prestazioni;
	struct appuntamento *appuntamenti;
	struct prenotazione prenotazione;
	pid_t pid;
	struct sockaddr_in my_addr, addr_reparto;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	ImpostaIndirizzoAnyServer(AF_INET,PORTA,&my_addr);
	srand((unsigned)time(NULL));
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
	{
		perror("setsockop");
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
			exit(1);
		}
		else if(pid > 0) //Padre
			close(connfd);
		else //Figlio
		{
			close(listenfd);
			reparto = scelta_reparto(connfd);
			sockreparto = Socket(AF_INET,SOCK_STREAM,0);
			ImpostaIndirizzoClient(AF_INET,"127.0.0.1",reparto,&addr_reparto);
			Connessione(sockreparto,addr_reparto);
			do
			{
				while(FullRead(connfd,&operazione,sizeof(int)) > 0);
				switch(operazione)
				{
					case PRENOTA:
						prestazioni_erogabili(sockreparto,&prestazioni,&n_prestazioni);
						scelta_prestazione = invia_prestazioni_erogabili(connfd,prestazioni,n_prestazioni);
						strcpy(prenotazione.prestazione,prestazioni[scelta_prestazione]);
						ricevi_date_disponibili(sockreparto,&appuntamenti,&n_appuntamenti, prenotazione);
						scelta_appuntamento = scelta_data_orario_disponibile(connfd,appuntamenti,n_appuntamenti);
						strcpy(prenotazione.data_appuntamento,appuntamenti[scelta_appuntamento].data);
						strcpy(prenotazione.orario_appuntamento,appuntamenti[scelta_appuntamento].orario);
						appuntamento_confermato = conferma_appuntamento(sockreparto,prenotazione);
						invia_conferma_data(connfd,appuntamento_confermato);
						while(FullRead(connfd,&prenotazione,sizeof(struct prenotazione)) > 0);
						genera_codice_prenotazione(prenotazione.codice_prenotazione);
						FullWrite(connfd,&prenotazione,sizeof(struct prenotazione));
						break;
				}
			} while(operazione != EXIT);
			FullWrite(connfd,&operazione,sizeof(int));
			close(sockreparto);
			close(connfd);
			exit(0);
		}
	}
	exit(0);
}
